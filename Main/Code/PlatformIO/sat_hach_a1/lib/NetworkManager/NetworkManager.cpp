#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Config.h>
#include "NetworkManager.h"

NetworkManager::NetworkManager(HardwareManager &hwManager) : server(80)
{
    this->hardwareManager = &hwManager;
}

NetworkManager::~NetworkManager()
{
    // stop server if running
    server.stop();
}
bool NetworkManager::isNotify()
{
    return _notify;
}
uint8_t NetworkManager::getNotify()
{
    _notify = false;
    return _notifyType;
}
void NetworkManager::setNotify(uint8_t type)
{
    _notify = true;
    _notifyType = type;
}
bool NetworkManager::begin()
{
    // Read system WiFi config from SD
    String sysConfigData = hardwareManager->microSD.readData(SYSTEM_WIFI_CONFIG_PATH);
    hardwareManager->serialLog.println("System config: ");
    hardwareManager->serialLog.println(sysConfigData);

    String ssid = "";
    String pass = "";

    if (sysConfigData.length() > 0)
    {
        JsonDocument doc; // use JsonDocument instead of deprecated DynamicJsonDocument
        DeserializationError err = deserializeJson(doc, sysConfigData);
        if (!err)
        {
            if (!doc["ssid"].isNull())
                ssid = doc["ssid"].as<String>();
            if (!doc["pass"].isNull())
                pass = doc["pass"].as<String>();
        }
        else
        {
            hardwareManager->serialLog.println("Failed to parse system config: ");
            hardwareManager->serialLog.println(err.c_str());
        }
    }

    if (ssid.length() == 0)
    {
        // fallback to defaults (could be replaced)
        ssid = DEFAULT_SSID; // leave empty to indicate no auto connect
        pass = DEFAULT_PASSWORD;
    }

    if (ssid.length() > 0)
    {
        WiFi.mode(WIFI_STA);
        hardwareManager->serialLog.println("Connecting to WiFi SSID: " + ssid + " PASS: " + pass);
        WiFi.begin(ssid, pass);
        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 20000)
        {
            delay(200);
            hardwareManager->serialLog.print(".");
        }
        hardwareManager->serialLog.println(".");
        if (WiFi.status() == WL_CONNECTED)
        {
            hardwareManager->serialLog.println("Connected: " + WiFi.localIP().toString());
        }
        else
        {
            hardwareManager->serialLog.println("WiFi connect timeout");
        }
    }

    return true;
}

void NetworkManager::startServer()
{
    hardwareManager->serialLog.println("Starting server...");

    server.onNotFound([this]()
                      { handleFileRequest(); });

    server.on("/", [this]()
              { handleFileRequest(); });

    server.on("/contest", [this]()
              { handleData(server.method()); });

    server.on("/camera", HTTP_GET, [this]()
              { handleCamera(); });

    server.on("/run", HTTP_POST, [this]()
              { handleControl(); });

    server.on("/server", [this]()
              { handleServer(server.method()); });

    server.on("/update", HTTP_POST, [this]()
              { handleUpdateEnd(); }, [this]()
              { handleUpdate(); });

    server.begin();
    hardwareManager->serialLog.println("Server started.");
}

void NetworkManager::handleClient()
{
    server.handleClient();
}
void NetworkManager::handleControl()
{
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");
        hardwareManager->serialLog.println("Control POST body: " + body);

    JsonDocument doc; // use JsonDocument instead of deprecated DynamicJsonDocument
    DeserializationError error = deserializeJson(doc, body);
        if (error)
        {
            hardwareManager->serialLog.println(F("deserializeJson() failed: "));
            hardwareManager->serialLog.println(error.c_str());
            server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        if (!doc["run"].isNull())
        {
            bool action = doc["run"].as<bool>();
            hardwareManager->serialLog.println("Received action: " + String(action));
            if (action)
            {
                // Start the contest
                setNotify(NOTIFY_RUN_STATE_ON);
                server.send(200, "application/json", "{\"status\":\"Contest started\"}");
            }
            else if (!action)
            {
                // Stop the contest
                setNotify(NOTIFY_RUN_STATE_OFF);
                server.send(200, "application/json", "{\"status\":\"Contest stopped\"}");
            }
            else
            {
                server.send(400, "application/json", "{\"error\":\"Unknown action\"}");
            }
        }
        else
        {
            server.send(400, "application/json", "{\"error\":\"No action specified\"}");
        }
    }
    else
    {
        server.send(400, "text/plain", "No body received");
    }
}
void NetworkManager::handleFileRequest()
{
    String path = server.uri();
    hardwareManager->serialLog.println("Requested path: " + path);
    if (path == "/")
    {
        path = "/index.html";
    }

    String fullPath = "/Webserver" + path;

    File file = hardwareManager->microSD.openFile(fullPath.c_str(), FILE_READ);
    if (!file)
    {
        server.send(404, "text/plain", "File Not Found");
        return;
    }

    String contentType = "text/plain";
    if (path.endsWith(".html"))
        contentType = "text/html";
    else if (path.endsWith(".css"))
        contentType = "text/css";
    else if (path.endsWith(".js"))
        contentType = "application/javascript";
    else if (path.endsWith(".png"))
        contentType = "image/png";
    else if (path.endsWith(".jpg"))
        contentType = "image/jpeg";

    server.streamFile(file, contentType);
    hardwareManager->microSD.closeFile(file);
}

void NetworkManager::handleData(HTTPMethod method)
{
    if (method == HTTP_POST)
    {
        if (server.hasArg("plain"))
        {
            String body = server.arg("plain");
            hardwareManager->serialLog.println("POST body: " + body);

            JsonDocument doc; // use JsonDocument instead of deprecated DynamicJsonDocument
            DeserializationError error = deserializeJson(doc, body);
            if (error)
            {
                hardwareManager->serialLog.println(F("deserializeJson() failed: "));
                hardwareManager->serialLog.println(error.c_str());
                server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            String configData = hardwareManager->microSD.readData(CONFIG_CONTEST1_PATH);
            JsonDocument configDoc;
            if (configData.length() > 0)
            {
                DeserializationError err2 = deserializeJson(configDoc, configData);
                if (err2)
                {
                    hardwareManager->serialLog.println(F("deserializeJson() failed: "));
                    hardwareManager->serialLog.println(err2.c_str());
                    server.send(500, "application/json", "{\"error\":\"Failed to read config\"}");
                    return;
                }
            }

            for (JsonPair kv : doc.as<JsonObject>())
            {
                configDoc[kv.key()] = kv.value();
            }
            String updatedConfig;
            serializeJson(configDoc, updatedConfig);
            bool writeSuccess = hardwareManager->microSD.writeData(CONFIG_CONTEST1_PATH, updatedConfig.c_str());
            if (!writeSuccess)
            {
                server.send(500, "application/json", "{\"error\":\"Failed to save config\"}");
                return;
            }
            server.send(200, "application/json", updatedConfig);
        }
        else
        {
            server.send(400, "text/plain", "No body received");
        }
    }
    else if (method == HTTP_GET)
    {
        String configData = hardwareManager->microSD.readData(CONFIG_CONTEST1_PATH);
        server.send(200, "application/json", configData);
    }
}

void NetworkManager::handleCamera()
{
    bool success = hardwareManager->camera.capture();
    if (success)
    {
        camera_fb_t *fb = hardwareManager->camera.getPicture();
        hardwareManager->serialLog.println("Captured image: " + String(fb->len) + " bytes");
        server.sendHeader("Content-Type", "image/jpeg");
        server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
        hardwareManager->camera.returnbuffer();
    }
    else
    {
        server.send(500, "text/plain", "Camera capture failed");
    }
}

void NetworkManager::handleServer(HTTPMethod method)
{
    if (method == HTTP_POST)
    {
        if (server.hasArg("plain"))
        {
            String body = server.arg("plain");
            hardwareManager->serialLog.println("config server POST: " + body);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, body);
            if (error)
            {
                hardwareManager->serialLog.println(F("deserializeJson() failed: "));
                hardwareManager->serialLog.println(error.c_str());
                server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            String configData = hardwareManager->microSD.readData(SYSTEM_WIFI_CONFIG_PATH);
            JsonDocument configDoc;
            if (configData.length() > 0)
            {
                DeserializationError err2 = deserializeJson(configDoc, configData);
                if (err2)
                {
                    hardwareManager->serialLog.println(F("deserializeJson() failed: "));
                    hardwareManager->serialLog.println(err2.c_str());
                    server.send(500, "application/json", "{\"error\":\"Failed to read config\"}");
                    return;
                }
            }

            for (JsonPair kv : doc.as<JsonObject>())
            {
                configDoc[kv.key()] = kv.value();
            }
            String updatedConfig;
            serializeJson(configDoc, updatedConfig);
            bool writeSuccess = hardwareManager->microSD.writeData(SYSTEM_WIFI_CONFIG_PATH, updatedConfig.c_str());
            if (!writeSuccess)
            {
                server.send(500, "application/json", "{\"error\":\"Failed to save config\"}");
                return;
            }

            server.send(200, "application/json", updatedConfig);
        }
        else
        {
            server.send(400, "text/plain", "No body received");
        }
    }
    else if (method == HTTP_GET)
    {
        String configData = hardwareManager->microSD.readData(SYSTEM_WIFI_CONFIG_PATH);
        server.send(200, "application/json", configData);
    }
}

void NetworkManager::handleUpdateEnd()
{
    server.sendHeader("Connection", "close");
    if (Update.hasError())
    {
        server.send(502, "text/plain", Update.errorString());
    }
    else
    {
        server.sendHeader("Refresh", "10");
        server.sendHeader("Location", "/");
        server.send(307);
        delay(500);
        ESP.restart();
    }
}

void NetworkManager::handleUpdate()
{
    size_t fsize = UPDATE_SIZE_UNKNOWN;
    if (server.hasArg("size"))
    {
        fsize = server.arg("size").toInt();
        // timerConnectWifi = millis();
    }
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        hardwareManager->serialLog.println("Receiving Update: " + String(upload.filename) + ", Size: " + String(fsize) + "\n");
        if (!Update.begin(fsize))
        {
            otaDone = 0;
            hardwareManager->serialLog.println(Update.errorString());
        }
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        {
            hardwareManager->serialLog.println("----" + String(Update.errorString()));
        }
        else
        {
            otaDone = 100 * Update.progress() / Update.size();
        }
        if (millis() - timerConnectWifi > 1000)
        {
            hardwareManager->serialLog.println("Update : " + String(otaDone) + "%");
            timerConnectWifi = millis();
        }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (Update.end(true))
        {
            hardwareManager->serialLog.println("Update Success: " + String(upload.totalSize) + " bytes\nRebooting...\n");
        }
        else
        {
            hardwareManager->serialLog.println(String(Update.errorString()) + "\n");
            otaDone = 0;
        }
    }
}