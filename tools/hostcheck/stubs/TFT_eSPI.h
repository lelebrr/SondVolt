#ifndef STUB_TFT_ESPI_H
#define STUB_TFT_ESPI_H
#include <Arduino.h>
#define TL_DATUM 0
#define TC_DATUM 1
#define MC_DATUM 4
#define TFT_BLACK 0x0000
#define TFT_WHITE 0xFFFF
#define TFT_RED 0xF800
#define TFT_GREEN 0x07E0
#define TFT_BLUE 0x001F
#define TFT_YELLOW 0xFFE0
#define TFT_CYAN 0x07FF
#define TFT_ORANGE 0xFD20
#define TFT_DARKGREY 0x7BEF
class TFT_eSPI {
public:
  TFT_eSPI(int w=320,int h=240){(void)w;(void)h;}
  void init(){} void begin(){}
  void setRotation(uint8_t){}
  void invertDisplay(bool){}
  void setSwapBytes(bool){}
  void fillScreen(uint16_t){}
  void drawPixel(int32_t,int32_t,uint16_t){}
  void drawLine(int32_t,int32_t,int32_t,int32_t,uint16_t){}
  void drawFastHLine(int32_t,int32_t,int32_t,uint16_t){}
  void drawFastVLine(int32_t,int32_t,int32_t,uint16_t){}
  void fillRect(int32_t,int32_t,int32_t,int32_t,uint16_t){}
  void drawRect(int32_t,int32_t,int32_t,int32_t,uint16_t){}
  void fillRoundRect(int32_t,int32_t,int32_t,int32_t,int32_t,uint16_t){}
  void drawRoundRect(int32_t,int32_t,int32_t,int32_t,int32_t,uint16_t){}
  void fillCircle(int32_t,int32_t,int32_t,uint16_t){}
  void drawCircle(int32_t,int32_t,int32_t,uint16_t){}
  void fillTriangle(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t,uint16_t){}
  void drawTriangle(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t,uint16_t){}
  void drawBitmap(int32_t,int32_t,const uint8_t*,int32_t,int32_t,uint16_t){}
  void pushImage(int32_t,int32_t,int32_t,int32_t,const uint16_t*){}
  void setTextColor(uint16_t,uint16_t bg=0){(void)bg;}
  void setTextSize(uint8_t){}
  void setTextDatum(uint8_t){}
  void setCursor(int32_t,int32_t){}
  int16_t drawString(const char*,int32_t,int32_t){return 0;}
  int16_t drawCentreString(const char*,int32_t,int32_t,uint8_t){return 0;}
  void print(const char*){} void print(int){} void print(float){}
  void println(const char*){}
  int printf(const char*,...){return 0;}
  int16_t textWidth(const char*){return 0;}
  int32_t width(){return 320;} int32_t height(){return 240;}
  uint16_t color565(uint8_t,uint8_t,uint8_t){return 0;}
};
class TFT_eSprite : public TFT_eSPI {
public:
  TFT_eSprite(TFT_eSPI* p){(void)p;}
  void setColorDepth(uint8_t){}
  void* createSprite(int16_t,int16_t){ return (void*)1; }
  void deleteSprite(){}
  void fillSprite(uint16_t){}
  void pushSprite(int32_t,int32_t){}
};
#endif
