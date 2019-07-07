#include "Config.h"
#include "Button.h"
#include "Sensors.h"
#include "Vibrator.h"
#include "Neopix.h"
#include "SerialInterface.h"
#include "WiFiInterface.h"
#include "Router.h"

void
Router::init(Config *c, Button *b, Neopix *n, Sensors *s, Vibrator *v, SerialInterface *si, WiFiInterface *wi) {

  // Serial.println("about to initialize everything");

  config = c;
  config->init();
  config->load(); // read from file if exists, otherwise use default values

  // Serial.println("initialized config");

  // initOSCAddresses(config->getMovuinoId());
  initOscAddresses();

  for (unsigned int i = 0; i < nbOscAddresses; ++i) {
    oscMessages[i] = new OSCMessage(oscAddresses[i]); // we never delete but it's ok
  }

  // Serial.println("initialized osc addresses");

  button = b;
  button->init(config, this);

  // Serial.println("initialized button");

  neopix = n;
  neopix->init(config, this);

  // Serial.println("initialized neopix");

  sensors = s;
  sensors->init(config, this);

  // Serial.println("initialized sensors");

  //*
  vibrator = v;
  vibrator->init(config, this);
  //*/

  // Serial.println("initialized vibrator");

  //*
  serial = si;
  serial->init(config, this);
  //*/

  // Serial.println("initialized serial");

  wifi = wi;
  wifi->init(config, this);

  // Serial.println("initialization done");
}

void
Router::update() {
  serial->update();
  wifi->update();
  vibrator->update();
  neopix->update();
  button->update();
  sensors->update();
}

void
Router::routeWiFiMessage(OSCMessage& msg) {
  char address[MAX_OSC_ADDRESS_LENGTH];
  msg.getAddress(address);

  // route only incoming control messages
  if (strcmp(address, oscAddresses[oscVibroNow]) == 0 ||
      strcmp(address, oscAddresses[oscVibroPulse]) == 0||
      strcmp(address, oscAddresses[oscNeopix]) == 0) {
    routeOSCMessage(msg);
  }
}

void
Router::wiFiMessageErrorCallback(OSCMessage& msg) {
  // TODO
}

void
Router::routeSerialMessage(OSCMessage& msg) {
  char address[MAX_OSC_ADDRESS_LENGTH];
  msg.getAddress(address);

  // route incoming vibrator control messages only if useSerial is on
  if (!config->getUseSerial()) {
    if (strcmp(address, oscAddresses[oscVibroNow]) != 0 &&
        strcmp(address, oscAddresses[oscVibroPulse]) != 0) {
      routeOSCMessage(msg);
    }
  } else {
    routeOSCMessage(msg);
  }
}

void
Router::serialMessageErrorCallback(OSCMessage& msg) {
  // TODO
}

//--------------------------------- EVENTS -----------------------------------//

void
Router::onWiFiConnectionEvent(WiFiConnectionState s) {
  sendWiFiConnectionMessage(getConnectionStateOSCValue(s));
}

void
Router::onButtonEvent(ButtonState s) {
  // if (!config->getSendSingleFrame()) {
  //   sendButtonMessage(getButtonIntValue(s));
  // }

  // sendButtonMessage(getButtonIntValue(s)); // send button anyways
}

void
Router::onNewSensorValues(float *f) {
  // if (config->getSendSingleFrame()) {
    sendSingleFrame(f);
  // } else {
  //   sendSensorsMessage(f);
  // }
}

//=============================== PRIVATE ====================================//

int
Router::getButtonIntValue(ButtonState s) {
  int val = 0;

  if (s == ButtonPressed) { // send 2
    val = 1;
  } else if (s == ButtonReleased) {
    val = 0;
  } else if (s == ButtonHolding) {
    val = 2;
  }

  return val;
}

void
Router::routeOSCMessage(OSCMessage& msg) {
  char address[MAX_OSC_ADDRESS_LENGTH];
  char arg[MAX_OSC_STRING_ARG_LENGTH];

  int msgLength = msg.size();
  msg.getAddress(address);

  //----------------------------------------------------------------------------
  if (strcmp(address, oscAddresses[oscHello]) == 0 && msgLength == 0) {
    // respond with "movuino", id, firmware version, wifistate
    sayHello();
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscSetId]) == 0 && msgLength > 0) {
    msg.getString(0, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
    config->setUserId((const char *)arg);
    config->store(); 
    sendUserId(oscSet);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscGetId]) == 0) {
    sendUserId(oscGet);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscSerialEnable]) == 0 && msgLength > 0) {
    config->setUseSerial(msg.getInt(0) > 0);
    config->store();
    sendSerialMessage(msg);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscMagnetoEnable]) == 0 && msgLength > 0) {
    config->setReadMag(msg.getInt(0) > 0);
    config->store();
    sendSerialMessage(msg);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscWifiEnable]) == 0 && msgLength > 0) {
    config->setUseWiFi(msg.getInt(0) > 0);
    config->store();
    sendSerialMessage(msg);
    wifi->stopWiFi();
    wifi->startWiFi();// will check if useWifi is set
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscSetWifi]) == 0 && msgLength > 1) {
    // we can have :
    // <ssid> <hostIP> if no password
    // <ssid> <password> <hostIP>
    msg.getString(0, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
    config->setSsid((const char *)arg);

    if (msgLength == 2) { // no password
      config->setPassword("");
      msg.getString(1, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
      config->setHostIP((const char *)arg);
    } else { // msgLength >= 3, we have a password
      msg.getString(1, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
      config->setPassword((const char *)arg);
      msg.getString(2, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
      config->setHostIP((const char *)arg);
    }

    config->store();
    sendWiFiSettings(oscSet);
    wifi->stopWiFi();
    wifi->startWiFi(); // will check if useWifi is set
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscGetWifi]) == 0) {
    sendWiFiSettings(oscGet);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscSetPorts]) == 0 && msgLength > 1) {
    int in = msg.getInt(0);
    int out = msg.getInt(1);

    in = in < 0 ? 0 : in;
    out = out < 0 ? 0 : out;

    config->setInputPort(in);
    config->setOutputPort(out);

    config->store();
    sendPorts(oscSet);
    wifi->stopWiFi();
    wifi->startWiFi(); // will check if useWifi is set
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscGetPorts]) == 0) {
    sendPorts(oscGet);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscSetRange]) == 0 && msgLength > 1) {
    sensors->setAccelRange(msg.getInt(0));
    sensors->setGyroRange(msg.getInt(1));
    config->setAccelRange(msg.getInt(0));
    config->setGyroRange(msg.getInt(1));
    config->store();
    sendAccelGyroRanges(oscSet);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscGetRange]) == 0) {
    sendAccelGyroRanges(oscGet);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscSetFrameperiod]) == 0 && msgLength > 0) {
    sensors->setOutputFramePeriod(msg.getInt(0));
    config->setOutputFramePeriod(msg.getInt(0));
    config->store();
    sendOutputFramePeriod(oscSet);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscGetFrameperiod]) == 0) {
    sendOutputFramePeriod(oscGet);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscVibroPulse]) == 0 && msgLength > 2) {
    vibrator->pulse(
      msg.getInt(0),
      msg.getInt(1),
      msg.getInt(2)
    );
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscVibroNow]) == 0 && msgLength > 0) {
    vibrator->vibrate(msg.getInt(0) > 0);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscNeopix]) == 0 && msgLength > 2) {
    neopix->setColor(
      (unsigned int) msg.getInt(0),
      (unsigned int) msg.getInt(1),
      (unsigned int) msg.getInt(2)
    );
  //----------------------------------------------------------------------------
  }
}

void
Router::sayHello() {
  OSCMessage *msg = oscMessages[oscHello];
  msg->empty();
  msg->add("movuino");
  msg->add(config->getUserId());
  msg->add(getConnectionStateOSCValue(wifi->getConnectionState()));
  msg->add(wifi->getStringIPAddress().c_str());
  msg->add(config->getMovuinoId());
  msg->add(config->getFirmwareVersion());
  sendSerialMessage(*msg);
}

void
Router::sendUserId(oscGetSet getSet) {
  OSCMessage *msg = getSet == oscGet 
                  ? oscMessages[oscGetId]
                  : oscMessages[oscSetId];
  msg->empty();
  msg->add(config->getUserId());
  sendSerialMessage(*msg);
}

void
Router::sendWiFiConnectionMessage(int i) {
  OSCMessage *msg = oscMessages[oscWifiState];
  msg->empty();
  msg->add(i);
  msg->add(wifi->getStringIPAddress().c_str());
  sendSerialMessage(*msg);
}

void
Router::sendWiFiSettings(oscGetSet getSet) {
  OSCMessage *msg = getSet == oscGet
                  ? oscMessages[oscGetWifi]
                  : oscMessages[oscSetWifi];
  msg->empty();
  msg->add(config->getSsid());
  msg->add(config->getPassword());
  msg->add(config->getHostIP());
  sendSerialMessage(*msg);
}

void
Router::sendPorts(oscGetSet getSet) {
  OSCMessage *msg = getSet == oscGet
                  ? oscMessages[oscGetPorts]
                  : oscMessages[oscSetPorts];
  msg->empty();  
  msg->add(config->getInputPort());
  msg->add(config->getOutputPort());
  sendSerialMessage(*msg);
}

void
Router::sendAccelGyroRanges(oscGetSet getSet) {
  OSCMessage *msg = getSet == oscGet
                  ? oscMessages[oscGetRange]
                  : oscMessages[oscSetRange];
  msg->empty();  
  msg->add(config->getAccelRange());
  msg->add(config->getGyroRange());
  sendSerialMessage(*msg);
}

void
Router::sendOutputFramePeriod(oscGetSet getSet) {
  OSCMessage *msg = getSet == oscGet 
                  ? oscMessages[oscGetFrameperiod]
                  : oscMessages[oscSetFrameperiod];
  msg->empty();
  msg->add(config->getOutputFramePeriod());
  sendSerialMessage(*msg);
}

//----------------------------------------------------------------------------//

void
Router::sendSingleFrame(float *f) {
  OSCMessage *msg = oscMessages[oscFrame];
  msg->empty();

  msg->add(config->getUserId());

  for (unsigned int i = 0; i < 9; i++) {
    msg->add(*(f + i));
  }

  msg->add(getButtonIntValue(button->getState())); // append the button value
  msg->add(vibrator->isVibrating() ? 1 : 0);

  if (config->getUseSerial()) {
    sendSerialMessage(*msg);
  }

  if (config->getUseWiFi()) {
    sendWiFiMessage(*msg);
  }
}

void
Router::sendWiFiMessage(OSCMessage& msg) {
  wifi->sendMessage(msg, config->getHostIP(), config->getOutputPort());
}

void
Router::sendSerialMessage(OSCMessage& msg) {
  serial->sendMessage(msg);
}
