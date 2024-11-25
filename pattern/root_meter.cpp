#include "WiFi.h"

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

class Meter {
    void setup() {
        pinMode(RELAY1, OUTPUT);
        pinMode(RELAY2, OUTPUT);
        pinMode(RELAY3, OUTPUT);
        pinMode(RELAY4, OUTPUT);
        digitalWrite(RELAY1, 1);
        digitalWrite(RELAY2, 0);
        digitalWrite(RELAY3, 0);
        digitalWrite(RELAY4, 0);

        PZEM_SERIAL.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);
        delay(1000);
    }
    void loop() {
        if (Serial.available() > 0) {
            char c = Serial.read();
            if (c == '1') {
                digitalWrite(RELAY1, !digitalRead(RELAY1));
            }
            else if (c == '2') {
                digitalWrite(RELAY2, !digitalRead(RELAY2));
            }
            else if (c == '3') {
                digitalWrite(RELAY3, !digitalRead(RELAY3));
            }
            else if (c == '4') {
                digitalWrite(RELAY4, !digitalRead(RELAY4));
            }
        }

        static uint32_t TIME_RUN_GET_DATA = millis();
        if (millis() - TIME_RUN_GET_DATA > 100) {
            TIME_RUN_GET_DATA = millis();
            static int ChonCbDoc = 0;
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
                }
                else if (isnan(current)) {
                    Serial.println("Error reading current pzem_1");
                }
                else if (isnan(power)) {
                    Serial.println("Error reading power pzem_1");
                }
                else if (isnan(energy)) {
                    Serial.println("Error reading energy pzem_1");
                }
                else if (isnan(frequency)) {
                    Serial.println("Error reading frequency pzem_1");
                }
                else if (isnan(pf)) {
                    Serial.println("Error reading power factor pzem_1");
                }
                else {
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
            else if (ChonCbDoc == 2) {
                float voltage = pzem_2.voltage();
                float current = pzem_2.current();
                float power = pzem_2.power();
                float energy = pzem_2.energy();
                float frequency = pzem_2.frequency();
                float pf = pzem_2.pf();
                if (isnan(voltage)) {
                    Serial.println("Error reading voltage pzem_2");
                }
                else if (isnan(current)) {
                    Serial.println("Error reading current pzem_2");
                }
                else if (isnan(power)) {
                    Serial.println("Error reading power pzem_2");
                }
                else if (isnan(energy)) {
                    Serial.println("Error reading energy pzem_2");
                }
                else if (isnan(frequency)) {
                    Serial.println("Error reading frequency pzem_2");
                }
                else if (isnan(pf)) {
                    Serial.println("Error reading power factor pzem_2");
                }
                else {
                    Serial.print("pzem_2 - Voltage: ");
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
            else if (ChonCbDoc == 3) {
                float voltage = pzem_3.voltage();
                float current = pzem_3.current();
                float power = pzem_3.power();
                float energy = pzem_3.energy();
                float frequency = pzem_3.frequency();
                float pf = pzem_3.pf();
                if (isnan(voltage)) {
                    Serial.println("Error reading voltage pzem_3");
                }
                else if (isnan(current)) {
                    Serial.println("Error reading current pzem_3");
                }
                else if (isnan(power)) {
                    Serial.println("Error reading power pzem_3");
                }
                else if (isnan(energy)) {
                    Serial.println("Error reading energy pzem_3");
                }
                else if (isnan(frequency)) {
                    Serial.println("Error reading frequency pzem_3");
                }
                else if (isnan(pf)) {
                    Serial.println("Error reading power factor pzem_3");
                }
                else {
                    Serial.print("pzem_3 - Voltage: ");
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
            else if (ChonCbDoc == 4) {
                ChonCbDoc = 0;
                float voltage = pzem_4.voltage();
                float current = pzem_4.current();
                float power = pzem_4.power();
                float energy = pzem_4.energy();
                float frequency = pzem_4.frequency();
                float pf = pzem_4.pf();
                if (isnan(voltage)) {
                    Serial.println("Error reading voltage pzem_4");
                }
                else if (isnan(current)) {
                    Serial.println("Error reading current pzem_4");
                }
                else if (isnan(power)) {
                    Serial.println("Error reading power pzem_4");
                }
                else if (isnan(energy)) {
                    Serial.println("Error reading energy pzem_4");
                }
                else if (isnan(frequency)) {
                    Serial.println("Error reading frequency pzem_4");
                }
                else if (isnan(pf)) {
                    Serial.println("Error reading power factor pzem_4");
                }
                else {
                    Serial.print("pzem_4 - Voltage: ");
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