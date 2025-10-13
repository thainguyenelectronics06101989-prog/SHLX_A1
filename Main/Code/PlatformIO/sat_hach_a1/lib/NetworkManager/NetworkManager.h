#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <HardwareManager.h>

#define DEFAULT_SSID "PC_NET_MOBILE"
#define DEFAULT_PASSWORD "1234567890"

#define NOTIFY_RUN_STATE 0x60
#define NOTIFY_RUN_STATE_ON 0x61
#define NOTIFY_RUN_STATE_OFF 0x62

class NetworkManager
{
private:
    WebServer server;
    HardwareManager *hardwareManager;
    float otaDone = 0.0f;
    unsigned long timerConnectWifi = 0;
    bool _notify = false;
    uint8_t _notifyType = 0;
    void setNotify(uint8_t type);

public:
    NetworkManager(HardwareManager &hwManager);
    ~NetworkManager();
    bool begin();
    void startServer();
    void handleClient();
    void handleFileRequest();
    void handleData(HTTPMethod method);
    void handleCamera();
    void handleControl();
    void handleServer(HTTPMethod method);
    void handleUpdate();
    void handleUpdateEnd();

    uint8_t getNotify();
    bool isNotify();
};
