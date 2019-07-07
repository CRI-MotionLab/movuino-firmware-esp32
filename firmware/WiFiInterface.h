#ifndef _MOVUINO_FIRMWARE_WIFIINTERFACE_H_
#define _MOVUINO_FIRMWARE_WIFIINTERFACE_H_

// #define MAX_OSC_ADDRESS_SIZE 120
// #define MAX_OSC_ADDRESSES 100

#include <Arduino.h>
// #include <ESP8266WiFi.h>
#include <WiFi.h>

// maybe someday we could allow to manage a list of networks via OSC messages
// or from config page in AP mode. when it happens, use addAP method from :
// #include <ESP8266WiFiMulti.h>

// for TCP OSC :
// #include <WebSocketsServer.h>

#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "Timer.h"
#include "globals.h"

class Config;
class Router;

//----------------------------------------------------------------------------//

class WiFiInterface : public Timer {
private:
  // timer specific vars
  bool wifiLight;
  bool batLight;
  WiFiConnectionState connectionState;
  unsigned long connectionTimeout;

  bool initialized;
  WiFiUDP udp;

  Config *config;
  Router *router;

  OSCMessage inputOSCMessage;
  
public:
  WiFiInterface(unsigned long blinkPeriod = LOW_BLINK_PERIOD,
                unsigned long timeout = WIFI_CONNECTION_TIMEOUT) :
  Timer(blinkPeriod),
  wifiLight(false),
  batLight(false),
  connectionState(WiFiDisconnected),
  connectionTimeout(timeout),
  initialized(false) {}

  virtual ~WiFiInterface() {}

  // override parent methods :
  void stop(); // called back by update when timeout is over
  void callback();

  void init(Config *c, Router *r); // supposed to be called once at start time
  void update();

  void readMessages(/*Router *router*/);
  bool sendMessage(OSCMessage &msg, const char *hostIP, int portOut);

  String getStringMacAddress();
  void getIPAddress(int *res); // res must be of type int[4]
  String getStringIPAddress();

  WiFiConnectionState getConnectionState();
  bool isConnected();
  void startWiFi();
  void stopWiFi();
  void toggleWiFiState();

private:
  void onConnectionEvent(WiFiConnectionState s);
};

#endif /* _MOVUINO_FIRMWARE_WIFIINTERFACE_H_ */
