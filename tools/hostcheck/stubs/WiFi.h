#ifndef STUB_WIFI_H
#define STUB_WIFI_H
#include <Arduino.h>
#define WL_CONNECTED 3
enum { WIFI_OFF=0, WIFI_STA=1, WIFI_AP=2 };
class IPAddr { public: String toString() const { return String("0.0.0.0"); } };
class WiFiClientStub { public: size_t write(const uint8_t*,size_t n){return n;} };
class WiFiClass {
public:
  void persistent(bool){}
  void setHostname(const char*){}
  void mode(int){}
  void begin(const char*,const char*){}
  int  status(){return 0;}
  int  RSSI(){return -60;}
  IPAddr localIP(){return IPAddr();}
  IPAddr softAPIP(){return IPAddr();}
  bool softAP(const char*,const char*){return true;}
  void disconnect(bool){}
  void reconnect(){}
};
extern WiFiClass WiFi;
#endif
