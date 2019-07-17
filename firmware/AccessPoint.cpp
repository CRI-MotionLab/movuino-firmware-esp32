#include <Arduino.h>
#include <FS.h>

// #ifdef ESP32
#include <SPIFFS.h>
// #endif

// #include <ESP8266WiFi.h>
#include <WiFi.h>
// #include <ESP8266WebServer.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
// Don't use, mDNS not supported by Android. That's an issue.
// see : https://tttapa.github.io/ESP8266/Chap08%20-%20mDNS.html
// #include <ESP8266mDNS.h>

#include "Config.h"
#include "AccessPoint.h"
#include "configWebPage.h"

#include "globals.h" // use MAX_CONFIG_STRING_SIZE to format and parse strings

// these are static :
int AccessPoint::msgLength;
String AccessPoint::msg[MAX_TOTAL_CONFIG_LENGTH];
Config *AccessPoint::config = NULL;
// ESP8266WebServer *AccessPoint::webServer = NULL;
WebServer *AccessPoint::webServer = NULL;
WebSocketsServer *AccessPoint::socketServer = NULL;

//======================= websocket server callback ==========================//

static void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: {
        // notify disconnection (with blink) ?
      }
      break;
    case WStype_CONNECTED: {
        // IPAddress ip = AccessPoint::socketServer->remoteIP(num);
        AccessPoint::encodeAndSendSettings(num);
      }
      break;
    case WStype_TEXT: {
        AccessPoint::parseInputMessage(payload, length);
      }
      break;
  }
}

//=========================== AccessPoint methods ============================//

void
AccessPoint::init(Config *c) {
  digitalWrite(pinLedWifi, HIGH);

  config = c;
  config->init();
  config->load(); // read from file if exists, otherwise use default values

  char apssid[30];
  strcpy(apssid, "movuino-");
  strcat(apssid, config->getMovuinoId());

  // Don't use WIFI_AP_STA mode !!!!!
  // (or be warned it also tries to connect to the last network the ESP
  // remembers and prevents websockets to work if it fails)
  // see : https://github.com/Links2004/arduinoWebSockets/issues/274

  WiFi.mode(WIFI_AP);
  IPAddress ip(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig(ip, gateway, subnet);

  if (WiFi.softAP(apssid)) {
    initialized = true;
  }

  // servers

  // webServer = new ESP8266WebServer(80);
  webServer = new WebServer(80);
  socketServer = new WebSocketsServer(81);

  socketServer->begin();
  socketServer->onEvent(webSocketEvent);

  // serve main config page

  webServer->on("/", []() {
    webServer->send(200, "text/html", configWebPage);
  });

  // just in case we change the config file structure,
  // here is a way to force it to be cleared :

  webServer->on("/clear", []() {
    config->reset();

    if (SPIFFS.exists("/config.txt")) {
      webServer->send(200, "text/plain", "config file not cleared !");
    } else {
      webServer->send(200, "text/plain", "config file cleared !");
      config->store();
    }
  });

  webServer->begin();
}

void
AccessPoint::update() {
  socketServer->loop();
  webServer->handleClient();

  digitalWrite(pinLedWifi, initialized ? LOW : HIGH);
}

//----------------- executed by websocket server callback --------------------//

void
AccessPoint::encodeAndSendSettings(uint8_t num, bool broadcast) {
  char packet[MAX_TOTAL_CONFIG_STRING_SIZE];
  String intStr;

  strcpy(packet, "settings\n");

  strcat(packet, config->getUserId());
  strcat(packet, "\n");

  intStr = String(config->getUseWiFi());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");

  strcat(packet, config->getSsid());
  strcat(packet, "\n");
  strcat(packet, config->getPassword());
  strcat(packet, "\n");
  strcat(packet, config->getHostIP());
  strcat(packet, "\n");

  intStr = String(config->getInputPort());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  intStr = String(config->getOutputPort());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");

  intStr = String(config->getAccelRange());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  intStr = String(config->getGyroRange());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");

  intStr = String(config->getUseSerial());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  
  intStr = String(config->getReadMag());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  intStr = String(config->getOutputFramePeriod());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  // intStr = String(config->getButtonHoldDuration());
  // strcat(packet, intStr.c_str());
  // strcat(packet, "\n");

  strcat(packet, config->getMovuinoId());
  strcat(packet, "\n");
  String version("v");
  version += MOVUINO_FIRMWARE_VERSION_MAJOR;
  version += ".";
  version += MOVUINO_FIRMWARE_VERSION_MINOR;
  version += ".";
  version += MOVUINO_FIRMWARE_VERSION_PATCH;
  strcat(packet, version.c_str());
  strcat(packet, "\n");

  if (broadcast) {
    socketServer->broadcastTXT(packet);
  } else {
    socketServer->sendTXT(num, packet);
  }
}

void
AccessPoint::parseInputMessage(uint8_t *payload, size_t length) {
  size_t index = 0;

  msgLength = 0;
  msg[0] = String("");

  for (size_t index = 0; index < length; index++) {
    if (payload[index] != '\n') {
      msg[msgLength] += (char) payload[index];
    } else if (index < length - 1) {
      msgLength++;
      if (msgLength == MAX_TOTAL_CONFIG_LENGTH) break;
      msg[msgLength] = String("");
    }
  }

  msgLength++;
  processInputMessage();
}

void
AccessPoint::processInputMessage() {
  bool light = false;
  for (int i = 0; i < 10; i++) {
    light = !light;
    digitalWrite(pinLedWifi, light ? LOW : HIGH);
    delay(50);
  }

  if (msgLength > 0) {
    if (msg[0] == "clear") {
      config->reset();
      config->store();
      encodeAndSendSettings(0, true); // refresh page
    } else if (msg[0] == "settings" && msgLength >= 13) {
      config->setUserId(msg[1].c_str());
      config->setUseWiFi(msg[2].toInt() > 0);
      config->setSsid(msg[3].c_str());
      config->setPassword(msg[4].c_str());
      config->setHostIP(msg[5].c_str());
      config->setInputPort(msg[6].toInt());
      config->setOutputPort(msg[7].toInt());
      config->setAccelRange(msg[8].toInt());
      config->setGyroRange(msg[9].toInt());
      config->setUseSerial(msg[10].toInt() > 0);
      config->setReadMag(msg[11].toInt() > 0);
      config->setOutputFramePeriod(msg[12].toInt());
      // config->setButtonHoldDuration(msg[13].toInt());
      config->store();
      encodeAndSendSettings(0, true);
    } else {
      // ... something else needed ?
    }
  }
}
