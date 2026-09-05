#ifndef STUB_DALLAS_H
#define STUB_DALLAS_H
#include <OneWire.h>
#define DEVICE_DISCONNECTED_C -127.0f
class DallasTemperature {
public:
  DallasTemperature(OneWire* w){(void)w;}
  void begin(){}
  void setResolution(uint8_t){}
  void setWaitForConversion(bool){}
  void requestTemperatures(){}
  float getTempCByIndex(uint8_t){return 25.0f;}
  uint8_t getDeviceCount(){return 0;}
};
#endif
