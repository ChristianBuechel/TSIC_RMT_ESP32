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


static void gpio_test_signal(void *arg)
{
	ESP_LOGI(TAG, "initializing fake pulses...");
	gpio_config_t gp;
	gp.mode = GPIO_MODE_OUTPUT;
    gp.pull_up_en = GPIO_PULLUP_ENABLE;
    gp.pull_down_en = GPIO_PULLDOWN_DISABLE;

	gp.pin_bit_mask = 1ULL << FAKE_PULSE;
	gpio_config(&gp);

	while (1)
	{
		//here the period of test signal is 10ms --> emulate ZC
		gpio_set_level(FAKE_PULSE, 0); // Set low
		ets_delay_us(60);
		gpio_set_level(FAKE_PULSE, 1); 
		ets_delay_us(70);
		gpio_set_level(FAKE_PULSE, 0); 
		ets_delay_us(80);
		gpio_set_level(FAKE_PULSE, 1); 
		ets_delay_us(90);
		gpio_set_level(FAKE_PULSE, 0); 
		ets_delay_us(80);
		gpio_set_level(FAKE_PULSE, 1); 
		ets_delay_us(70);
		gpio_set_level(FAKE_PULSE, 0); 
		ets_delay_us(60);
		gpio_set_level(FAKE_PULSE, 1); // Set high
		vTaskDelay(20);				// wait for about 200ms
	}
}

void app_main(void)
{
    //here we start

    // start fake pulses 
	xTaskCreate(gpio_test_signal, "gpio_test_signal", 4096, NULL, 5, NULL); //comment if you don't want to use capture module

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
