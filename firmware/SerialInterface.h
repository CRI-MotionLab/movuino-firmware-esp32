#ifndef _MOVUINO_FIRMWARE_SERIALINTERFACE_H_
#define _MOVUINO_FIRMWARE_SERIALINTERFACE_H_

#define PAULSSUGGESTION
#define MAX_SERIAL_COMMAND_LENGTH 20

#include <Arduino.h>
#include <SLIPEncodedSerial.h>
#include <SLIPEncodedUSBSerial.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

class Router;

class SerialInterface {
private:
  SLIPEncodedSerial *slip;

  Config *config;
  Router *router;

  OSCMessage inputOSCMessage;

public:
  SerialInterface() {
    slip = new SLIPEncodedSerial(Serial);
  }

  ~SerialInterface() {
    delete slip;
  }

  void init(Config *c, Router *r);
  void update();

  void readMessages();
  bool sendMessage(OSCMessage& msg);
};

#endif /* _MOVUINO_FIRMWARE_SERIALINTERFACE_H_ */
