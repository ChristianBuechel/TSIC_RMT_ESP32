/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
//#include "esp_log.h"
#include "tsic_rmt.h"

#define TAG "MAIN"

extern "C"
{
    void app_main();
}

void app_main(void)
{
    //here we start
    //setup tsic routines
    tsic_event_t tsicev;
    QueueHandle_t tsic_events = tsic_init(); //init with GPIOs would be nice

    while (1) //main loop
    {
        if (xQueueReceive(tsic_events, &tsicev, 0)) //do not wait
        {
            ESP_LOGI(TAG, "Last reading(ms): %d", uint32_t(tsicev.time / 1000));
            ESP_LOGI(TAG, "Number of items : %d", tsicev.n_data);
            ESP_LOGI(TAG, "Last reading(ms): %d", tsicev.result);
            float value = ((float)tsicev.result / 2047 * 200) - 50;
            ESP_LOGI(TAG, "Temperature     : %f", value);
        }
        vTaskDelay(10 / portTICK_RATE_MS); //wait 
    }
}
