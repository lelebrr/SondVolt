#ifndef STUB_SPI_H
#define STUB_SPI_H
#include <Arduino.h>
#define VSPI 3
#define HSPI 2
class SPIClass {
public:
  SPIClass(int bus=3){(void)bus;}
  void begin(int sck=-1,int miso=-1,int mosi=-1,int ss=-1){(void)sck;(void)miso;(void)mosi;(void)ss;}
  void end(){}
};
extern SPIClass SPI;
#endif
