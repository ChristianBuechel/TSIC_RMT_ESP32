#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"


#include "time.h"
#include "sys/time.h"


#define SPP_TAG "SPP_ACCEPTOR_DEMO"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP_SPP_ACCEPTOR"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_DATA    /*Choose show mode: show data or speed*/


    typedef struct
    {
        uint64_t time;   // time of measurement
        // something else
    } bt_event_t;

    QueueHandle_t *bt_init();

#ifdef __cplusplus
}
#endif

#endif