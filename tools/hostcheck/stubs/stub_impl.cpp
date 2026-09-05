// Implementacoes vazias para o link do harness de verificacao no host.
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <driver/i2s.h>
#include <driver/adc.h>
#include <MLX90640_API.h>
#include <esp_adc_cal.h>
HardwareSerial Serial;
EspClass ESP;
SPIClass SPI;
TwoWire Wire;
WiFiClass WiFi;
ArduinoOTAClass ArduinoOTA;
static unsigned long g_ms=0;
void pinMode(int,int){}
void digitalWrite(int,int){}
int  digitalRead(int){return 0;}
int  analogRead(int){return 2048;}
void analogReadResolution(int){}
void analogSetPinAttenuation(int,int){}
unsigned long millis(){return ++g_ms;}
unsigned long micros(){return (++g_ms)*1000;}
void delay(unsigned long){}
void delayMicroseconds(unsigned long){}
long map(long x,long a,long b,long c,long d){ return (b==a)?c:(x-a)*(d-c)/(b-a)+c; }
void ledcAttach(int,int,int){}
void ledcWrite(int,int){}
void ledcSetup(uint8_t,uint32_t,uint8_t){}
void ledcAttachPin(uint8_t,uint8_t){}
void ledcWriteTone(uint8_t,uint32_t){}
uint32_t getCpuFrequencyMhz(){return 240;}
uint32_t temperatureRead(){return 45;}
BaseType_t xTaskCreate(void(*)(void*),const char*,uint32_t,void*,UBaseType_t,TaskHandle_t*){return 1;}
BaseType_t xTaskCreatePinnedToCore(void(*)(void*),const char*,uint32_t,void*,UBaseType_t,TaskHandle_t*,int){return 1;}
void vTaskDelay(TickType_t){}
void vTaskDelete(TaskHandle_t){}
uint32_t uxTaskGetStackHighWaterMark(TaskHandle_t){return 1024;}
SemaphoreHandle_t xSemaphoreCreateRecursiveMutex(){return (SemaphoreHandle_t)1;}
SemaphoreHandle_t xSemaphoreCreateMutex(){return (SemaphoreHandle_t)1;}
BaseType_t xSemaphoreTakeRecursive(SemaphoreHandle_t,TickType_t){return 1;}
BaseType_t xSemaphoreGiveRecursive(SemaphoreHandle_t){return 1;}
BaseType_t xSemaphoreTake(SemaphoreHandle_t,TickType_t){return 1;}
BaseType_t xSemaphoreGive(SemaphoreHandle_t){return 1;}
void esp_task_wdt_reset(){}
int  esp_task_wdt_add(TaskHandle_t){return 0;}
int  esp_task_wdt_init(uint32_t,bool){return 0;}
unsigned long configTime(long,int,const char*){return 0;}
esp_err_t i2s_driver_install(i2s_port_t,const i2s_config_t*,int,void*){return 0;}
esp_err_t i2s_driver_uninstall(i2s_port_t){return 0;}
esp_err_t i2s_set_adc_mode(int,int){return 0;}
esp_err_t i2s_adc_enable(i2s_port_t){return 0;}
esp_err_t i2s_adc_disable(i2s_port_t){return 0;}
esp_err_t i2s_read(i2s_port_t,void*,size_t n,size_t* got,TickType_t){ if(got)*got=n; return 0; }
int adc1_config_channel_atten(adc1_channel_t,int){return 0;}
int   MLX90640_DumpEE(uint8_t, uint16_t*){return 0;}
int   MLX90640_ExtractParameters(uint16_t*, paramsMLX90640*){return 0;}
int   MLX90640_GetFrameData(uint8_t, uint16_t*){return 0;}
float MLX90640_GetTa(uint16_t*, const paramsMLX90640*){return 25.0f;}
void  MLX90640_CalculateTo(uint16_t*, const paramsMLX90640*, float, float, float*){}
int   MLX90640_SetRefreshRate(uint8_t, uint8_t){return 0;}
int   MLX90640_SetResolution(uint8_t, uint8_t){return 0;}
int   MLX90640_SetChessMode(uint8_t){return 0;}
esp_adc_cal_value_t esp_adc_cal_characterize(int,int,int,uint32_t v,esp_adc_cal_characteristics_t* c){ if(c){c->vref=v;} return ESP_ADC_CAL_VAL_EFUSE_TP; }
uint32_t esp_adc_cal_raw_to_voltage(uint32_t raw,const esp_adc_cal_characteristics_t*){ return raw*3300/4095; }
int main(){ return 0; }
