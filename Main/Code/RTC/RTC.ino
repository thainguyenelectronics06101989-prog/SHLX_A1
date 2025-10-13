#include "time.h"

void setup()
{
    Serial.begin(115200);

    // Khai báo cấu trúc thời gian
    struct tm timeinfo = {};
    timeinfo.tm_year = 2025 - 1900; // Năm (tính từ 1900)
    timeinfo.tm_mon = 9;            // Tháng (0 = Jan, 9 = Oct)
    timeinfo.tm_mday = 4;           // Ngày
    timeinfo.tm_hour = 14;
    timeinfo.tm_min = 30;
    timeinfo.tm_sec = 0;

    // Chuyển struct tm -> time_t
    time_t t = mktime(&timeinfo);

    // Đặt thời gian hệ thống (RTC nội)
    struct timeval now = {.tv_sec = t};
    settimeofday(&now, NULL);

    Serial.println("✅ Đã cấu hình RTC nội ESP32");

    // In ra để kiểm tra
    printTime();
}

void printTime()
{
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        Serial.printf("⏰ %02d/%02d/%04d %02d:%02d:%02d\n",
                      timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
                      timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }
    else
    {
        Serial.println("⚠️ Không lấy được thời gian!");
    }
}

void loop()
{
    delay(1000);
    printTime();
}