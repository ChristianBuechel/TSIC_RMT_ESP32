
#include "tsic_rmt.h"

#define TAG "TSIC_RMT"

static rmt_isr_handle_t s_rmt_driver_intr_handle;
int tsic_initialized = -1;

QueueHandle_t *tsic_queue;


static u_int16_t IRAM_ATTR rmt_get_mem_len(rmt_channel_t channel)
{
    u_int16_t block_num = RMT.conf_ch[channel].conf0.mem_size;
    u_int16_t item_block_len = block_num * RMT_MEM_ITEM_NUM;
    volatile rmt_item32_t* data = RMTMEM.chan[channel].data32;
    u_int16_t idx;
    for(idx = 0; idx < item_block_len; idx++) {
        if(data[idx].duration0 == 0) {
            return idx;
        } else if(data[idx].duration1 == 0) {
            return idx + 1;
        }
    }
    return idx;
}


static void IRAM_ATTR rmt_isr_handler(void *arg) //static ??
{
    //read RMT interrupt status.
    uint32_t intr_st = RMT.int_st.val;
    portBASE_TYPE HPTaskAwoken = pdFALSE;

    RMT.conf_ch[RMT_RX_CHANNEL].conf1.rx_en = 0;
    RMT.conf_ch[RMT_RX_CHANNEL].conf1.mem_owner = RMT_MEM_OWNER_TX;

    // CB first check whether we have the correct number of items (should be 39 ans start with L)

    // the zeroth 0(L) sets half the frame around 62us, we are then interested in 12(L) 14(L) 16(L) HiByte
    // then LoByte 22(L) 24 26 ... 36(L)    [38(L) is parity]

    // shift L by 10,9,8   7,6,...,0

    // make this 2 small for loops

    //CAVE each item in rmt are 2 pulses in our case L --> HIGH
    //so we want items 6 7 8   11 12 ... 18 in each case we look at duration0
 
    u_int16_t n_data = rmt_get_mem_len(RMT_RX_CHANNEL); //n pulses 

    volatile rmt_item32_t *data = RMTMEM.chan[RMT_RX_CHANNEL].data32;

    uint16_t TSIC_value = 0;
    uint8_t i, j;
    uint16_t t_strobe, bs;

    if (data[0].level0 == 0) //first pulse should be negative edge
    { t_strobe = data[0].duration0; } //first negative pulse is strobe
    else
    { t_strobe = 1000; }

    j = 10;
    for (i = 6; i < 9; i++, j--)
    {
        bs = (data[i].duration0 < t_strobe);
        TSIC_value |= (bs << j);
    }

    j = 7;
    for (i = 11; i < 19; i++, j--)
    {
        bs = (data[i].duration0 < t_strobe);
        TSIC_value |= (bs << j);
    }

    tsic_event_t evt;
    evt.time = esp_timer_get_time();
    evt.result = TSIC_value;
    evt.n_data = n_data;
    //evt.n_data = 0;
    xQueueSendFromISR(tsic_queue, &evt, &HPTaskAwoken);
 
    RMT.conf_ch[RMT_RX_CHANNEL].conf1.mem_wr_rst = 1;
    RMT.conf_ch[RMT_RX_CHANNEL].conf1.mem_owner = RMT_MEM_OWNER_RX;
    RMT.conf_ch[RMT_RX_CHANNEL].conf1.rx_en = 1;

    //clear RMT interrupt status.
    RMT.int_clr.val = intr_st;
    if (HPTaskAwoken == pdTRUE)
    { portYIELD_FROM_ISR(); } //not sure about this??
    //CB probably do the whole decoding of the waveform in ISR and simply send an int over a semaphore
}

QueueHandle_t *tsic_init()
{
    if (tsic_initialized != -1)
    {
        ESP_LOGI(TAG, "Already initialized");
        return NULL;
    }

    tsic_queue = xQueueCreate(10, sizeof(tsic_event_t));
    ESP_LOGI(TAG, "Queue created");

    // set pull-up 
  
    gpio_config_t tsic;
    tsic.intr_type = GPIO_INTR_DISABLE; 
    tsic.mode = GPIO_MODE_INPUT;
    tsic.pull_up_en = GPIO_PULLUP_ENABLE;
    tsic.pull_down_en = GPIO_PULLDOWN_DISABLE;
    tsic.pin_bit_mask = (1ULL << TSIC_PIN); //
    gpio_config(&tsic);
    
    //configure RMT for FAN (phase cut)
    rmt_config_t config;
    config.channel = RMT_RX_CHANNEL;
    config.gpio_num = TSIC_PIN;
    config.rmt_mode = RMT_MODE_RX;
    config.mem_block_num = 1; // use default of 1 memory block
    config.clk_div = 80; //us resolution
    config.rx_config.filter_en = true;
    config.rx_config.filter_ticks_thresh = 10; //only pulses longer than 10us
    config.rx_config.idle_threshold = 200; //CB here I would suggest 200us
    ESP_ERROR_CHECK(rmt_config(&config));
    rmt_set_rx_intr_en(config.channel,true);
    rmt_isr_register(rmt_isr_handler, NULL, ESP_INTR_FLAG_LEVEL1, &s_rmt_driver_intr_handle);
    rmt_rx_start(config.channel, 1);
    ESP_LOGI(TAG, "Installed RX RMT driver\n");

    tsic_initialized = 1;
    return tsic_queue;
}
