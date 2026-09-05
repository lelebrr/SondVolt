#ifndef STUB_WIRE_H
#define STUB_WIRE_H
#include <Arduino.h>
class TwoWire {
public:
  void begin(int sda=-1,int scl=-1){(void)sda;(void)scl;}
  void setClock(uint32_t){}
  void beginTransmission(uint8_t){}
  uint8_t endTransmission(bool s=true){(void)s;return 1;}
  uint8_t requestFrom(uint8_t,uint8_t){return 0;}
  int read(){return 0;}
  size_t write(uint8_t){return 1;}
};
extern TwoWire Wire;
#endif
