#include "Config.h"
#include "Router.h"
#include "SerialInterface.h"

#define START_SERIAL_MESSAGE_HEADER 2
#define START_SERIAL_MESSAGE_DATA 2
#define SERIAL_MESSAGE_SEPARATOR 2
#define END_SERIAL_MESSAGE 3

void
SerialInterface::init(Config *c, Router *r) {
  config = c;
  router = r;

  slip->begin(115200);
}

void
SerialInterface::update() {
  readMessages();
}

void
SerialInterface::readMessages() {
  if (slip->available() > 0) {
    int size;

    while (!slip->endofPacket()) {
      if ((size = slip->available()) > 0) {
        while (size--) {
          inputOSCMessage.fill(slip->read());
        }
      }
    }

    if (!inputOSCMessage.hasError()) {
      router->routeSerialMessage(inputOSCMessage);        
    }

    inputOSCMessage.empty();
  }
}

bool
SerialInterface::sendMessage(OSCMessage& msg) {
  slip->beginPacket();
  msg.send(*slip);
  slip->endPacket();
}
