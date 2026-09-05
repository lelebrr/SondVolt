#ifndef STUB_I2S_H
#define STUB_I2S_H
#include <Arduino.h>
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_INTR_FLAG_LEVEL1 2
typedef enum { I2S_NUM_0=0, I2S_NUM_1=1 } i2s_port_t;
typedef enum { I2S_MODE_MASTER=1, I2S_MODE_RX=2, I2S_MODE_ADC_BUILT_IN=4 } i2s_mode_t;
typedef enum { I2S_BITS_PER_SAMPLE_16BIT=16 } i2s_bits_per_sample_t;
typedef enum { I2S_CHANNEL_FMT_ONLY_LEFT=1 } i2s_channel_fmt_t;
typedef enum { I2S_COMM_FORMAT_STAND_I2S=1 } i2s_comm_format_t;
typedef struct {
  i2s_mode_t mode; int sample_rate; i2s_bits_per_sample_t bits_per_sample;
  i2s_channel_fmt_t channel_format; i2s_comm_format_t communication_format;
  int intr_alloc_flags; int dma_buf_count; int dma_buf_len; bool use_apll;
} i2s_config_t;
esp_err_t i2s_driver_install(i2s_port_t,const i2s_config_t*,int,void*);
esp_err_t i2s_driver_uninstall(i2s_port_t);
esp_err_t i2s_set_adc_mode(int,int);
esp_err_t i2s_adc_enable(i2s_port_t);
esp_err_t i2s_adc_disable(i2s_port_t);
esp_err_t i2s_read(i2s_port_t,void*,size_t,size_t*,TickType_t);
#endif
