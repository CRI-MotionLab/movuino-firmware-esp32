#ifndef _MOVUINO_FIRMWARE_ROUTER_H_
#define _MOVUINO_FIRMWARE_ROUTER_H_

#include <OSCMessage.h>
#include "globals.h"

class Config;
class Button;
class Sensors;
class Vibrator;
class Neopix;
class SerialInterface;
class WiFiInterface;

class Router {
private:
  Config *config;
  Button *button;
  Sensors *sensors;
  Vibrator *vibrator;
  Neopix *neopix;
  SerialInterface *serial;
  WiFiInterface *wifi;

  OSCMessage *oscMessages[nbOscAddresses]; // nb of output OSC messages

public:
  Router() {}
  ~Router() {}

  void init(Config *c, Button *b, Neopix *n, Sensors *s, Vibrator *v, SerialInterface *si, WiFiInterface *wi);
  void update();

  // CALLBACKS :
  void routeWiFiMessage(OSCMessage& msg);
  void wiFiMessageErrorCallback(OSCMessage& msg);
  void routeSerialMessage(OSCMessage& msg);
  void serialMessageErrorCallback(OSCMessage& msg);
  // called by wifi
  void onWiFiConnectionEvent(WiFiConnectionState s);
  // called by button
  void onButtonEvent(ButtonState s);
  // called by sensors
  void onNewSensorValues(float *f);

private:
  int getButtonIntValue(ButtonState s);
  void routeOSCMessage(OSCMessage& msg);

  void sayHello();
  void sendUserId(oscGetSet getSet);
  void sendWiFiConnectionMessage(int i);
  void sendWiFiSettings(oscGetSet getSet);
  void sendPorts(oscGetSet getSet);
  void sendAccelGyroRanges(oscGetSet getSet);
  void sendOutputFramePeriod(oscGetSet getSet);

  void sendSingleFrame(float *f);

  void sendWiFiMessage(OSCMessage& msg);
  void sendSerialMessage(OSCMessage& msg);
};

#endif /* _MOVUINO_FIRMWARE_ROUTER_H_ */
