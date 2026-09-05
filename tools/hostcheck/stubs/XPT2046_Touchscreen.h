#ifndef STUB_XPT_H
#define STUB_XPT_H
#include <Arduino.h>
#include <SPI.h>
class TS_Point {
public:
  int16_t x=0,y=0,z=0;
  TS_Point(){} TS_Point(int16_t a,int16_t b,int16_t c):x(a),y(b),z(c){}
};
class XPT2046_Touchscreen {
public:
  XPT2046_Touchscreen(uint8_t cs,uint8_t irq=255){(void)cs;(void)irq;}
  bool begin(){return true;}
  bool begin(SPIClass&){return true;}
  bool touched(){return false;}
  TS_Point getPoint(){return TS_Point();}
  void setRotation(uint8_t){}
};
#endif
