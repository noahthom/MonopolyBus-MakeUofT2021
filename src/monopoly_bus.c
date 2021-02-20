#include "monopoly_bus.h"


uint32_t MNPLY_begin(uint32_t divider)
{
    mnply_pin_config.intr_type= GPIO_INTR_DISABLE;
    mnply_pin_config.mode = GPIO_MODE_INPUT;
    mnply_pin_config.pin_bit_mask = (1<<MNPLY_PIN);
    mnply_pin_config.pull_up_en = 1;

    gpio_config(&mnply_pin_config);

    timer_config_t virtual_clk;

    virtual_clk.divider = divider; /// ideally 1000
    virtual_clk.counter_en = TIMER_PAUSE;
    virtual_clk.counter_dir = TIMER_COUNT_UP;
    virtual_clk.alarm_en = TIMER_ALARM_DIS;
    virtual_clk.auto_reload = TIMER_AUTORELOAD_DIS;
    //timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 1);
    timer_init(TIMER_GROUP_1, TIMER_0, &virtual_clk);

    timer_config_t wait_timer;
    wait_timer.divider = divider; 
    wait_timer.counter_en = TIMER_PAUSE;
    wait_timer.counter_dir = TIMER_COUNT_UP;
    wait_timer.alarm_en = TIMER_ALARM_DIS;
    wait_timer.auto_reload = TIMER_AUTORELOAD_DIS;
    //timer_set_counter_value(TIMER_GROUP_1, TIMER_1, 1);
    timer_init(TIMER_GROUP_1, TIMER_1, &wait_timer);

    uint32_t tick_rate = 60000000 / divider;
    return tick_rate;



}


esp_err_t MNPLY_broadcast_listen(uint8_t mnply_send, uint8_t *mnply_receive)
{
    uint64_t timeline = 0;
    uint64_t timer_value = 0;
    uint8_t inp_buff[8];
    uint8_t out_buff[8];

    byte2bit(mnply_send, out_buff);

    uint8_t x = esp_random();

    
    timer_start(TIMER_GROUP_1, TIMER_1);
    timer_set_counter_value(TIMER_GROUP_1, TIMER_1, 1);
    while ((uint64_t)x != timer_value)
    {   
        timer_get_counter_value(TIMER_GROUP_1, TIMER_1, &timer_value);
        if (gpio_get_level(MNPLY_PIN) == 0)
        {
            timer_pause(TIMER_GROUP_1, TIMER_1);

            timer_start(TIMER_GROUP_1, TIMER_0);
            timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 1);
            while(timeline < 100)
            {
                timer_get_counter_value(TIMER_GROUP_1, TIMER_0, &timeline);
                while(timeline >= 4 && timeline <= 11)
                {
                    inp_buff[timeline - 4] = gpio_get_level(MNPLY_PIN);
                }
            }
            timeline = 0;
            timer_pause(TIMER_GROUP_1, TIMER_0);
            timer_start(TIMER_GROUP_1, TIMER_1);
        }
        
    }
    if(gpio_get_level(MNPLY_PIN) == 1)
    {
        mnply_pin_config.intr_type= GPIO_INTR_DISABLE;
        mnply_pin_config.mode = GPIO_MODE_OUTPUT;
        mnply_pin_config.pin_bit_mask = (1<<MNPLY_PIN);
        mnply_pin_config.pull_up_en = 0;
        mnply_pin_config.pull_down_en = 0;

        gpio_config(&mnply_pin_config);
        gpio_set_level(MNPLY_PIN, 0);
        timeline = 0;


        timer_start(TIMER_GROUP_1, TIMER_0);
        timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 1);
        while(timeline < 100)
        {
            timer_get_counter_value(TIMER_GROUP_1, TIMER_0, &timeline);
            while(timeline >= 4 && timeline <= 11)
            {
                gpio_set_level(MNPLY_PIN, out_buff[timeline - 4]);
            }
        }
    }
    timer_pause(TIMER_GROUP_1, TIMER_0);
    timer_pause(TIMER_GROUP_1, TIMER_1);

    mnply_pin_config.intr_type= GPIO_INTR_DISABLE;
    mnply_pin_config.mode = GPIO_MODE_INPUT;
    mnply_pin_config.pin_bit_mask = (1<<MNPLY_PIN);
    mnply_pin_config.pull_up_en = 1;
    gpio_config(&mnply_pin_config);

    *(mnply_receive) = bit2byte(inp_buff);
    return ESP_OK;
}


esp_err_t MNPLY_end()
{
    mnply_pin_config.intr_type= GPIO_INTR_DISABLE;
    mnply_pin_config.mode = GPIO_MODE_DISABLE;
    mnply_pin_config.pin_bit_mask = (1<<MNPLY_PIN);
    mnply_pin_config.pull_up_en = 0;
    mnply_pin_config.pull_down_en = 0;

    timer_deinit(TIMER_GROUP_1, TIMER_0);
    timer_deinit(TIMER_GROUP_1, TIMER_1);
    return ESP_OK;



}


void byte2bit(uint8_t inp, uint8_t *out)
{
    for(int i = 7; i >= 0; i--)
    {
        *(out+i) = inp%2;
        inp = inp/2;
    }

}

uint8_t bit2byte(uint8_t *inp)
{
    uint8_t num = 0;
    for(int i = 0; i < 8; i++)
    {
        num += (*(inp+i)) * pow(2,i);
    }
    return num;
}

