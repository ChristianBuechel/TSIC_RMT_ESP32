/* 
An example of how to read the TSIC 306 temperature sensor Zac Wire protocol
using the ESP32 RMT engine. Its running completely in the background and an interrupt
is fired once new data has arrived. Data is transmitted to the main code 
via a queue 
*/
/*#include <stdio.h>
#include "freertos/task.h"
#include "esp_spi_flash.h"
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "esp_system.h"

#include "bluetooth.h"
#include "tsic_rmt.h"

#define TAG "MAIN"

extern "C"
{
    void app_main();
}

void app_main(void)
{

    //setup tsic module returns the handle for the queue
    tsic_event_t tsicev;
    bt_event_t btev;
    QueueHandle_t tsic_events = tsic_init();
    QueueHandle_t bt_events = bt_init();

    while (1) //main loop
    {
        if (xQueueReceive(bt_events, &btev, 0)) //do not wait
        {
            ESP_LOGI(TAG, "Recieved(ms)    : %d", btev.time);
            ESP_LOGI(TAG, "Message         : %s", btev.message);
            uint32_t pre = esp_get_free_heap_size(); 
            free(btev.message);
            uint32_t post = esp_get_free_heap_size(); 
            ESP_LOGI(TAG, "Heap before %d after %d bytes (delta: %d)", pre, post, post-pre);

        }
        if (xQueueReceive(tsic_events, &tsicev, 0)) //do not wait
        {
            /*
            ESP_LOGI(TAG, "Last reading(us): %d", uint32_t(tsicev.time));
            ESP_LOGI(TAG, "Number of items : %d", tsicev.n_data);
            ESP_LOGI(TAG, "T_Strobe(us)    : %d", tsicev.duty);
            ESP_LOGI(TAG, "Value           : %d", tsicev.result);
            float value = ((float)tsicev.result / 2047 * 200) - 50; //TSIC value to deg C
            ESP_LOGI(TAG, "Temperature (°C): %f", value);
            ESP_LOGI(TAG, "\n");
            */
            //esp_spp_write(param->write.handle, strlen(spp_data), (uint8_t *)spp_data);
        }
        vTaskDelay(10 / portTICK_RATE_MS); //wait
    }
}
