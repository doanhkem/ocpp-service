
#include <MicroOcpp.h>
#include <MicroOcpp/Debug.h>
#include <SPIFFS.h>
#include <vector>
#include <ModbusIP_ESP8266.h>
#include "WiFiConfig.h"
#include <IPAddress.h>
#include <PZEM004Tv30.h>

#define NUM_CONNECTOR 1
#define MAX_TIME_TRY 50   // 50
#define HIGH_LIMIT_POWER 8000
#define HIGH_LIMIT_AMPE 33000 // 9000
#define LOW_LIMIT_POWER 0   // 10
#define LOW_LIMIT_POWER_TIME_TRY 50 //50
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

#define STASSID "SOLARZ-PKT"
#define STAPSK "88888888"

// #define OCPP_BACKEND_URL "ws://dev-ocpp.rabbitevc.vn/websocket/central-system"
// #define OCPP_CHARGE_BOX_ID "1"

#define RELAY1 1 

#ifndef PZEM_RX_PIN
#define PZEM_RX_PIN 17 // Chọn chân GPIO UART hợp lệ
#endif

#ifndef PZEM_TX_PIN
#define PZEM_TX_PIN 18 // Chọn chân GPIO UART hợp lệ
#endif

#ifndef PZEM_SERIAL
#define PZEM_SERIAL Serial1 // Sử dụng Serial1 thay vì Serial2
#endif
PZEM004Tv30 pzem_1(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x01);


using namespace std;


const int REGISTERS[] = {5015, 5018, 5011, 5023, 5024}; 
IPAddress remote; 
int part1, part2, part3, part4;      
ModbusIP mb; 
uint16_t old_state = 0;
uint8_t cnt[5], counter[5], counting[5];

namespace Helper {
    template <class T, class V>
    T castType(V number) {
        if (number < 0) {
            number *= -1;
        }
        return static_cast<T>(isnan(number) || isnanf(number) ? 0 : number);
    }

    uint8_t generateId(uint8_t id) {
        return id == 4 ? 4 : id - 1;
    }
};


template <class T = float>
struct ConnectorState {

    T previousPower, currentPower, prevEnergy, currentEnergy, prevAmpe, currentAmpe;

    bool isActivated;
    shared_ptr<MicroOcpp::Transaction> txn;
    ConnectorState() : previousPower(0.f), currentPower(0.f),
                       prevEnergy(0.f), currentEnergy(0.f),
                       prevAmpe(0.f), currentAmpe(0.f),
                       isActivated(false) {};

    void setTransaction(std::shared_ptr<MicroOcpp::Transaction> tx) { this->txn = tx; }

    void setPreviousPower(T previous) { this->previousPower = previous; }

    void setCurrentPower(T current) { this->currentPower = current; }

    void setPreviousEnergy(T previous) { this->prevEnergy = previous; }

    void setCurrentEnergy(T current) { this->currentEnergy = current; }

    void setPreviousAmpe(T previous) { this->prevAmpe = previous; }

    void setCurrentAmpe(T current) { this->currentAmpe = current; }

    void setActivated() { this->isActivated = true; }

    void deActivated() { this->isActivated = false; }

    bool getStatus() {
        return this->isActivated;
    }

    // PREVIUOS POWER
    T getPreviousPower() const { return Helper::castType<T>(previousPower); }
    // CURRENT POWER
    T getCurrentPower() const { return Helper::castType<T>(currentPower); }
    // PREVIOUS ENERGY
    T getPreviousEnergy() const { return Helper::castType<T>(prevEnergy); }
    // CURRENT ENERGY
    T getCurrentEnergy() const { return Helper::castType<T>(currentEnergy); }
    // PREVVIOUS AMPE
    T getPreviousAmpe() const { return Helper::castType<T>(prevAmpe); }
    // GET CURRENT AMPE
    T getCurrentAmpe() const { return Helper::castType<T>(currentAmpe); }
};

namespace ItemPower {
    struct Item {
        uint8_t channel, messageId;
    };

    
    Item item[] = {{0x00, 0x0000}, {0x01, 0x0001}, {0x02, 0x0002}, {0x03, 0x0003}, {0x05, 0x0005}};


    Item getInfo(int id) {
        return item[id];
    }
};namespace PowerService {
    float values[5];
    uint16_t Charger_status;
    struct PowerInfo {
        float voltage;
        float current;
        float power;
        float energy;
    };
    bool isEqual(const PowerInfo &info1, const PowerInfo &info2) {
        return (info1.voltage == info2.voltage &&
                info1.current == info2.current &&
                info1.power == info2.power &&
                info1.energy == info2.energy);
    }
    uint8_t channelControlState = 0x00;

void decodeData(const float *registers, PowerInfo &info) {
    MO_DBG_INFO("Volt = %.2f V", registers[0]);
    info.voltage = registers[0];
    info.current = registers[1] * 1000;
    MO_DBG_INFO("Ampe = %.2f mA", registers[1] * 1000);
    info.power = registers[2];
    MO_DBG_INFO("Power = %.2f W", registers[2]);
    info.energy = registers[3] * 1000;
    MO_DBG_INFO("Ene = %.2f Wh", registers[3] * 1000);
}


    PowerInfo sendAndReceiveDataToStm32(uint16_t messageID, uint16_t eventCode, const uint8_t *eventPayload, size_t payloadLength, PowerInfo &oldInfo) {

        PowerInfo newInfo = {0, 0, 0, 0};
        values[0] = pzem_1.voltage();
        values[1] = pzem_1.current();
        values[2] = pzem_1.power();
        values[3] = pzem_1.energy();  
        

        decodeData(values, newInfo);
        MO_DBG_INFO("newwinfo", newInfo);

        PowerInfo defaultInfo = {0, 0, 0, 0};

        if (!isEqual(newInfo, defaultInfo)) {
            return newInfo;
            MO_DBG_INFO("return oldinfo %d", newInfo);
        } else {
            return oldInfo;
            MO_DBG_INFO("return oldinfo %d", oldInfo);
        }
    }

    void setChannelControl(uint8_t bitPosition, uint16_t value) {
        if (value == 0 || value == 1) {
            digitalWrite(RELAY1, value);
            MO_DBG_INFO("set chargring: %d", value);
            delay(100);
        } else {
            Serial.println("Invalid state. Please use 0 or 1.");
        }
    }

    PowerInfo getPowerInfo(uint8_t channel, uint16_t messageID, PowerInfo &oldInfo) {
        uint8_t payload[] = {channel};
        return sendAndReceiveDataToStm32(messageID, 0x0003, payload, sizeof(payload), oldInfo);
    }

    void checkState(uint16_t _state, uint16_t *state_) {
        IPAddress remote(192, 168, 2, 253);
        ModbusIP mb;
        mb.client();
        mb.connect(remote, 502);
        if (mb.isConnected(remote)) {
            mb.readHreg(remote, REGISTERS[0], &Charger_status);
            delay(100);
            MO_DBG_INFO("done %d", Charger_status);
        } else {
            mb.connect(remote, 502);
        }
        mb.task();
        delay(100);
        if (Charger_status == 6 && _state != Charger_status) {
            setConnectorPluggedInput([]() -> bool { return true; }, 1);
            MO_DBG_INFO("set preparing");
        } else if (Charger_status == 0 && _state != Charger_status) {
            setConnectorPluggedInput([]() -> bool { return true; }, 1);
            MO_DBG_INFO("set avaible");
        }
        *state_ = Charger_status;
    }
};


PowerService::PowerInfo powerChannel[10] = {{0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 0, 0}};


class Connector;

vector<Connector *> connector;
vector<ConnectorState<> *> state(10);

class Connector {
private:
    uint8_t itemId;

public:
    Connector() = default;

    Connector(uint8_t id) { this->itemId = id; }

    uint8_t getItemId() {
        return this->itemId;
    }

    void runInstance() {

        setEnergyMeterInput([&]() -> float {
            auto tx = getTransaction(this->getItemId());
            bool isRunning = (tx->isActive() and !tx->isRunning()) or (tx->isActive() and tx->isRunning());
            bool isStop = !tx->isActive() and tx->isRunning();
            if (tx) {
                if (isRunning) {
                    if (!state[this->getItemId()]->getStatus()) {
                        (state[this->getItemId()])->setActivated();
                        (state[this->getItemId()])->setTransaction(tx);
                    }
                    PowerService::setChannelControl(Helper::generateId(this->getItemId()), 1);
                }
                else if (isStop) {
                    (state[this->getItemId()])->deActivated();
                    PowerService::setChannelControl(Helper::generateId(this->getItemId()), 0);
                }
            }
            delay(200);

            auto [channel, messageID] = ItemPower::getInfo(this->getItemId());
            PowerService::PowerInfo power = PowerService::getPowerInfo(channel, messageID,
                                                                       powerChannel[this->getItemId()]);
            return Helper::castType<float>(power.energy);
        }, getItemId());

        setPowerMeterInput([&]() -> float {
            auto [channel, messageID] = ItemPower::getInfo(this->getItemId());
            PowerService::PowerInfo power = PowerService::getPowerInfo(channel, messageID,
                                                                       powerChannel[this->getItemId()]);
            return Helper::castType<float>(power.power);
        }, getItemId());

        setConnectorPluggedInput([]() -> boolean { return true; }, getItemId());
    }
};

void initConnector() {
    if (!connector.size()) {
        for (uint8_t index = 1; index <= NUM_CONNECTOR; ++index) {
            connector.emplace_back(new Connector(*&index));
            state[index] = new ConnectorState<>();
        }
        for (uint8_t i = 0; i < NUM_CONNECTOR; ++i) {
            PowerService::setChannelControl(*&i, 0);
        }
    }
    for (auto item : connector) {
        item->runInstance();
    }
    delay(1000);
}
void runInstance() {
    for (uint8_t i = 1; i <= NUM_CONNECTOR; ++i) {
        if (!state[i]->isActivated) {
            continue;
        }
        else {

            auto [channel, messageID] = ItemPower::getInfo(i);
            PowerService::PowerInfo info = {0, 0, 0, 0};
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

            if (isAbleToStop) {
                counter[i] = 0;
                cnt[i] = 0;
                (state[i]->txn)->setInactive();
                (state[i]->txn)->setStopReason("Other");
                (state[i]->txn)->commit();
            }
        }
    }
    delay(READ_DELAY);
}




void setup() {
    Serial.begin(115200);

    Serial.print(F("[main] Wait for WiFi: "));

#if defined(ESP8266)
    WiFiMulti.addAP(STASSID, STAPSK);
    while (WiFiMulti.run() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
#elif defined(ESP32)

    pinMode(RELAY1, OUTPUT);
    digitalWrite(RELAY1, LOW);
    PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);


    setupWiFi();
    Serial.println("Setup complete!");
    // Serial.println(evseIpAddress);
    Serial.println(WiFi.localIP());
    Serial.print("CharboxID: ");
    Serial.println(charboxID);
    Serial.print("OCPP Server: ");
    Serial.println(ocppServer);
    
    String URL;

    if (ocppServer.equals("dev")) {
        URL = "wss://dev-ocpp.rabbitevc.vn/websocket/central-system";
    } else if (ocppServer.equals("1")) {
        URL =  "wss://ocpp-1.rabbitevc.vn/steve/websocket/CentralSystemService";
    } else if (ocppServer.equals("2")) {
        URL = "wss://ocpp-2.rabbitevc.vn/steve/websocket/CentralSystemService";
    } else {
        URL = "wss://";
    }
    #define OCPP_BACKEND_URL URL.c_str()
    #define OCPP_CHARGE_BOX_ID charboxID.c_str()

#else
#error only ESP32 or ESP8266 supported at the moment
#endif

    Serial.println(F(" connected!"));
    mocpp_initialize(OCPP_BACKEND_URL, OCPP_CHARGE_BOX_ID, "ZioT Test Station", "ZioT");
    initConnector();
}

void loop() {
    mocpp_loop();
    runInstance();
}
