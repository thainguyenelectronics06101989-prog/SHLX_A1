#pragma once
#include <Myconfig.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <HardwareManager.h>
#include <ClientHandler.h>

class NetworkManager
{
private:
    WebServer server;
    ClientHandler client;
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
