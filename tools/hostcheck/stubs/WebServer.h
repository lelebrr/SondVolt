#ifndef STUB_WEBSERVER_H
#define STUB_WEBSERVER_H
#include <Arduino.h>
#include <WiFi.h>
class WebServer {
public:
  WebServer(int port=80){(void)port;}
  void on(const char*, void(*)()){}
  void onNotFound(void(*)()){}
  void begin(){}
  void stop(){}
  void handleClient(){}
  void send(int,const char*,const char*){}
  void send_P(int,const char*,const char*){}
  void sendHeader(const char*,const char*){}
  void setContentLength(size_t){}
  WiFiClientStub client(){return WiFiClientStub();}
};
#endif
