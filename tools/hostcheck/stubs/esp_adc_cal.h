#ifndef STUB_ADC_CAL_H
#define STUB_ADC_CAL_H
#include <Arduino.h>
#include <driver/gpio.h>
#define ADC_WIDTH_BIT_12 3
typedef enum { ESP_ADC_CAL_VAL_EFUSE_VREF=0, ESP_ADC_CAL_VAL_EFUSE_TP=1,
               ESP_ADC_CAL_VAL_DEFAULT_VREF=2 } esp_adc_cal_value_t;
typedef struct { uint32_t vref; int adc_num; int atten; int bit_width; } esp_adc_cal_characteristics_t;
esp_adc_cal_value_t esp_adc_cal_characterize(int,int,int,uint32_t,esp_adc_cal_characteristics_t*);
uint32_t esp_adc_cal_raw_to_voltage(uint32_t,const esp_adc_cal_characteristics_t*);
#endif
