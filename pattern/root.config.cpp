#include <library.h>

#define RELAY1 19
#define RELAY2 18
#define RELAY3 22
#define RELAY4 23

#include <PZEM004Tv30.h>
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#define PZEM_SERIAL Serial2
PZEM004Tv30 pzem_1(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x01);
PZEM004Tv30 pzem_2(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x02);
PZEM004Tv30 pzem_3(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x03);
PZEM004Tv30 pzem_4(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x04);

unsigned int connectorId = 1;

void setup() {

    /*
     * Initialize Serial and WiFi
     */

    Serial.begin(115200);

    Serial.print(F("[main] Wait for WiFi: "));

#if defined(ESP8266)
    WiFiMulti.addAP(STASSID, STAPSK);
    while (WiFiMulti.run() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
#elif defined(ESP32)
    WiFi.begin(STASSID, STAPSK);
    while (!WiFi.isConnected()) {
        Serial.print('.');
        delay(1000);
    }
#else
#error only ESP32 or ESP8266 supported at the moment
#endif

    Serial.println(F(" connected!"));

    /*
     * Initialize the OCPP library
     */
    mocpp_initialize(OCPP_BACKEND_URL, OCPP_CHARGE_BOX_ID, CHARGE_STATION, COMPANY_NAME);

    // THIS IS OPEN METER TO READ
    // 1. OPEN RELAY
    // 2. CLOSE RELAY
    auto setupMeter = [&](int val) -> void {
        pinMode(RELAY1, OUTPUT);
        pinMode(RELAY2, OUTPUT);
        pinMode(RELAY3, OUTPUT);
        pinMode(RELAY4, OUTPUT);
        digitalWrite(RELAY1, val);
        digitalWrite(RELAY2, val);
        digitalWrite(RELAY3, val);
        digitalWrite(RELAY4, val);

        PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
        delay(1000);
    };

    /*
     * Integrate OCPP functionality. You can leave out the following part if your EVSE doesn't need it.
     */

    setEnergyMeterInput(
        [&]() -> float {
            // take the energy register of the main electricity meter and return the value in watt-hours
            if (isTransactionActive() && !isTransactionRunning()) {
                MO_DBG_INFO("START OPEN");
                setupMeter(1);
            }
            else if (!isTransactionActive() && isTransactionRunning()) {
                MO_DBG_INFO("CLOSE");
                setupMeter(0);
            }
            MO_DBG_INFO("Active: %d, Running: %d", isTransactionActive(), isTransactionRunning());
            MO_DBG_INFO("Re set energy");
            float result = isnan(pzem_1.energy()) ? 0 : pzem_1.energy();
            MO_DBG_INFO("Ener: %f", result);
            return result;
        },
        connectorId);

    setPowerMeterInput(
        [&]() -> float {
            float result = isnan(pzem_1.power()) ? static_cast<float>(0) : pzem_1.power();
            MO_DBG_INFO("Power: %f", result);
            return result;
        },
        connectorId);

    setSmartChargingCurrentOutput(
        [](float limit) -> void {
            // set the SAE J1772 Control Pilot value here
            Serial.printf("[main] Smart Charging allows maximum charge rate: %.0f\n", limit);
        },
        connectorId);

    setConnectorPluggedInput(
        []() -> bool {
            return false;
        },
        connectorId);

    //... seez MicroOcpp.h for more settings
}

void loop() {
    /*
     * Do all OCPP stuff (process WebSocket input, send recorded meter values to Central System, etc.)
     */
    // meterLoop();
    mocpp_loop();

    /*
     * Energize EV plug if OCPP transaction is up and running
     */
    // /*
    //  * Use NFC reader to start and stop transactions
    //  */
    //... see MicroOcpp.h for more possibilities
}