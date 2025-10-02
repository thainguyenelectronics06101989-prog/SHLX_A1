#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <Ticker.h>
#include <ESPping.h>
#include "html.h"

#define SSID_FORMAT "ESP32" // 12 chars total
#define PASSWORD "12345678" // generate if remarked

// Địa chỉ IP tĩnh muốn gán
IPAddress local_IP;
IPAddress gateway;
IPAddress subnet;
IPAddress primaryDNS(8, 8, 8, 8);   // DNS chính (tùy chọn)
IPAddress secondaryDNS(8, 8, 4, 4); // DNS phụ (tùy chọn)

// Set the username and password for firmware upload
const char *authUser = "phamchinh";
const char *authPass = "phamchinh202";

WebServer server(80);
Ticker tkSecond;
uint8_t otaDone = 0;

const char *csrfHeaders[2] = {"Origin", "Host"};
static bool authenticated = false;

const char *alphanum = "0123456789!@#$%^&*abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
String generatePass(uint8_t str_len)
{
    String buff;
    for (int i = 0; i < str_len; i++)
    {
        buff += alphanum[random(strlen(alphanum) - 1)];
    }
    return buff;
}

void apMode()
{
    char ssid[13];
    char passwd[11];
    long unsigned int espmac = ESP.getEfuseMac() >> 24;
    snprintf(ssid, 13, SSID_FORMAT, espmac);
#ifdef PASSWORD
    snprintf(passwd, 11, PASSWORD);
#else
    snprintf(passwd, 11, generatePass(10).c_str());
#endif
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, passwd); // Set up the SoftAP
    MDNS.begin("esp32");
    Serial.printf("AP: %s, PASS: %s\n", ssid, passwd);
}

void staMode()
{
    char *ssid = "PC_NET_MOBILE";
    char *password = "1234567890";
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.printf("STA: %s, PASS: %s, DHCP IP: %s, gateway: %s, Subnet: %s\n",
                  ssid, password, WiFi.localIP().toString().c_str(), WiFi.gatewayIP().toString().c_str(), WiFi.subnetMask().toString().c_str());
    gateway = WiFi.gatewayIP();
    subnet = WiFi.subnetMask();

    // 2. Tính network
    IPAddress network;
    for (int i = 0; i < 4; i++)
    {
        network[i] = (WiFi.localIP()[i] & subnet[i]);
    }

    // 3. Thử từ 180 trở lên
    int startIP = 180;
    bool found = false;
    for (int i = startIP; i < 254; i++)
    { // tránh 255 broadcast
        local_IP = network;
        local_IP[3] = i;

        Serial.printf("Thử IP %s ...\n", local_IP.toString().c_str());

        if (!Ping.ping(local_IP, 1))
        { // nếu ping không trả lời → IP trống
            Serial.printf("=> Chọn Static IP: %s\n", local_IP.toString().c_str());
            found = true;
            break;
        }
    }

    if (!found)
    {
        Serial.println("Không tìm được IP trống!");
        return;
    }

    WiFi.disconnect();

    Serial.println("Configuring static IP address...");

    WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.printf("STA: %s, PASS: %s, STATIC IP: %s, gateway: %s\n", ssid, password, WiFi.localIP().toString().c_str(), WiFi.gatewayIP().toString().c_str());
}

void handleUpdateEnd()
{
    if (!authenticated)
    {
        return server.requestAuthentication();
    }
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

void handleUpdate()
{
    size_t fsize = UPDATE_SIZE_UNKNOWN;
    if (server.hasArg("size"))
    {
        fsize = server.arg("size").toInt();
    }
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        authenticated = server.authenticate(authUser, authPass);
        if (!authenticated)
        {
            Serial.println("Authentication fail!");
            otaDone = 0;
            return;
        }
        String origin = server.header(String(csrfHeaders[0]));
        String host = server.header(String(csrfHeaders[1]));
        String expectedOrigin = String("http://") + host;
        if (origin != expectedOrigin)
        {
            Serial.printf("Wrong origin received! Expected: %s, Received: %s\n", expectedOrigin.c_str(), origin.c_str());
            authenticated = false;
            otaDone = 0;
            return;
        }

        Serial.printf("Receiving Update: %s, Size: %d\n", upload.filename.c_str(), fsize);
        if (!Update.begin(fsize))
        {
            otaDone = 0;
            Update.printError(Serial);
        }
    }
    else if (authenticated && upload.status == UPLOAD_FILE_WRITE)
    {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        {
            Update.printError(Serial);
        }
        else
        {
            otaDone = 100 * Update.progress() / Update.size();
        }
    }
    else if (authenticated && upload.status == UPLOAD_FILE_END)
    {
        if (Update.end(true))
        {
            Serial.printf("Update Success: %u bytes\nRebooting...\n", upload.totalSize);
        }
        else
        {
            Serial.printf("%s\n", Update.errorString());
            otaDone = 0;
        }
    }
}

void webServerInit()
{
    server.collectHeaders(csrfHeaders, 2);
    server.on(
        "/update", HTTP_POST,
        []()
        {
            handleUpdateEnd();
        },
        []()
        {
            handleUpdate();
        });
    server.on("/favicon.ico", HTTP_GET, []()
              {
    server.sendHeader("Content-Encoding", "gzip");
    server.send_P(200, "image/x-icon", favicon_ico_gz, favicon_ico_gz_len); });
    server.onNotFound([]()
                      {
    if (!server.authenticate(authUser, authPass)) {
      return server.requestAuthentication();
    }
    server.send(200, "text/html", indexHtml); });
    server.begin();
    Serial.printf("Web Server ready at http://esp32.local or http://%s\n", WiFi.softAPIP().toString().c_str());
}

void everySecond()
{
    if (otaDone > 1)
    {
        Serial.printf("ota: %d%%\n", otaDone);
    }
}

void setup()
{
    Serial0.begin(115200);

    Serial.begin(115200);
    delay(1000);
    Serial.println("esp32 OTA ");
    Serial0.println("Serial0: esp32 OTA ");
    // apMode();
    staMode();
    webServerInit();
    tkSecond.attach(1, everySecond);
}

void loop()
{
    delay(150);
    server.handleClient();
}
