#include <IPv6Address.h>
#include <WiFi.h>
// #include <stm32_def.h>
const char *ssid = "SLZ-PKT";
const char *password = "88888888";

#define RELAY1 19

#include <PZEM004Tv30.h>
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#define PZEM_SERIAL Serial2
PZEM004Tv30 pzem_1(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x01);

class Meter {
   public:
    void setup() {
        Serial.begin(115200);
        delay(200);
        Serial.println();
        Serial.print("Connecting to wifi: ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.println("WiFi connected with IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Subnet Mask : ");
        Serial.println(WiFi.subnetMask());
        Serial.print("Gateway IP : ");
        Serial.println(WiFi.gatewayIP());

        pinMode(RELAY1, OUTPUT);
        digitalWrite(RELAY1, 1);

        PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
        delay(1000);
    }

    void loop() {
        if (Serial.available() > 0) {
            char c = Serial.read();
            if (c == '1') {
                digitalWrite(RELAY1, !digitalRead(RELAY1));
            }
        }

        static uint32_t TIME_RUN_GET_DATA = millis();
        if (millis() - TIME_RUN_GET_DATA > 100) {
            TIME_RUN_GET_DATA = millis();
            static int ChonCbDoc = 1;
            ChonCbDoc++;
            if (ChonCbDoc == 1) {
                float voltage = pzem_1.voltage();
                float current = pzem_1.current();
                float power = pzem_1.power();
                float energy = pzem_1.energy();
                float frequency = pzem_1.frequency();
                float pf = pzem_1.pf();
                if (isnan(voltage)) {
                    Serial.println("Error reading voltage pzem_1");
                } else if (isnan(current)) {
                    Serial.println("Error reading current pzem_1");
                } else if (isnan(power)) {
                    Serial.println("Error reading power pzem_1");
                } else if (isnan(energy)) {
                    Serial.println("Error reading energy pzem_1");
                } else if (isnan(frequency)) {
                    Serial.println("Error reading frequency pzem_1");
                } else if (isnan(pf)) {
                    Serial.println("Error reading power factor pzem_1");
                } else {
                    Serial.print("pzem_1 - Voltage: ");
                    Serial.print(voltage);
                    Serial.print("V");
                    Serial.print("  Current: ");
                    Serial.print(current);
                    Serial.print("A");
                    Serial.print("  Power: ");
                    Serial.print(power);
                    Serial.print("W");
                    Serial.print("  Energy: ");
                    Serial.print(energy, 3);
                    Serial.print("kWh");
                    Serial.print("  Frequency: ");
                    Serial.print(frequency, 1);
                    Serial.print("Hz");
                    Serial.print("  PF: ");
                    Serial.println(pf);
                }
            }
        }
    }
};
