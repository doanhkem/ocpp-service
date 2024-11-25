#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences preferences;
WebServer server(80);

// Khai báo các biến toàn cục
String ssid, password, ip, gateway, evseIpAddress;
bool configSaved = false;

void saveWiFiConfig(const String& ssid, const String& password, const String& ip, const String& gateway, const String& evseIpAddress) {
  preferences.begin("wifi-config", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.putString("ip", ip);
  preferences.putString("gateway", gateway);
  preferences.putString("evseIp", evseIpAddress);  // Lưu EVSE address IP
  preferences.end();
}

bool loadWiFiConfig(String& ssid, String& password, String& ip, String& gateway, String& evseIpAddress) {
  preferences.begin("wifi-config", true);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  ip = preferences.getString("ip", "");
  gateway = preferences.getString("gateway", "");
  evseIpAddress = preferences.getString("evseIp", "");  // Đọc EVSE address IP
  preferences.end();

  return ssid.length() > 0 && password.length() > 0 && ip.length() > 0 && gateway.length() > 0 && evseIpAddress.length() > 0;
}

void handleRoot() {
  String html = "<html><body><h2>Wi-Fi Configuration</h2>";
  html += "<form action=\"/save\" method=\"POST\">";
  html += "SSID: <input type=\"text\" name=\"ssid\"><br>";
  html += "Password: <input type=\"password\" name=\"password\"><br>";
  html += "Static IP: <input type=\"text\" name=\"ip\"><br>";
  html += "Gateway: <input type=\"text\" name=\"gateway\"><br>";
  html += "EVSE Address IP: <input type=\"text\" name=\"evseIpAddress\"><br>";  // Trường nhập EVSE address IP
  html += "<input type=\"submit\" value=\"Save\">";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("ip") && server.hasArg("gateway") && server.hasArg("evseIpAddress")) {
    ssid = server.arg("ssid");
    password = server.arg("password");
    ip = server.arg("ip");
    gateway = server.arg("gateway");
    evseIpAddress = server.arg("evseIpAddress");  // Lấy EVSE address IP từ form

    saveWiFiConfig(ssid, password, ip, gateway, evseIpAddress);
    configSaved = true;

    server.send(200, "text/html", "<html><body><h2>Configuration Saved!</h2></body></html>");
    delay(1000);
    server.stop();
  } else {
    server.send(200, "text/html", "<html><body><h2>Invalid Input!</h2></body></html>");
  }
}

void startWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
}

void connectWiFi(const String& ssid, const String& password, const String& ip, const String& gateway) {
  IPAddress local_ip, gateway_ip, subnet(255, 255, 255, 0);
  IPAddress dns(8, 8, 8, 8); // DNS Google

  if (local_ip.fromString(ip) && gateway_ip.fromString(gateway)) {
    WiFi.config(local_ip, gateway_ip, subnet, dns);
  }

  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 30000) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi.");
  }
}

void setupWiFi() {
  if (loadWiFiConfig(ssid, password, ip, gateway, evseIpAddress)) {
    Serial.println("Connecting with saved WiFi config");
    connectWiFi(ssid, password, ip, gateway);

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Entering Config Mode");
      WiFi.softAP("ESP32_Config", "12345678");
      startWebServer();
      while (!configSaved) {
        server.handleClient();
        delay(10);
      }
      connectWiFi(ssid, password, ip, gateway);
    }
  } else {
    Serial.println("Entering Config Mode");
    WiFi.softAP("ESP32_Config", "12345678");
    startWebServer();
    while (!configSaved) {
      server.handleClient();
      delay(10);
    }
    connectWiFi(ssid, password, ip, gateway);
  }
}

#endif // WIFI_CONFIG_H
