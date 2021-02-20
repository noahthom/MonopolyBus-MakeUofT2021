#ifndef MONOPOLY_BUS
#define MONOPOLY_BUS

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "esp_system.h"
#define MNPLY_PIN GPIO_NUM_27

uint8_t sendBuff;
uint8_t receiveBuff;

gpio_config_t mnply_pin_config;

uint8_t *rand_time;

uint32_t MNPLY_begin(uint32_t divider);

esp_err_t MNPLY_broadcast_listen(uint8_t mnply_input, uint8_t *mnply_receive);

esp_err_t MNPLY_end();


void byte2bit(uint8_t inp, uint8_t *out);
uint8_t bit2byte(uint8_t *inp);


#endif
