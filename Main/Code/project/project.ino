// project.ino
// Entry point for the project

#include <WiFi.h>
#include <WebServer.h>
#include "html.h"

const char *ssid = "PC_NET_MOBILE";
const char *password = "1234567890";

WebServer server(80);

void handleRoot()
{
    server.send(200, "text/html", indexHtml);
}

void handleSendData()
{
    if (server.hasArg("plain"))
    {
        String data = server.arg("plain");
        Serial.println("Received data: " + data);
        server.send(200, "text/plain", "Data received successfully");
    }
    else
    {
        server.send(400, "text/plain", "No data received");
    }
}

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/sendData", HTTP_POST, handleSendData);
    server.onNotFound([]()
                      { server.send(200, "text/html", indexHtml); });

    server.begin();
    Serial.println("Server started");
}

void loop()
{
    server.handleClient();
}