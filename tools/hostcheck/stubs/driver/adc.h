#ifndef STUB_ADC_H
#define STUB_ADC_H
#include <driver/gpio.h>
typedef int adc1_channel_t;
int adc1_config_channel_atten(adc1_channel_t,int);
#endif
