// Stub minimo do Arduino/ESP32 para verificacao no host. NAO usar na placa.
#ifndef STUB_ARDUINO_H
#define STUB_ARDUINO_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#define PROGMEM
#define F(x) (x)
#define pgm_read_byte(a) (*(const uint8_t*)(a))
#define pgm_read_word(a) (*(const uint16_t*)(a))
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
typedef bool boolean;
typedef uint8_t byte;
class String {
public:
  std::string s;
  String() {}
  String(const char* c) : s(c?c:"") {}
  String(int v){ char b[32]; snprintf(b,32,"%d",v); s=b; }
  String(float v){ char b[32]; snprintf(b,32,"%f",v); s=b; }
  size_t length() const { return s.size(); }
  const char* c_str() const { return s.c_str(); }
  int indexOf(char c, int f=0) const { size_t p=s.find(c,f); return p==std::string::npos?-1:(int)p; }
  int indexOf(const char* c, int f=0) const { size_t p=s.find(c,f); return p==std::string::npos?-1:(int)p; }
  String substring(int a) const { return String(s.substr(a).c_str()); }
  String substring(int a,int b) const { return String(s.substr(a,b-a).c_str()); }
  float toFloat() const { return atof(s.c_str()); }
  int toInt() const { return atoi(s.c_str()); }
  void trim() {} void toUpperCase() {}
  String& operator+=(const String& o){ s+=o.s; return *this; }
  char operator[](int i) const { return s[i]; }
  bool operator==(const char* c) const { return s==c; }
  bool startsWith(const char* c) const { return s.rfind(c,0)==0; }
};
inline String operator+(const String&a,const String&b){ String r=a; r+=b; return r; }
class HardwareSerial {
public:
  void begin(unsigned long){}
  void println(const char* s=""){ (void)s; }
  void println(int){} void println(float){}
  void print(const char*){} void print(int){} void print(float){}
  int printf(const char* f,...){ (void)f; return 0; }
  void flush(){}
  operator bool() const { return true; }
};
extern HardwareSerial Serial;
void pinMode(int,int);
void digitalWrite(int,int);
int  digitalRead(int);
int  analogRead(int);
void analogReadResolution(int);
void analogSetPinAttenuation(int,int);
unsigned long millis();
unsigned long micros();
void delay(unsigned long);
void delayMicroseconds(unsigned long);
long map(long,long,long,long,long);
void ledcAttach(int,int,int);
void ledcWrite(int,int);
void ledcSetup(uint8_t,uint32_t,uint8_t);
void ledcAttachPin(uint8_t,uint8_t);
void ledcWriteTone(uint8_t,uint32_t);
uint32_t getCpuFrequencyMhz();
uint32_t temperatureRead();
unsigned long configTime(long,int,const char*);
template<class T,class U,class V> auto constrain(T v,U a,V b) -> T { return v<(T)a?(T)a:(v>(T)b?(T)b:v); }
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#define PI 3.14159265358979f
typedef void* TaskHandle_t;
typedef void* QueueHandle_t;
typedef void* SemaphoreHandle_t;
typedef uint32_t TickType_t;
typedef int BaseType_t;
typedef unsigned int UBaseType_t;
#define pdMS_TO_TICKS(x) ((TickType_t)(x))
#define pdTRUE 1
#define pdPASS 1
#define pdFAIL 0
#define pdFALSE 0
#define portMAX_DELAY 0xFFFFFFFFUL
#define tskNO_AFFINITY 0x7FFFFFFF
BaseType_t xTaskCreate(void(*)(void*),const char*,uint32_t,void*,UBaseType_t,TaskHandle_t*);
BaseType_t xTaskCreatePinnedToCore(void(*)(void*),const char*,uint32_t,void*,UBaseType_t,TaskHandle_t*,int);
void vTaskDelay(TickType_t);
inline void taskYIELD(){}
void vTaskDelete(TaskHandle_t);
uint32_t uxTaskGetStackHighWaterMark(TaskHandle_t);
SemaphoreHandle_t xSemaphoreCreateRecursiveMutex();
SemaphoreHandle_t xSemaphoreCreateMutex();
BaseType_t xSemaphoreTakeRecursive(SemaphoreHandle_t,TickType_t);
BaseType_t xSemaphoreGiveRecursive(SemaphoreHandle_t);
BaseType_t xSemaphoreTake(SemaphoreHandle_t,TickType_t);
BaseType_t xSemaphoreGive(SemaphoreHandle_t);
void esp_task_wdt_reset();
int  esp_task_wdt_add(TaskHandle_t);
int  esp_task_wdt_init(uint32_t,bool);
class EspClass {
public:
  uint32_t getFreeHeap(){return 200000;}
  uint32_t getMinFreeHeap(){return 180000;}
  uint32_t getHeapSize(){return 327680;}
  uint32_t getCpuFreqMHz(){return 240;}
  uint32_t getFlashChipSize(){return 4194304;}
  const char* getSdkVersion(){return "stub";}
  uint8_t getChipRevision(){return 3;}
  const char* getChipModel(){return "ESP32";}
  uint32_t getPsramSize(){return 0;}
  void restart(){}
};
extern EspClass ESP;
#endif
