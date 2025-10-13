#include <MicroSDTask.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Config.h>
#include <Motorbike.h>
#include <Update.h>
#include <ContestManager.h>
#include <HardwareManager.h>
#include <NetworkManager.h>

const char *ssid_default = "PC_NET_MOBILE";
const char *password_default = "1234567890";
float otaDone;
unsigned long timerConnectWifi;

HardwareManager hardwareManager;
NetworkManager networkManager(hardwareManager);

ContestManager contestManager(hardwareManager);
JsonString contest;
TaskHandle_t webServerTaskHandle = NULL;
TaskHandle_t contestTaskHandle = NULL;

// function prototypes
void firstSetup()
{
  String data;
  JsonDocument contest1Doc;

  contest1Doc["distance1"] = "12";
  contest1Doc["distance2"] = "13";
  contest1Doc["distance3"] = "14";
  contest1Doc["distance4"] = "15";
  contest1Doc["distance5"] = "16";
  contest1Doc["distance6"] = "17";

  serializeJson(contest1Doc, data);
  hardwareManager.microSD.writeData(CONFIG_CONTEST1_PATH, data.c_str());

  JsonDocument contest2Doc;

  contest2Doc["distance7"] = "18";

  serializeJson(contest2Doc, data);
  hardwareManager.microSD.writeData(CONFIG_CONTEST2_PATH, data.c_str());

  JsonDocument contest3Doc;

  contest3Doc["distance8"] = "19";
  contest3Doc["distance9"] = "20";

  serializeJson(contest3Doc, data);
  hardwareManager.microSD.writeData(CONFIG_CONTEST2_PATH, data.c_str());

  JsonDocument contest4Doc;

  contest4Doc["distance10"] = "21";
  contest4Doc["distance11"] = "22";

  serializeJson(contest4Doc, data);
  hardwareManager.microSD.writeData(CONFIG_CONTEST2_PATH, data.c_str());
}
void wifiSetup()
{
}

// Server functionality is provided by NetworkManager now.

// main task
void webServerTask(void *pvParameters)
{
  networkManager.begin();
  networkManager.startServer();
  hardwareManager.serialLog.println("Web server task inited running on core " + String(xPortGetCoreID()));

  while (true)
  {
    networkManager.handleClient();
    if (networkManager.isNotify())
    {
      uint8_t notifyType = networkManager.getNotify();
      switch (notifyType)
      {
      case NOTIFY_RUN_STATE_ON:
        hardwareManager.serialLog.println("Starting contest from web server task...");
        xTaskNotify(contestTaskHandle, notifyType, eSetValueWithOverwrite);
        break;
      case NOTIFY_RUN_STATE_OFF:
        hardwareManager.serialLog.println("Stopping contest from web server task...");
        xTaskNotify(contestTaskHandle, notifyType, eSetValueWithOverwrite);
        break;
      default:
        break;
      }
    }

    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}
void contestTask(void *pvParameters)
{
  contestManager.begin();
  hardwareManager.serialLog.println("Contest task inited running on core " + String(xPortGetCoreID()));
  bool lastHall;
  bool lastSL;
  unsigned long timer = millis();
  uint32_t notifyType;
  while (true)
  {
    motor.update();
    hardwareManager.update();

    if (xTaskNotifyWait(0, 0, &notifyType, 0) == pdTRUE)
    {
      switch (notifyType)
      {
      case NOTIFY_RUN_STATE_ON:
        hardwareManager.serialLog.println("Starting contest from contest task...");
        contestManager.addContest(CONTEST_1_ID, 0);
        contestManager.addContest(CONTEST_2_ID, 1);
        contestManager.addContest(CONTEST_3_ID, 2);
        contestManager.addContest(CONTEST_4_ID, 3);
        contestManager.run();
        break;
      case NOTIFY_RUN_STATE_OFF:
        hardwareManager.serialLog.println("Stopping contest from contest task...");
        contestManager.stop();
        break;
      default:
        break;
      }
    }

    if (motor.getSignelLeft() && !lastSL)
    {
      hardwareManager.serialLog.println("u have just touch to Left signal !!!");
      contestManager.stop();
    }
    lastSL = motor.getSignelLeft();

    if (millis() - timer > 1000)
    {
      // hardwareManager.serialLog.println("Encoder count : " + String(motor.getEncoderCount()));
      // hardwareManager.serialLog.println("DateTime : " + hardwareManager.dateTime.toString() + " , Timestamp: " + String(hardwareManager.dateTime.getTimestamp()));
      timer = millis();
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial0.begin(115200);
  delay(1000);
  Serial.println("Booting...");

  hardwareManager.begin();
  motor.init();

  xTaskCreatePinnedToCore(webServerTask, "WebServerTask", 8192, NULL, 1, &webServerTaskHandle, 0); // Core 0
  xTaskCreatePinnedToCore(contestTask, "ContestTask", 8192, NULL, 1, &contestTaskHandle, 1);       // Core 1
}

void loop()
{
}
