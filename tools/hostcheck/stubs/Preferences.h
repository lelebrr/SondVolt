#ifndef STUB_PREFS_H
#define STUB_PREFS_H
#include <Arduino.h>
class Preferences {
public:
  bool begin(const char*,bool ro=false){(void)ro;return true;}
  void end(){}
  size_t putBytes(const char*,const void*,size_t n){return n;}
  size_t getBytes(const char*,void*,size_t n){return n;}
  size_t putUInt(const char*,uint32_t){return 4;}
  uint32_t getUInt(const char*,uint32_t d=0){return d;}
  size_t putFloat(const char*,float){return 4;}
  float getFloat(const char*,float d=0){return d;}
  bool clear(){return true;}
  bool remove(const char*){return true;}
  bool isKey(const char*){return false;}
};
#endif
