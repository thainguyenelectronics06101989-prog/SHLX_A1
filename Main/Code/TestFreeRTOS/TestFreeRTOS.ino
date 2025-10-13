
// Mục đích	                    Cách phù hợp nhất	                    Ghi chú
// Gửi dữ liệu nhỏ 1 chiều	Biến global (có volatile)	            Nhanh, đơn giản
// Gửi nhiều dữ liệu	        xQueue	FIFO, thread-safe
// Bảo vệ ngoại vi (SD, SPI)	xSemaphore hoặc xMutex	            Ngăn truy cập đồng thời
// Đoạn code ngắn cần bảo vệ	portENTER_CRITICAL()	            Nhanh, cấp thấp
// Gửi tín hiệu (event)	        xTaskNotify hoặc EventGroup	        Nhẹ, hiệu quả

TaskHandle_t cameraTaskHandle = NULL;
TaskHandle_t encoderTaskHandle = NULL;

void cameraTask(void *pvParameters)
{
    while (true)
    {

        Serial.printf("Task cam running on core %d\n", xPortGetCoreID());

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Chụp mỗi 1 giây
    }
}
void encoderTask(void *pvParameters)
{
    while (1)
    {
        Serial.printf("encoder task running on core %d\n", xPortGetCoreID());
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Đọc mỗi 10ms
    }
}
void setup()
{
    Serial.begin(115200);

    delay(1000);

    xTaskCreatePinnedToCore(cameraTask, "CameraTask", 4096, NULL, 2, &cameraTaskHandle, 0);    // Core 0
    xTaskCreatePinnedToCore(encoderTask, "EncoderTask", 2048, NULL, 1, &encoderTaskHandle, 1); // Core 0
}
void loop()
{
    Serial.printf("main task running on core %d\n", xPortGetCoreID());
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Đọc mỗi 10ms
}