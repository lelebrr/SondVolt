#ifndef STUB_SDFAT_H
#define STUB_SDFAT_H
#include <Arduino.h>
#include <SPI.h>
#define O_RDONLY 0x01
#define O_RDWR   0x03
#define O_APPEND 0x04
#define O_CREAT  0x10
#define FILE_READ  O_RDONLY
#define FILE_WRITE (O_RDWR|O_CREAT)
#define SHARED_SPI 1
#define SD_SCK_MHZ(x) ((uint32_t)(x)*1000000UL)
class SdSpiConfig {
public:
  SdSpiConfig(uint8_t cs,uint8_t opt,uint32_t mhz,SPIClass* p=nullptr){(void)cs;(void)opt;(void)mhz;(void)p;}
};
class FsFile {
public:
  operator bool() const { return false; }
  bool available(){return false;}
  String readStringUntil(char){return String("");}
  int read(){return -1;}
  size_t print(const char*){return 0;}
  size_t println(const char*){return 0;}
  size_t println(){return 0;}
  void close(){} void flush(){}
  uint32_t size(){return 0;}
  bool seek(uint32_t){return true;}
  bool isDirectory(){return false;}
  bool openNext(FsFile*,uint8_t){return false;}
  void getName(char* b,size_t n){ if(n) b[0]=0; }
  uint32_t position(){return 0;}
  bool truncate(uint32_t){return true;}
  size_t write(const uint8_t*,size_t n){return n;}
};
class SdFat {
public:
  bool begin(SdSpiConfig){return false;}
  bool begin(uint8_t cs=0,uint32_t s=0){(void)cs;(void)s;return false;}
  FsFile open(const char*,uint8_t mode=O_RDONLY){(void)mode;return FsFile();}
  bool exists(const char*){return false;}
  bool remove(const char*){return true;}
  bool mkdir(const char*){return true;}
  bool rename(const char*,const char*){return true;}
};
#endif
