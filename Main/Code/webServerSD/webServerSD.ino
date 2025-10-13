#include "MicroSDTask.h"
#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "PC_NET_MOBILE";
const char *password = "1234567890";

MicroSDTask microSD;
WebServer server(80);

void handleFileRequest()
{
    String path = server.uri();
    if (path == "/")
    {
        path = "/index.html";
    }

    String fullPath = "/sdcard/Webserver" + path;

    File file = microSD.openFile(fullPath.c_str(), FILE_READ);

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
    microSD.closeFile(file);
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    if (microSD.begin())
    {
        Serial.println("MicroSD initialized successfully.");
    }
    else
    {
        Serial.println("MicroSD initialization failed: " + microSD.getError());
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi. IP address: " + WiFi.localIP().toString());

    server.onNotFound(handleFileRequest);

    server.begin();
}

void loop()
{
    server.handleClient();
}
