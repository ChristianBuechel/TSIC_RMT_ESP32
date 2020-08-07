/* 
An example of how to read the TSIC 306 temperature sensor Zac Wire protocol
using the ESP32 RMT engine. Its running completely in the background and an interrupt
is fired once new data has arrived. Data is transmitted to the main code 
via a queue 
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
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
    QueueHandle_t tsic_events = tsic_init(); 

    while (1) //main loop
    {
        if (xQueueReceive(tsic_events, &tsicev, 0)) //do not wait
        {
            ESP_LOGI(TAG, "Last reading(us): %d", uint32_t(tsicev.time));
            ESP_LOGI(TAG, "Number of items : %d", tsicev.n_data);
            ESP_LOGI(TAG, "T_Strobe(us)    : %d", tsicev.duty);
            ESP_LOGI(TAG, "Value           : %d", tsicev.result);
            float value = ((float)tsicev.result / 2047 * 200) - 50; //TSIC value to deg C
            ESP_LOGI(TAG, "Temperature (°C): %f", value);
            ESP_LOGI(TAG, "\n");
        }
        vTaskDelay(10 / portTICK_RATE_MS); //wait 
    }
}
