

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences preferences;
WebServer server(80);

// Khai báo các biến toàn cục
String ssid, password, charboxID, ocppServer;
bool configSaved = false;

// Lưu cấu hình Wi-Fi và thông tin khác
void saveWiFiConfig(const String &ssid, const String &password, const String &charboxID, const String &ocppServer) {
  preferences.begin("wifi-config", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.putString("charboxID", charboxID);
  preferences.putString("ocppServer", ocppServer);
  preferences.end();
}

// Tải cấu hình Wi-Fi
bool loadWiFiConfig(String &ssid, String &password, String &charboxID, String &ocppServer) {
  preferences.begin("wifi-config", true);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  charboxID = preferences.getString("charboxID", "");
  ocppServer = preferences.getString("ocppServer", "");
  preferences.end();

  return !ssid.isEmpty() && !password.isEmpty() && !charboxID.isEmpty() && !ocppServer.isEmpty();
}

// Xử lý trang chính
void handleRoot() {
  int n = WiFi.scanComplete();
  String wifiOptions = "";

  if (n == WIFI_SCAN_RUNNING) {
    wifiOptions = "<option value=\"\">Scanning...</option>";
  } else if (n <= 0) {
    wifiOptions = "<option value=\"\">No networks found</option>";
  } else {
    for (int i = 0; i < n; ++i) {
      wifiOptions += "<option value=\"" + WiFi.SSID(i) + "\">" + WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + " dBm)</option>";
    }
    WiFi.scanDelete(); // Xóa danh sách sau khi quét xong
  }

  String html = "<!DOCTYPE html><html><head><title>Wi-Fi Config</title>";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; height: 100vh; background-color: #e9ecef; }";
  html += ".container { max-width: 400px; width: 100%; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); }";
  html += "h2 { text-align: center; color: #333; margin-bottom: 20px; }";
  html += "form { margin-bottom: 15px; }";
  html += "input, select, button { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; font-size: 16px; box-sizing: border-box; }";
  html += "button { background-color: #007BFF; color: white; font-size: 16px; cursor: pointer; border: none; }";
  html += "button:hover { background-color: #0056b3; }";
  html += "p { text-align: center; font-size: 14px; color: #555; margin-top: 10px; }";
  html += "</style>";
  html += "</head><body>";

  html += "<div class='container'>";
  html += "<h2>Wi-Fi Configuration</h2>";

  html += "<form action=\"/scan\" method=\"POST\">";
  html += "<button type=\"submit\">Scan Wi-Fi</button>";
  html += "</form>";

  html += "<form action=\"/save\" method=\"POST\">";
  html += "<label for=\"ssid\">SSID:</label>";
  html += "<select id=\"ssid\" name=\"ssid\">" + wifiOptions + "</select>";

  html += "<label for=\"password\">Password:</label>";
  html += "<input type=\"password\" id=\"password\" name=\"password\" placeholder=\"Enter Wi-Fi password\">";

  html += "<label for=\"charboxID\">CharboxID:</label>";
  html += "<input type=\"text\" id=\"charboxID\" name=\"charboxID\" placeholder=\"Enter Charbox ID\">";

  html += "<label for=\"ocppServer\">OCPP Server:</label>";
  html += "<input type=\"text\" id=\"ocppServer\" name=\"ocppServer\" placeholder=\"Enter OCPP server URL\">";

  html += "<button type=\"submit\">Save</button>";
  html += "</form>";

  if (wifiOptions == "<option value=\"\">No networks found</option>") {
    html += "<p>No Wi-Fi networks found. Please try again.</p>";
  }

  html += "</div>";
  html += "</body></html>";


// String html = "<!DOCTYPE html><html><head><title>Wi-Fi Config</title>";
// html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
// html += "<style>";
// html += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; min-height: 100vh; background-color: #e9ecef; }";
// html += ".container { width: 90%; max-width: 400px; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); }";
// html += "h2 { text-align: center; color: #333; margin-bottom: 20px; font-size: 1.5em; }";
// html += "form { margin-bottom: 15px; }";
// html += "input, select, button { width: 100%; padding: 12px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; font-size: 16px; box-sizing: border-box; }";
// html += "button { background-color: #007BFF; color: white; font-size: 16px; cursor: pointer; border: none; transition: background-color 0.3s ease; }";
// html += "button:hover { background-color: #0056b3; }";
// html += "label { display: block; margin-bottom: 5px; font-weight: bold; font-size: 14px; color: #333; }";
// html += "p { text-align: center; font-size: 14px; color: #555; margin-top: 10px; }";
// html += "@media (max-width: 600px) {";
// html += "  h2 { font-size: 1.2em; }";
// html += "  button, input, select { font-size: 14px; }";
// html += "  .container { padding: 15px; }";
// html += "}";
// html += "</style>";
// html += "</head><body>";

// html += "<div class='container'>";
// html += "<h2>Wi-Fi Configuration</h2>";

// html += "<form action=\"/scan\" method=\"POST\">";
// html += "<button type=\"submit\">Scan Wi-Fi</button>";
// html += "</form>";

// html += "<form action=\"/save\" method=\"POST\">";
// html += "<label for=\"ssid\">SSID:</label>";
// html += "<select id=\"ssid\" name=\"ssid\">" + wifiOptions + "</select>";

// html += "<label for=\"password\">Password:</label>";
// html += "<input type=\"password\" id=\"password\" name=\"password\" placeholder=\"Enter Wi-Fi password\">";

// html += "<label for=\"charboxID\">CharboxID:</label>";
// html += "<input type=\"text\" id=\"charboxID\" name=\"charboxID\" placeholder=\"Enter Charbox ID\">";

// html += "<label for=\"ocppServer\">OCPP Server:</label>";
// html += "<input type=\"text\" id=\"ocppServer\" name=\"ocppServer\" placeholder=\"Enter OCPP server URL\">";

// html += "<button type=\"submit\">Save</button>";
// html += "</form>";

// if (wifiOptions == "<option value=\"\">No networks found</option>") {
//   html += "<p>No Wi-Fi networks found. Please try again.</p>";
// }

// html += "</div>";
// html += "</body></html>";

  server.send(200, "text/html", html);
}

// Xử lý quét Wi-Fi
void handleScan() {
  WiFi.scanDelete();           // Xóa danh sách mạng cũ
  WiFi.disconnect();
  int n = WiFi.scanNetworks(); // Bắt đầu quét Wi-Fi đồng bộ

  if (n > 0) {
    Serial.println("Wi-Fi networks found:");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s (%d dBm)\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
    }
  }

  handleRoot(); // Quay lại trang root với danh sách mạng Wi-Fi mới
}

// Xử lý lưu cấu hình
void handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("charboxID") && server.hasArg("ocppServer")) {
    ssid = server.arg("ssid");
    password = server.arg("password");
    charboxID = server.arg("charboxID");
    ocppServer = server.arg("ocppServer");

    saveWiFiConfig(ssid, password, charboxID, ocppServer);
    configSaved = true;

    server.send(200, "text/html", "<html><body><h2>Configuration Saved!</h2></body></html>");
    delay(1000);
    server.stop();
  } else {
    server.send(200, "text/html", "<html><body><h2>Invalid Input!</h2></body></html>");
  }
}

// Bắt đầu server
void startWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/scan", HTTP_POST, handleScan);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
}

// Kết nối Wi-Fi với cấu hình đã lưu
void connectWiFi() {
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

// Cấu hình Wi-Fi
void setupWiFi() {
  if (loadWiFiConfig(ssid, password, charboxID, ocppServer)) {
    Serial.println("Connecting with saved WiFi config");
    connectWiFi();

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Entering Config Mode");
      WiFi.softAP("ZIOT Charger", "88888888");
      startWebServer();
      while (!configSaved) {
        server.handleClient();
        delay(10);
      }
      connectWiFi();
    }
  } else {
    Serial.println("Entering Config Mode");
    WiFi.softAP("ZIOT Charger", "88888888");
    startWebServer();
    while (!configSaved) {
      server.handleClient();
      delay(10);
    }
    connectWiFi();
  }
}
