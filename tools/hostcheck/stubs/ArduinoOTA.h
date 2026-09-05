#ifndef STUB_OTA_H
#define STUB_OTA_H
#include <Arduino.h>
typedef int ota_error_t;
class ArduinoOTAClass {
public:
  void setHostname(const char*){}
  void setPassword(const char*){}
  template<class F> void onStart(F){}
  template<class F> void onProgress(F){}
  template<class F> void onEnd(F){}
  template<class F> void onError(F){}
  void begin(){}
  void end(){}
  void handle(){}
};
extern ArduinoOTAClass ArduinoOTA;
#endif
