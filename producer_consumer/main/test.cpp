#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>

#define MAX_COUNT 10
#define TAG "app"

QueueHandle_t m_number_queue{xQueueCreate(5, sizeof(int))};
void producer(void *p);
void consumer(void *p);

extern "C" void app_main()
{
    ESP_LOGI(TAG, "application started");
    xTaskCreate(producer, "producer", 4096, nullptr, 5, nullptr);
    xTaskCreatePinnedToCore(consumer, "consumer-0", 4096, (void *)0, 5, nullptr, 0);
    xTaskCreatePinnedToCore(consumer, "consumer-1", 4096, (void *)1, 5, nullptr, 1);

    char buffer[256]{0};
    vTaskList(buffer);
    ESP_LOGI(TAG, "\n%s", buffer);
}

void producer(void *p)
{
    int cnt{0};
    vTaskDelay(pdMS_TO_TICKS(500));

    while (++cnt <=MAX_COUNT)
    {
        xQueueSendToBack(m_number_queue, &cnt, portMAX_DELAY);
        ESP_LOGI(TAG, "p:%d", cnt);
    }

    vTaskDelete(nullptr);
}

void consumer(void *p)
{
    int num;

    while(true)
    {
        xQueueReceive(m_number_queue, &num, portMAX_DELAY);
        ESP_LOGI(TAG, "c%d:%d", (int) p, num);
        vTaskDelay(2);
    }
}
