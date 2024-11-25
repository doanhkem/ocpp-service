#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,avx512f,avx512vl,fma")
#pragma GCC optimization("O2")
#pragma GCC optimization("O3")
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")

#include <PZEM004Tv30.h>
#include <library.h>

#include <algorithm>
#include <vector>

#include "helper.cpp"
#include "logger.cpp"

#ifdef ESP32
#define RELAY1 19
#define RELAY2 18
#define RELAY3 22
#define RELAY4 23

#ifndef PZEM_RX_PIN
#define PZEM_RX_PIN 16
#endif

#ifndef PZEM_TX_PIN
#define PZEM_TX_PIN 17
#endif

#ifndef PZEM_SERIAL
#define PZEM_SERIAL Serial2
#endif
PZEM004Tv30 pzem_1(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x01);
PZEM004Tv30 pzem_2(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x02);
PZEM004Tv30 pzem_3(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x03);
PZEM004Tv30 pzem_4(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x04);
#endif

#ifdef ESP8266
PZEM004Tv30 pzem1(1, 3, 0x01);
#endif

using std::vector, std::function;

vector<int> cnt(10, 0), counter(10, 0), counting(10, 0);
int nxt;

ConnectorState<float> *state[10];

class Charger;

vector<Charger *> connectors;
// CREATE 8 INSTANCE
// vector<Charger *> charger;

namespace PowerService {
float getPower(int id) {
    float power[] = {
        Helper::castType<float>(pzem_1.power()),
        Helper::castType<float>(pzem_2.power()),
    };
    return power[id - 1];
}

float getMeter(int id) {
    float meter[] = {
        Helper::castType<float>(pzem_1.energy()),
        Helper::castType<float>(pzem_2.energy()),
    };
    return meter[id - 1] * CONVERT_TO_KWH;
}

float getAmpe(int id) {
    float ampe[] = {
        Helper::castType<float>(pzem_1.current()),
        Helper::castType<float>(pzem_2.current()),
    };
    return ampe[id - 1];
}
};  // namespace PowerService

class Charger {
   private:
    int itemId;

   public:
    // THIS IS OPEN METER TO READ 1. OPEN RELAY 0. CLOSE RELAY
    function<auto(int)->void> setRelayStatus = [&](int val) -> void {
        int id = this->getById();
// MO_DBG_INFO("Item id: %d", id);
#ifdef ESP32
        switch (id) {
            case 1:
                // MO_DBG_INFO("Relay 1");
                pinMode(RELAY1, OUTPUT);
                digitalWrite(RELAY1, val);
                break;
            case 2:
                // MO_DBG_INFO("Relay 2");
                pinMode(RELAY2, OUTPUT);
                digitalWrite(RELAY2, val);
                break;
            case 3:
                // MO_DBG_INFO("Relay 3");
                pinMode(RELAY3, OUTPUT);
                digitalWrite(RELAY3, val);
                break;
            case 4:
                // MO_DBG_INFO("Relay 4");
                pinMode(RELAY4, OUTPUT);
                digitalWrite(RELAY4, val);
                break;
            default:
                break;
        }

        PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
// delay(1000);
#endif
    };

   public:
    Charger() = default;

    Charger(int id) { this->itemId = id; }

    int getById() { return this->itemId; }

    void executeInstance(void) {
        setEnergyMeterInput(
            [&]() -> float {
                // take the energy register of the main electricity meter and return the value in watt-hours
                // CHECK IF RUNNING => OPEN RELAY
                // CHECK DONE CHARGING => CLOSE RELAY
                auto tx = getTransaction(this->getById());
                if (tx) {
                    if ((tx->isActive() && !tx->isRunning()) || (tx->isActive() && !tx->isRunning()) || (tx->isActive() && tx->isRunning())) {
                        MO_DBG_INFO("=============START OPEN=============");
                        if (!state[this->getById()]->getActive()) {
                            (state[this->getById()])->setActivated();
                            (state[this->getById()])->setTransaction(tx);
                            (state[this->getById()])->setMeter = setRelayStatus;
                        }
                        setRelayStatus(1);
                    } else if (!tx->isActive() && tx->isRunning()) {
                        MO_DBG_INFO("=============CLOSE=============");
                        (state[this->getById()])->deActivated();
                        counter[this->getById()] = 0;
                        cnt[this->getById()] = 0;
                        setRelayStatus(0);
                    }
                }
                float result = PowerService::getMeter(this->getById());
                return result;
            },
            getById());

        setPowerMeterInput(
            [&]() -> float {
                auto tx = getTransaction(this->getById());
                float result = PowerService::getPower(this->getById());

                if (result > 1e4 || result <= 0) {
                    result = Helper::castType<float>(1);
                }
                // Logger::logItem({this->getById(), result, pzem_1.power(), pzem_2.power(), pzem_3.power(),
                // pzem_4.power()},
                //                "power");
                return result;
            },
            getById());

        setConnectorPluggedInput([]() -> bool { return true; }, getById());
    };
};

void runAllCharger() {
    if (!connectors.size()) {
        for (int index = 1; index <= TOTAL_CONNECTOR; ++index) {
            state[index] = new ConnectorState<float>;
            connectors.emplace_back(new Charger(index));
        }
    }
    for (auto item : connectors) {
        item->executeInstance();
    }
}

void configModeCallback(WiFiManager *myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setupWifi() {
#if defined(ESP8266)
    MO_DBG_INFO("CHECK HERE");
    Serial.begin(115200);
    if (!WiFi.isConnected()) {
        WiFiManager wifiManager;
        // Khai bao wifiManager
        // CLEAR CONFIG HERE
        // wifiManager.resetSettings();
        // ESP.eraseConfig();
        // Setup callback de khoi dong AP voi SSID "ESP+chipID"`
        delay(200);
        wifiManager.setAPCallback(configModeCallback);
        if (!wifiManager.autoConnect()) {
            Serial.println("failed to connect and hit timeout");
            // Neu ket noi that bai thi reset
            ESP.reset();
            delay(10 * 1000);
        }
        // Thanh cong thi bao ra man hinh
        Serial.println("connected...");
        delay(2 * 1000);
    } else {
        Serial.println("CONNECTED OK");
    }
#endif

#if defined(ESP32)
    WiFi.mode(WIFI_AP_STA);  // explicitly set mode, esp defaults to STA+AP
    Serial.begin(115200);

    WiFiManager wm;
    // wm.resetSettings();

    bool res;
    res = wm.autoConnect("ZIOT CHARGER", "rabbitevc");  // password protected ap

    if (!res) {
        Serial.println("Failed to connect");
        ESP.restart();
    } else {
        // if you get here you have connected to the WiFi
        Serial.println("Connected Wifi success !!!");
    }
#endif
}

void setup() {
    /*
     * Initialize Serial and WiFi
     */

    Serial.begin(115200);

    Serial.print(F("[main] Wait for WiFi: "));

    setupWifi();

    mocpp_initialize(OCPP_BACKEND_URL, OCPP_CHARGE_BOX_ID, CHARGE_STATION, COMPANY_NAME);
    if (WiFi.isConnected()) {
        runAllCharger();
    }
}

void executeTracker() {
    for (int i = 1; i <= TOTAL_CONNECTOR; ++i) {
        if (!state[i]->getActive()) {
            continue;
        } else {
            state[i]->setPreviousPower(state[i]->getCurrentPower());
            state[i]->setCurrentPower(PowerService::getPower(i));
            state[i]->setPreviousEnergy(state[i]->getCurrentEnergy());
            state[i]->setCurrentEnergy(PowerService::getMeter(i));
            // LOGIC KHI DÒNG DƯỚI 20W
            // TĂNG TẦN SUẤT
            if (state[i]->getCurrentPower() > 0 && state[i]->getCurrentPower() <= LOW_LIMIT_POWER) {
                ++cnt[i];
            }
            if (state[i]->getCurrentPower() == 0 && state[i]->getPreviousPower() == 0) {
                ++counter[i];
            }
            // HANDLE TOÀN BỘ LOGIC TRONG HỆ XỬ LÝ
            if (cnt[i] >= LOW_LIMIT_POWER_TIME_TRY || counter[i] >= MAX_TIME || state[i]->getCurrentAmpe() > HIGH_LIMIT_AMPE) {
                counter[i] = 0;
                cnt[i] = 0;
                (state[i]->txn)->setInactive();
                (state[i]->txn)->setStopReason("Other");
                (state[i]->txn)->commit();
                state[i]->setMeter(0);
            }
        }
    }
    delay(200);
}

void loop() {
    mocpp_loop();
    for (int i = 1; i <= TOTAL_CONNECTOR; ++i) {
        if (state[i]->getActive()) {
            executeTracker();
        }
    }
}