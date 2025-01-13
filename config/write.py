import argparse

item_definition = """
    Item item[] = {{0x00, 0x0000}, {0x01, 0x0001}, {0x02, 0x0002}, {0x03, 0x0003}, {0x05, 0x0005}};
"""

channel_data = """
PowerService::PowerInfo powerChannel[10] = {{0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 0, 0}};
"""

def main():
    # Khởi tạo đối tượng ArgumentParser
    parser = argparse.ArgumentParser(description="Chương trình ví dụ sử dụng argparse")

    # Thêm các tùy chọn dòng lệnh
    parser.add_argument("-wifi", type=str, help="SSID của mạng WiFi")
    parser.add_argument("-password", type=str, help="Mật khẩu của mạng WiFi")
    parser.add_argument("-num_connector", type=int, help="Số lượng vòi sạc")
    parser.add_argument(
        "-backend_url", type=str, help="Đường dẫn websocket vào trong server"
    )
    parser.add_argument(
        "-charge_box_id", type=str, help="Mã của trụ sạc đại diện trong server"
    )
    parser.add_argument(
        "-file", type=str, default="output.txt", help="Đường dẫn file để ghi thông tin"
    )

    # Phân tích các đối số dòng lệnh
    args = parser.parse_args()

    # Chuẩn bị dữ liệu để ghi
    data = f"""
#include <MicroOcpp.h>
#include <MicroOcpp/Debug.h>
#include <SPIFFS.h>
#include <vector>
#define NUM_CONNECTOR {args.num_connector}
#define MAX_TIME_TRY 20
#define HIGH_LIMIT_POWER 2000
#define HIGH_LIMIT_AMPE 9
#define LOW_LIMIT_POWER 10
#define LOW_LIMIT_POWER_TIME_TRY 25
#define START_OF_MESSAGE 0xA5
#define MAX_ATTEMPTS 5
#define READ_DELAY 200
#if defined(ESP32)
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>
#else
#error only ESP32 or ESP8266 supported at the moment
#endif

#define STASSID "{args.wifi}"
#define STAPSK "{args.password}"

#define OCPP_BACKEND_URL "{args.backend_url}"
#define OCPP_CHARGE_BOX_ID "{args.charge_box_id}"

using namespace std;

uint8_t cnt[5], counter[5], counting[5];

namespace Helper {{
    template <class T, class V>
    T castType(V number) {{
        if (number < 0) {{
            number *= -1;
        }}
        return static_cast<T>(isnan(number) || isnanf(number) ? 0 : number);
    }}

    uint8_t generateId(uint8_t id) {{
        return id == 4 ? 4 : id - 1;
    }}
}};


template <class T = float>
struct ConnectorState {{

    T previousPower, currentPower, prevEnergy, currentEnergy, prevAmpe, currentAmpe;

    bool isActivated;
    shared_ptr<MicroOcpp::Transaction> txn;
    ConnectorState() : previousPower(0.f), currentPower(0.f),
                       prevEnergy(0.f), currentEnergy(0.f),
                       prevAmpe(0.f), currentAmpe(0.f),
                       isActivated(false) {{}};

    void setTransaction(std::shared_ptr<MicroOcpp::Transaction> tx) {{ this->txn = tx; }}

    void setPreviousPower(T previous) {{ this->previousPower = previous; }}

    void setCurrentPower(T current) {{ this->currentPower = current; }}

    void setPreviousEnergy(T previous) {{ this->prevEnergy = previous; }}

    void setCurrentEnergy(T current) {{ this->currentEnergy = current; }}

    void setPreviousAmpe(T previous) {{ this->prevAmpe = previous; }}

    void setCurrentAmpe(T current) {{ this->currentAmpe = current; }}

    void setActivated() {{ this->isActivated = true; }}

    void deActivated() {{ this->isActivated = false; }}

    bool getStatus() {{
        return this->isActivated;
    }}

    // PREVIUOS POWER
    T getPreviousPower() const {{ return Helper::castType<T>(previousPower); }}
    // CURRENT POWER
    T getCurrentPower() const {{ return Helper::castType<T>(currentPower); }}
    // PREVIOUS ENERGY
    T getPreviousEnergy() const {{ return Helper::castType<T>(prevEnergy); }}
    // CURRENT ENERGY
    T getCurrentEnergy() const {{ return Helper::castType<T>(currentEnergy); }}
    // PREVVIOUS AMPE
    T getPreviousAmpe() const {{ return Helper::castType<T>(prevAmpe); }}
    // GET CURRENT AMPE
    T getCurrentAmpe() const {{ return Helper::castType<T>(currentAmpe); }}
}};

namespace ItemPower {{
    struct Item {{
        uint8_t channel, messageId;
    }};

    {item_definition}

    Item getInfo(int id) {{
        return item[id];
    }}
}};
namespace PowerService {{
    struct PowerInfo {{
        uint16_t voltage;
        uint16_t current;
        uint16_t power;
        uint32_t energy;
    }};

    uint8_t channelControlState = 0x00;

    uint16_t calcrc(const uint8_t *ptr, size_t count) {{
        uint16_t crc = 0;
        while (count--) {{
            crc ^= (*ptr++ << 8);
            for (uint8_t i = 0; i < 8; i++) {{
                crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1;
            }}
        }}
        return crc;
    }}

    void decodeData(const uint8_t *buffer, PowerInfo &info) {{
        info.voltage = (buffer[6] << 8) | buffer[7];
        info.current = (buffer[8] << 8) | buffer[9];
        info.power = (buffer[10] << 8) | buffer[11];
        info.energy = (buffer[12] << 24) | (buffer[13] << 16) | (buffer[14] << 8) | buffer[15];
    }}

    void sendMessage(uint16_t messageID, uint16_t eventCode, const uint8_t *eventPayload, size_t payloadLength) {{
        uint8_t message[255];

        message[0] = START_OF_MESSAGE;
        message[1] = 2 + 2 + payloadLength; // Byte độ dài
        message[2] = highByte(messageID);
        message[3] = lowByte(messageID);
        message[4] = highByte(eventCode);
        message[5] = lowByte(eventCode);

        memcpy(&message[6], eventPayload, payloadLength);

        uint16_t checksum = calcrc(message, 6 + payloadLength);
        message[6 + payloadLength] = highByte(checksum);
        message[7 + payloadLength] = lowByte(checksum);

        Serial.write(message, 8 + payloadLength);
    }}

    PowerInfo sendAndReceiveDataToStm32(uint16_t messageID, uint16_t eventCode, const uint8_t *eventPayload, size_t payloadLength, PowerInfo &oldInfo) {{
        PowerInfo newInfo = {{0, 0, 0, 0}};
        uint8_t buffer[2048];
        size_t bytesRead = 0;
        size_t expectedLength = 0;

        for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++) {{
            sendMessage(messageID, eventCode, eventPayload, payloadLength);

            unsigned long startTime = millis();
            while (millis() - startTime < 500) {{ // Timeout 500ms
                if (Serial.available() > 0) {{
                    uint8_t byte = Serial.read();

                    if (bytesRead == 0 && byte == START_OF_MESSAGE) {{
                        buffer[bytesRead++] = byte;
                    }}
                    else if (bytesRead == 1) {{
                        buffer[bytesRead++] = byte;
                        expectedLength = byte + 4;
                    }}
                    else if (bytesRead > 1) {{
                        buffer[bytesRead++] = byte;

                        if (bytesRead == expectedLength) {{
                            uint16_t receivedChecksum = (buffer[expectedLength - 2] << 8) | buffer[expectedLength - 1];
                            uint16_t calculatedChecksum = calcrc(buffer, expectedLength - 2);

                            uint16_t receivedMessageID = (buffer[2] << 8) | buffer[3];
                            if (receivedChecksum == calculatedChecksum && receivedMessageID == messageID && buffer[5] == 0x03) {{
                                decodeData(buffer, newInfo);
                                return newInfo; // Trả về thông tin điện năng mới nếu phản hồi hợp lệ
                            }}

                            bytesRead = 0;
                            expectedLength = 0;
                        }}
                    }}
                }}
            }}
            delay(100);
        }}
        return oldInfo; // Trả về thông tin cũ nếu không nhận được phản hồi hợp lệ
    }}

    void setChannelControl(uint8_t bitPosition, bool value) {{
        channelControlState = value ? (channelControlState | (1 << bitPosition)) : (channelControlState & ~(1 << bitPosition));
        uint8_t payload[] = {{channelControlState}};
        sendMessage(0x0000, 0x0001, payload, sizeof(payload));
    }}

    PowerInfo getPowerInfo(uint8_t channel, uint16_t messageID, PowerInfo &oldInfo) {{
        uint8_t payload[] = {{channel}};
        return sendAndReceiveDataToStm32(messageID, 0x0003, payload, sizeof(payload), oldInfo);
    }}
}};

{channel_data}

class Connector;

vector<Connector *> connector;
vector<ConnectorState<> *> state(10);

class Connector {{
private:
    uint8_t itemId;

public:
    Connector() = default;

    Connector(uint8_t id) {{ this->itemId = id; }}

    uint8_t getItemId() {{
        return this->itemId;
    }}

    void runInstance() {{

        setEnergyMeterInput([&]() -> float {{
            auto tx = getTransaction(this->getItemId());
            bool isRunning = (tx->isActive() and !tx->isRunning()) or (tx->isActive() and tx->isRunning());
            bool isStop = !tx->isActive() and tx->isRunning();
            if (tx) {{
                if (isRunning) {{
                    if (!state[this->getItemId()]->getStatus()) {{
                        (state[this->getItemId()])->setActivated();
                        (state[this->getItemId()])->setTransaction(tx);
                    }}
                    PowerService::setChannelControl(Helper::generateId(this->getItemId()), 1);
                }}
                else if (isStop) {{
                    (state[this->getItemId()])->deActivated();
                    PowerService::setChannelControl(Helper::generateId(this->getItemId()), 0);
                }}
            }}
            delay(100);

            auto [channel, messageID] = ItemPower::getInfo(this->getItemId());
            PowerService::PowerInfo power = PowerService::getPowerInfo(channel, messageID,
                                                                       powerChannel[this->getItemId()]);
            return Helper::castType<float>(power.energy);
        }}, getItemId());

        setPowerMeterInput([&]() -> float {{
            auto [channel, messageID] = ItemPower::getInfo(this->getItemId());
            PowerService::PowerInfo power = PowerService::getPowerInfo(channel, messageID,
                                                                       powerChannel[this->getItemId()]);
            return Helper::castType<float>(power.power);
        }}, getItemId());

        setConnectorPluggedInput([]() -> boolean {{ return true; }}, getItemId());
    }}
}};

void initConnector() {{
    if (!connector.size()) {{
        for (uint8_t index = 1; index <= NUM_CONNECTOR; ++index) {{
            connector.emplace_back(new Connector(*&index));
            state[index] = new ConnectorState<>();
        }}
        for (uint8_t i = 0; i < NUM_CONNECTOR; ++i) {{
            PowerService::setChannelControl(*&i, 0);
        }}
    }}
    for (auto item : connector) {{
        item->runInstance();
    }}
    delay(1000);
}}
void runInstance() {{
    for (uint8_t i = 1; i <= NUM_CONNECTOR; ++i) {{
        if (!state[i]->isActivated) {{
            continue;
        }}
        else {{

            auto [channel, messageID] = ItemPower::getInfo(i);
            PowerService::PowerInfo info = {{0, 0, 0, 0}};
            PowerService::PowerInfo power = PowerService::getPowerInfo(channel, messageID,
                                                                       info);

            state[i]->setPreviousPower(state[i]->getCurrentPower());
            state[i]->setCurrentPower(power.power);

            state[i]->setPreviousEnergy(state[i]->getCurrentEnergy());
            state[i]->setCurrentEnergy(power.energy);

            state[i]->setPreviousAmpe(state[i]->getCurrentAmpe());
            state[i]->setCurrentAmpe(power.current);

            bool isAbleToStop = cnt[i] >= LOW_LIMIT_POWER_TIME_TRY or
                                counter[i] >= MAX_TIME_TRY or
                                state[i]->getCurrentAmpe() >= HIGH_LIMIT_AMPE or
                                state[i]->getPreviousAmpe() >= HIGH_LIMIT_AMPE;

            bool isLowPower = state[i]->getCurrentPower() > 0 and state[i]->getCurrentPower() <= LOW_LIMIT_POWER;
            bool isZeroPower = state[i]->getCurrentPower() == 0 and state[i]->getPreviousPower() == 0;

            cnt[i] += isLowPower;

            counter[i] += isZeroPower;

            if (isAbleToStop) {{
                counter[i] = 0;
                cnt[i] = 0;
                (state[i]->txn)->setInactive();
                (state[i]->txn)->setStopReason("Other");
                (state[i]->txn)->commit();
            }}
        }}
    }}
    delay(READ_DELAY);
}}

void setup() {{
    Serial.begin(115200);

    Serial.print(F("[main] Wait for WiFi: "));

#if defined(ESP8266)
    WiFiMulti.addAP(STASSID, STAPSK);
    while (WiFiMulti.run() != WL_CONNECTED) {{
        Serial.print('.');
        delay(1000);
    }}
#elif defined(ESP32)
    WiFi.begin(STASSID, STAPSK);
    while (!WiFi.isConnected()) {{
        Serial.print('.');
        delay(1000);
    }}
#else
#error only ESP32 or ESP8266 supported at the moment
#endif

    Serial.println(F(" connected!"));
    mocpp_initialize(OCPP_BACKEND_URL, OCPP_CHARGE_BOX_ID, "ZioT Test Station", "ZioT");
    initConnector();
}}

void loop() {{
    mocpp_loop();
    runInstance();
}}
"""

    # Mở file và ghi dữ liệu xuống
    with open(args.file, "w") as f:
        f.write(data)

    print(f"Thông tin đã được ghi xuống file {args.file}")


if __name__ == "__main__":
    main()

# python write.py -wifi=SOLARZ-PKT -password=88888888 -num_connector=4 -backend_url=ws://dev-ocpp.rabbitevc.vn/websocket/central-system -charge_box_id=1 -file=../src/main.cpp
