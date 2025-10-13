#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <FS.h>
#include <SPIFFS.h>
#include "WebInterface.h"
#include <WebSocketsServer.h>

// WiFi credentials
const char* ssid = "PC_NET_MOBILE";
const char* password = "1234567890";

WebServer server(80); 
WebSocketsServer webSocket = WebSocketsServer(81);

void handleOTAUpdate() 
{
    HTTPUpload& upload = server.upload();
    static size_t lastPercent = 0; // Lưu trạng thái phần trăm trước đó

    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { // Start with max available size
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        } else {
            // Tính toán phần trăm cập nhật
            size_t percent = (Update.progress() * 100) / Update.size();
            if (percent != lastPercent) { // Chỉ hiển thị khi phần trăm thay đổi
                lastPercent = percent;
                Serial.printf("Update Progress: %u%%\n", percent);
            }
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { // true to set the size
            Serial.printf("Update Success: %u bytes\n", upload.totalSize);
            Serial.println("Rebooting...");
            ESP.restart();
        } else {
            Update.printError(Serial);
        }
    }
}

void setupOTAWebServer() {
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", otaUpdatePage);
    });

    server.on("/update", HTTP_POST, []() {
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    }, handleOTAUpdate);

    server.on("/progress", HTTP_GET, []() {
        if (Update.isRunning()) {
            size_t percent = (Update.progress() * 100) / Update.size();
            server.send(200, "text/plain", String(percent) + "%");
        } else {
            server.send(200, "text/plain", "No update in progress");
        }
    });

    server.begin();
    Serial.println("HTTP server started");
}
void startOTAUpdateServer() {
    setupOTAWebServer();
    Serial.println("OTA Update Server started. Press the reset button to exit.");
    while (true) {
        server.handleClient();
        delay(10); // Avoid watchdog reset
    }
}
void handleWebSocketMessage(uint8_t num, uint8_t *payload, size_t length) {
    String message = "";
    for (size_t i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.printf("Client %u sent: %s\n", num, message.c_str());

    // Xử lý dữ liệu từ client
    if (message == "ping") {
        webSocket.sendTXT(num, "pong"); // Phản hồi lại client
    } else if (message.startsWith("set:")) {
        String value = message.substring(4); // Lấy giá trị sau "set:"
        Serial.printf("Set value: %s\n", value.c_str());
        // Thực hiện hành động với giá trị nhận được
    } else {
        webSocket.sendTXT(num, "Unknown command");
    }
}

void manageWebSocketClients() {
    webSocket.loop();
}

void setupWebSocketServer() {
    webSocket.begin();
    webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
        switch (type) {
            case WStype_DISCONNECTED:
                Serial.printf("Client %u disconnected\n", num);
                break;
            case WStype_CONNECTED:
                Serial.printf("Client %u connected\n", num);
                webSocket.sendTXT(num, "Welcome to WebSocket Server!");
                break;
            case WStype_TEXT:
                handleWebSocketMessage(num, payload, length);
                break;
            default:
                break;
        }
    });
    Serial.println("WebSocket server started on port 81");
}

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());

    setupWebSocketServer();

    pinMode(0, INPUT_PULLUP); // GPIO0 is typically the BOOT button
    if (digitalRead(0) == LOW) {
        Serial.println("BOOT button pressed. Starting OTA Update Server...");
        startOTAUpdateServer();
    }
}

void loop() {
    manageWebSocketClients();
    server.handleClient();
}