#include "HeaderInclude.h"
#include "DefineName.h"

volatile long encoderCount = 0;

const char *authUser = AUTH_USER;
const char *authPass = AUTH_PASS;

WebServer server(80);
Ticker tkSecond;
uint8_t otaDone = 0;

const char *csrfHeaders[2] = {"Origin", "Host"};
static bool authenticated = false;

struct WifiConfig
{
    char ssid[32];
    char password[32];
    IPAddress local_IP;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress primaryDNS;   // DNS chính (tùy chọn)
    IPAddress secondaryDNS; // DNS phụ (tùy chọn)
};
WifiConfig wifiConfig = {
    "PC_NET_MOBILE",
    "1234567890",
    IPAddress(192, 168, 1, 180), // local_IP
    IPAddress(192, 168, 1, 1),   // gateway
    IPAddress(255, 255, 255, 0), // subnet
    IPAddress(8, 8, 8, 8),       // primaryDNS
    IPAddress(8, 8, 4, 4)        // secondaryDNS
};

CameraHandler camera;
camera_fb_t *latest_fb = NULL;

Motorbike motorbike(X1_IN, X2_IN, X3_IN, ENC_A, ENC_I);
SemaphoreHandle_t fb_mutex;
SemaphoreHandle_t ledMutex;
Log logger(Serial0, 115200);
CRGB leds[NUM_LEDS];
CRGB updateleds;

TaskHandle_t ioTaskHandle = NULL;
TaskHandle_t wifiTaskHandle = NULL;
TaskHandle_t contestTaskHandle = NULL;

// Biến trạng thái nhấn nút BOOT
bool bootState, lastBootState;
unsigned long timerBootState = 0;

bool smartConfigState = false, updateOTAState = false;

unsigned long timerConnectWifi = 0;
unsigned long timerShowData = 0;

void initSys();
void smartConfig();
void remoteIP();
void updateOTA();
void connectWiFi();
void setled(int r, int g, int b);

void ioTask(void *pvParameters)
{
    DEBUG_OUT.println("ioTask Start !");

    DEBUG_OUT.println("ioTask: init Motorbike");
    motorbike.init();

    while (true)
    {
        bootState = digitalRead(BOOT_PIN);
        if (!bootState && lastBootState)
        {
            timerBootState = millis();
        }
        if (!bootState && !lastBootState)
        {
            if (millis() - timerBootState > TIME_BOOT_PRESS)
            {
                updateOTAState = true;
            }
        }
        if (bootState && !lastBootState)
        {
            if (millis() - timerBootState < TIME_BOOT_PRESS)
            {
                smartConfigState = true;
            }
        }
        lastBootState = bootState;

        motorbike.update();

        if (millis() - timerShowData > 500)
        {
            timerShowData = millis();
            Serial0.println("encoderCount: " + String(motorbike.getEncoderCount()));
        }

        // DEBUG_OUT.println("ioTask Running on core " + String(xPortGetCoreID()));

        vTaskDelay(10 / portTICK_PERIOD_MS); // Chụp mỗi 1 giây
    }
}
void wififTask(void *pvParameters)
{

    DEBUG_OUT.println("wififTask Start !");
    connectWiFi();

    smartConfigState = false;
    updateOTAState = false;

    while (1)
    {
        if (smartConfigState)
        {
            smartConfigState = false;
            smartConfig();
        }

        // DEBUG_OUT.println("wififTask Running on core " + String(xPortGetCoreID()));
        vTaskDelay(10 / portTICK_PERIOD_MS); // Đọc mỗi 10ms
    }
}
void contestTask(void *pvParameters)
{
    DEBUG_OUT.println("contestTask Start !");
    while (1)
    {   
        

        vTaskDelay(100 / portTICK_PERIOD_MS); // Cập nhật LED mỗi 100ms
    }
}
void setup()
{
    initSys();

    xTaskCreatePinnedToCore(ioTask, "ioTask", 4096, NULL, 2, &ioTaskHandle, 0);
    xTaskCreatePinnedToCore(wififTask, "wififTask", 4096, NULL, 1, &wifiTaskHandle, 1);
    xTaskCreatePinnedToCore(contestTask, "contestTask", 4096, NULL, 1, &contestTaskHandle, 0);
}

void loop()
{
    if (updateOTAState)
    {
        updateOTAState = false;
        updateOTA();
        DEBUG_OUT.println("NO UPDATE OTA ESP ...");
        ESP.restart();
    }
    showLed();
    // DEBUG_OUT.println("loop Running on core " + String(xPortGetCoreID()));
    vTaskDelay(100 / portTICK_PERIOD_MS);
}
void initSys()
{
    fb_mutex = xSemaphoreCreateMutex();
    ledMutex = xSemaphoreCreateMutex();
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.setBrightness(BRIGHTNESS);
    leds[0] = CRGB(50, 0, 0);
    // FastLED.show();

    // pinMode(ENC_A, INPUT);
    // pinMode(ENC_B, INPUT);
    // pinMode(ENC_I, INPUT);

    pinMode(BOOT_PIN, INPUT_PULLUP);

    // pinMode(X1_IN, INPUT_PULLUP);
    // pinMode(X2_IN, INPUT_PULLUP);
    // pinMode(X3_IN, INPUT_PULLUP);
    pinMode(X4_IN, INPUT_PULLUP);
    pinMode(X5_IN, INPUT_PULLUP);
    pinMode(X6_IN, INPUT_PULLUP);
    pinMode(X_IN, INPUT_PULLUP);

    EEPROM.begin(EEPROM_SIZE);

    Serial.begin(115200);
    Serial0.begin(115200);

    delay(1000);
    Serial.println("this is Serial ");
    Serial0.println("this is Serial0 ");

    Serial.println("ESP32-CAM OV2640 CAMERA_CUSTOMER ");

    logger.init(10);
    camera.init();

    // attachInterrupt(digitalPinToInterrupt(ENC_A), encoderISR, CHANGE);
}
void smartConfig()
{
    setled(0, 50, 50);
    DEBUG_OUT.println("smartConfig Start !");
    WiFi.disconnect();
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    timerConnectWifi = millis();
    DEBUG_OUT.println("Waiting for SmartConfig.");
    while (!WiFi.smartConfigDone() && (millis() - timerConnectWifi < 2 * TIME_WAIT_WIFI))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        DEBUG_OUT.print(".");
    }
    DEBUG_OUT.println("");
    DEBUG_OUT.println("SmartConfig received.");
    DEBUG_OUT.println("Waiting for WiFi");
    timerConnectWifi = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - timerConnectWifi < TIME_WAIT_WIFI))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        DEBUG_OUT.print(".");
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        DEBUG_OUT.println("WiFi not Connected. !!!!!!!!!!!");
    }
    else
    {
        DEBUG_OUT.println("WiFi Connected.");
        DEBUG_OUT.printf("SSID: %s, PASS: %s, IP: %s, gateway: %s, Subnet: %s\n",
                         WiFi.SSID().c_str(), WiFi.psk().c_str(), WiFi.localIP().toString().c_str(), WiFi.gatewayIP().toString().c_str(), WiFi.subnetMask().toString().c_str());

        wifiConfig.local_IP = WiFi.localIP();
        wifiConfig.gateway = WiFi.gatewayIP();
        wifiConfig.subnet = WiFi.subnetMask();
        strncpy(wifiConfig.ssid, WiFi.SSID().c_str(), sizeof(wifiConfig.ssid));
        strncpy(wifiConfig.password, WiFi.psk().c_str(), sizeof(wifiConfig.password));

        EEPROM.put(0, wifiConfig);
        EEPROM.commit();
    }
    setled(0, 50, 0);
}
void connectWiFi()
{
    setled(0, 0, 50);
    DEBUG_OUT.println("connectWiFi Start !");
    WifiConfig lastConfig;

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);

    DEBUG_OUT.println("Waiting for Default WiFi");
    DEBUG_OUT.printf("DEFAULT: SSID: %s, PASS: %s, IP: %s, gateway: %s, Subnet: %s\n",
                     wifiConfig.ssid, wifiConfig.password, wifiConfig.local_IP.toString().c_str(), wifiConfig.gateway.toString().c_str(), wifiConfig.subnet.toString().c_str());

    WiFi.begin(wifiConfig.ssid, wifiConfig.password);
    timerConnectWifi = millis();

    while (WiFi.status() != WL_CONNECTED && (millis() - timerConnectWifi < TIME_WAIT_WIFI))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        DEBUG_OUT.print(".");
    }
    DEBUG_OUT.println("");
    DEBUG_OUT.println(WiFi.status() == WL_CONNECTED ? "WiFi Connected." : "WiFi Not Connected.");

    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect(true, true); // force disconnect & erase old config
        vTaskDelay(500 / portTICK_PERIOD_MS);
        WiFi.mode(WIFI_OFF); // Tắt hẳn WiFi
        vTaskDelay(500 / portTICK_PERIOD_MS);
        WiFi.mode(WIFI_STA);

        EEPROM.get(0, lastConfig);
        DEBUG_OUT.printf("EEPROM : SSID: %s, PASS: %s, IP: %s, gateway: %s, Subnet: %s\n",
                         lastConfig.ssid, lastConfig.password, lastConfig.local_IP.toString().c_str(), lastConfig.gateway.toString().c_str(), lastConfig.subnet.toString().c_str());

        WiFi.begin(lastConfig.ssid, lastConfig.password);
        timerConnectWifi = millis();
        DEBUG_OUT.println("Waiting for EEPROM WiFi");
        while (WiFi.status() != WL_CONNECTED && (millis() - timerConnectWifi < TIME_WAIT_WIFI))
        {
            vTaskDelay(500 / portTICK_PERIOD_MS);
            DEBUG_OUT.print(".");
        }
        DEBUG_OUT.println("");
        DEBUG_OUT.println(WiFi.status() == WL_CONNECTED ? "WiFi Connected." : "WiFi Not Connected.");
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        strncpy(wifiConfig.ssid, WiFi.SSID().c_str(), sizeof(wifiConfig.ssid));
        strncpy(wifiConfig.password, WiFi.psk().c_str(), sizeof(wifiConfig.password));
        wifiConfig.local_IP = WiFi.localIP();
        wifiConfig.subnet = WiFi.subnetMask();
        wifiConfig.gateway = WiFi.gatewayIP();

        EEPROM.get(0, lastConfig);

        DEBUG_OUT.printf("new : SSID: %s, PASS: %s, IP: %s, gateway: %s, Subnet: %s\n",
                         WiFi.SSID().c_str(), WiFi.psk().c_str(), WiFi.localIP().toString().c_str(), WiFi.gatewayIP().toString().c_str(), WiFi.subnetMask().toString().c_str());
        DEBUG_OUT.printf("old : SSID: %s, PASS: %s, IP: %s, gateway: %s, Subnet: %s\n",
                         lastConfig.ssid, lastConfig.password, lastConfig.local_IP.toString().c_str(), lastConfig.gateway.toString().c_str(), lastConfig.subnet.toString().c_str());

        if (strcmp(wifiConfig.ssid, lastConfig.ssid) != 0 ||
            strcmp(wifiConfig.password, lastConfig.password) != 0 ||
            !(wifiConfig.local_IP == lastConfig.local_IP) ||
            !(wifiConfig.gateway == lastConfig.gateway) ||
            !(wifiConfig.subnet == lastConfig.subnet))
        {
            DEBUG_OUT.println("update EEPROM WiFi !");
            EEPROM.put(0, wifiConfig);
            EEPROM.commit();
        }
    }
    DEBUG_OUT.printf("SSID: %s, PASS: %s, IP: %s, gateway: %s, Subnet: %s\n",
                     WiFi.SSID().c_str(), WiFi.psk().c_str(), WiFi.localIP().toString().c_str(), WiFi.gatewayIP().toString().c_str(), WiFi.subnetMask().toString().c_str());
    remoteIP();
    setled(0, 50, 0);
}
void remoteIP()
{
    setled(0, 0, 50);
    DEBUG_OUT.println("remoteIP Start !");
    if (WiFi.status() != WL_CONNECTED)
        return;

    IPAddress network;
    for (int i = 0; i < 4; i++)
    {
        network[i] = (WiFi.localIP()[i] & WiFi.subnetMask()[i]);
    }

    // 3. Thử từ 180 trở lên
    int startIP = 180;
    bool found = false;
    for (int i = startIP; i < 254; i++)
    { // tránh 255 broadcast
        network[3] = i;

        DEBUG_OUT.printf("Thử IP %s ...\n", network.toString().c_str());

        if (!Ping.ping(network, 1))
        { // nếu ping không trả lời → IP trống
            DEBUG_OUT.printf("=> Chọn Static IP: %s\n", network.toString().c_str());
            wifiConfig.local_IP = network;
            found = true;
            break;
        }
    }

    if (!found)
    {
        DEBUG_OUT.println("Không tìm được IP trống!");
        return;
    }

    WiFi.disconnect();

    DEBUG_OUT.println("Configuring static IP address...");

    WiFi.config(wifiConfig.local_IP, wifiConfig.gateway, wifiConfig.subnet, wifiConfig.primaryDNS, wifiConfig.secondaryDNS);
    WiFi.begin(wifiConfig.ssid, wifiConfig.password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        DEBUG_OUT.print(".");
    }
    DEBUG_OUT.println("\nWiFi connected!");
    DEBUG_OUT.printf("NEW STATIC : SSID: %s, PASS: %s, IP: %s, gateway: %s, Subnet: %s\n",
                     WiFi.SSID().c_str(), WiFi.psk().c_str(), WiFi.localIP().toString().c_str(), WiFi.gatewayIP().toString().c_str(), WiFi.subnetMask().toString().c_str());

    setled(0, 50, 0);
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
        timerConnectWifi = millis();
    }
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        authenticated = server.authenticate(authUser, authPass);
        if (!authenticated)
        {
            DEBUG_OUT.println("Authentication fail!");
            otaDone = 0;
            return;
        }
        String origin = server.header(String(csrfHeaders[0]));
        String host = server.header(String(csrfHeaders[1]));
        String expectedOrigin = String("http://") + host;
        if (origin != expectedOrigin)
        {
            DEBUG_OUT.printf("Wrong origin received! Expected: %s, Received: %s\n", expectedOrigin.c_str(), origin.c_str());
            authenticated = false;
            otaDone = 0;
            return;
        }

        DEBUG_OUT.printf("Receiving Update: %s, Size: %d\n", upload.filename.c_str(), fsize);
        if (!Update.begin(fsize))
        {
            otaDone = 0;
            Update.printError(DEBUG_OUT);
        }
        timerConnectWifi = millis();
    }
    else if (authenticated && upload.status == UPLOAD_FILE_WRITE)
    {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        {
            Update.printError(DEBUG_OUT);
        }
        else
        {
            otaDone = 100 * Update.progress() / Update.size();
        }
        timerConnectWifi = millis();
    }
    else if (authenticated && upload.status == UPLOAD_FILE_END)
    {
        if (Update.end(true))
        {
            DEBUG_OUT.printf("Update Success: %u bytes\nRebooting...\n", upload.totalSize);
        }
        else
        {
            DEBUG_OUT.printf("%s\n", Update.errorString());
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
    DEBUG_OUT.printf("Web Server ready at http://esp32.local or http://%s\n", WiFi.localIP().toString().c_str());
}
void everySecond()
{
    if (otaDone > 1)
    {
        DEBUG_OUT.printf("ota: %d%%\n", otaDone);
    }
}
void updateOTA()
{
    setled(50, 50, 0);
    showLed();
    DEBUG_OUT.println("updateOTA Start !");
    if (WiFi.status() != WL_CONNECTED)
        return;

    webServerInit();
    // attach timer
    tkSecond.attach(1, everySecond);
    timerConnectWifi = millis();

    while (millis() - timerConnectWifi < 5 * TIME_WAIT_WIFI)
    {
        delay(100);
        server.handleClient();
    }
    setled(0, 50, 0);
    showLed();
}
void setled(int r, int g, int b)
{

    // xSemaphoreTake(ledMutex, portMAX_DELAY);
    leds[0] = CRGB(r, g, b);
    // xSemaphoreGive(ledMutex);
}
void showLed()
{
    xSemaphoreTake(ledMutex, portMAX_DELAY);
    FastLED.show();
    xSemaphoreGive(ledMutex);
}