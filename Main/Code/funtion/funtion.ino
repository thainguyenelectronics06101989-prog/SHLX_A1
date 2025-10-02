// #define DEBUG_CORE

// #define DEBUG_SOCKET
//  #define DEBUG_SENSOR

#define PIN 48
#define NUMPIXELS 1

#define X1_IN 41
#define X2_IN 42
#define X3_IN 2
#define X4_IN 1
#define X5_IN 3
#define X6_IN 46
#define X_IN 47

#define ENC_A 21
#define ENC_B 48
#define ENC_I 45

#define EEPROM_SIZE 64

#include <EEPROM.h>
#include <WiFi.h>
#include "CameraHandler.h"
#include "Log.h"
#include <Adafruit_NeoPixel.h>

// Thông tin WiFi
const char *ssid = "PC_NET_MOBILE";
const char *password = "1234567890";

// Socket server info
const char *server_ip = "192.168.98.31";
const uint16_t server_port = 5000;

volatile long encoderCount = 0;

CameraHandler camera;
camera_fb_t *latest_fb = NULL;
SemaphoreHandle_t fb_mutex;
Log logger(Serial0, 115200);
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

TaskHandle_t cameraTaskHandle = NULL;
TaskHandle_t encoderTaskHandle = NULL;
TaskHandle_t socketTaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;

void IRAM_ATTR encoderISR()
{
    int A = digitalRead(ENC_A);
    int B = digitalRead(ENC_B);

    // Logic xác định chiều quay
    if (A == HIGH)
    {
        if (B == LOW)
            encoderCount++;
        else
            encoderCount--;
    }
    else
    {
        if (B == HIGH)
            encoderCount++;
        else
            encoderCount--;
    }
}
void cameraTask(void *pvParameters)
{
    if (camera.init())
    {
        Serial.println("Camera task started");
    }
    else
    {

        while (true)
        {
            Serial.println("Camera init failed");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    while (true)
    {
        // if (camera.capture())
        // {
        //     xSemaphoreTake(fb_mutex, portMAX_DELAY);
        //     if (latest_fb)
        //         esp_camera_fb_return(latest_fb);
        //     latest_fb = camera.getPicture();
        //     xSemaphoreGive(fb_mutex);
        // }
        // else
        // {
        //     Serial.println("Failed to capture image in camera task");
        Serial.printf("Task cam running on core %d\n", xPortGetCoreID());
        delay(1000);
        // }

        vTaskDelay(3000 / portTICK_PERIOD_MS); // Chụp mỗi 1 giây
    }
}

// Encoder task: đọc encoder (giả lập)
void encoderTask(void *pvParameters)
{
    pinMode(ENC_A, INPUT);
    pinMode(ENC_B, INPUT);
    pinMode(ENC_I, INPUT);

    pinMode(X1_IN, INPUT_PULLUP);
    pinMode(X2_IN, INPUT_PULLUP);
    pinMode(X3_IN, INPUT_PULLUP);
    pinMode(X4_IN, INPUT_PULLUP);
    pinMode(X5_IN, INPUT_PULLUP);
    pinMode(X6_IN, INPUT_PULLUP);
    pinMode(X_IN, INPUT_PULLUP);

    while (1)
    {
        if (digitalRead(X1_IN) == HIGH)
        {
            logger.logI(1, "X1_IN HIGH");
        }
        else
        {
            logger.logI(1, "X1_IN LOW");
        }
        if (digitalRead(X2_IN) == HIGH)
        {
            logger.logI(2, "X2_IN HIGH");
        }
        else
        {
            logger.logI(2, "X2_IN LOW");
        }
        if (digitalRead(X3_IN) == HIGH)
        {
            logger.logI(3, "X3_IN HIGH");
        }
        else
        {
            logger.logI(3, "X3_IN LOW");
        }
        if (digitalRead(X4_IN) == HIGH)
        {
            logger.logI(4, "X4_IN HIGH");
        }
        else
        {
            logger.logI(4, "X4_IN LOW");
        }
        if (digitalRead(X5_IN) == HIGH)
        {
            logger.logI(5, "X5_IN HIGH");
        }
        else
        {
            logger.logI(5, "X5_IN LOW");
        }
        if (digitalRead(X6_IN) == HIGH)
        {
            logger.logI(6, "X6_IN HIGH");
        }
        else
        {
            logger.logI(6, "X6_IN LOW");
        }
        if (digitalRead(X_IN) == HIGH)
        {
            logger.logI(7, "X_IN HIGH");
        }
        else
        {
            logger.logI(7, "X_IN LOW");
        }
        // Đọc encoder ở đây (ví dụ: digitalRead, hoặc dùng interrupt)
        // int encoderValue = digitalRead(ENCODER_PIN);
        // Xử lý dữ liệu encoder nếu cần
        vTaskDelay(10 / portTICK_PERIOD_MS); // Đọc mỗi 10ms
    }
}

// Socket task: truyền ảnh lên server
void socketTask(void *pvParameters)
{
    WiFiClient client;

    while (1)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi disconnected, trying to reconnect...");
            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED)
            {
                vTaskDelay(500 / portTICK_PERIOD_MS);
                Serial.print(".");
            }
            Serial.println("\nWiFi reconnected!");
        }
        if (!client.connected())
        {
            client.connect(server_ip, server_port);
            Serial.println("Connecting to server ");
            while (!client.connected())
            {
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                Serial.print(".");
                client.connect(server_ip, server_port);
            }
            Serial.println("Connect to server : " + String(server_ip) + " Port: " + String(server_port));
        }
        while (client.connected())
        {
            if (camera.capture())
            {
                client.write((uint8_t *)&camera.fb->len, 4); // Gửi kích thước ảnh trước
                client.write((uint8_t *)camera.fb->buf, camera.fb->len);
                Serial.printf("Sent image: %d bytes\n", camera.fb->len);
            }

            xSemaphoreTake(fb_mutex, portMAX_DELAY);
            // if (latest_fb)
            // {
            // client.write((uint8_t *)&latest_fb->len, 4); // Gửi kích thước ảnh trước

            // client.write((uint8_t *)latest_fb->buf, latest_fb->len);
            // Serial.printf("Sent image: %d bytes\n", latest_fb->len);
            // }
            xSemaphoreGive(fb_mutex);
            camera.returnbuffer();
            client.flush();

            Serial.println("Image sent to server " + client.connected() ? " connected" : " disconnected");
            vTaskDelay(2000 / portTICK_PERIOD_MS); // Gửi mỗi 2 giây
            Serial.println("Image sent to server " + client.connected() ? " connected" : " disconnected");
        }
    }
}

// Sensor task: tính toán cảm biến (giả lập)
void sensorTask(void *pvParameters)
{
    while (1)
    {
#ifdef DEBUG_SENSOR
        Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
        Serial.printf("Min free heap: %u bytes\n", ESP.getMinFreeHeap());
        TaskStatus_t *pxTaskStatusArray;
        UBaseType_t uxArraySize, x;
        uint32_t totalRunTime;

        uxArraySize = uxTaskGetNumberOfTasks();
        pxTaskStatusArray = (TaskStatus_t *)pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
        if (pxTaskStatusArray != NULL)
        {
            uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &totalRunTime);
            Serial.println("Task Name\tState\tPrio\tStack\tNum");
            for (x = 0; x < uxArraySize; x++)
            {
                Serial.printf("%s\t%d\t%d\t%u\t%d\n",
                              pxTaskStatusArray[x].pcTaskName,
                              pxTaskStatusArray[x].eCurrentState,
                              pxTaskStatusArray[x].uxCurrentPriority,
                              pxTaskStatusArray[x].usStackHighWaterMark,
                              pxTaskStatusArray[x].xTaskNumber);
            }
            vPortFree(pxTaskStatusArray);
        }
#endif

        logger.showLog();
        Serial.printf("Task sensor running on core %d\n", xPortGetCoreID());
        delay(1500);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}
void setup()
{
    pixels.begin();
    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(50, 0, 0));
    pixels.show();
    EEPROM.begin(EEPROM_SIZE);

    Serial.begin(115200);
    Serial0.begin(115200);


    delay(1000);
    Serial.println("this is Serial ");
    Serial0.println("this is Serial0 ");






    pixels.setPixelColor(0, pixels.Color(0, 0, 50));
    pixels.show();
    Serial.println("ESP32-CAM OV2640 CAMERA_CUSTOMER ");

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");

    logger.init(10);
    pixels.setPixelColor(0, pixels.Color(0, 50, 0));
    pixels.show();

    attachInterrupt(digitalPinToInterrupt(ENC_A), encoderISR, CHANGE);

    fb_mutex = xSemaphoreCreateMutex();

    // Tạo các task, gán core
    xTaskCreate(cameraTask, "CameraTask", 4096, NULL, 2, &cameraTaskHandle);    // Core 0
    xTaskCreate(encoderTask, "EncoderTask", 2048, NULL, 1, &encoderTaskHandle); // Core 0
    xTaskCreate(socketTask, "SocketTask", 4096, NULL, 1, &socketTaskHandle);    // Core 1
    xTaskCreate(sensorTask, "SensorTask", 2048, NULL, 1, &sensorTaskHandle);    // Core 1
}

void loop()
{
}