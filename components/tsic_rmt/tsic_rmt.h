#ifndef _TSIC_H_
#define _TSIC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/rmt.h"

#include "esp_log.h"
#include <stdint.h>

#define ESP_INTR_FLAG_DEFAULT 0

#define RMT_RX_CHANNEL RMT_CHANNEL_0 //channel 0 

// TSIC data pin configuration 
#define TSIC_PIN GPIO_NUM_23

    typedef struct
    {
        uint64_t time;   // time of measurement
        uint16_t result; // 11 bit value of TSIC
        uint16_t n_data; // number of pulses received
        uint16_t duty;   // duty cycle in us
    } tsic_event_t;

    QueueHandle_t *tsic_init();

#ifdef __cplusplus
}
#endif

#endif