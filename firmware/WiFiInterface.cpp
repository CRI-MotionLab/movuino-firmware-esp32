#include "Router.h"
#include "Config.h"
#include "WiFiInterface.h"

// found here : https://github.com/espressif/arduino-esp32/issues/932
#include "esp_system.h"
void getMacAddress(char *dst) { // dst should be 18 characters long (12 chars + 5 semicolons + '\0')
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  sprintf(
    dst,
    "%02X:%02X:%02X:%02X:%02X:%02X",
    baseMac[0],
    baseMac[1],
    baseMac[2],
    baseMac[3],
    baseMac[4],
    baseMac[5]
  );
}

/******************************************************************************/

void
WiFiInterface::init(Config *c, Router *r) {
  config = c;
  router = r;

  if (config->getUseWiFi()) {
    startWiFi();
  }
}

//============================ TIMER METHODS =================================//

void
WiFiInterface::update() {
  Timer::update();

  // digitalWrite(pinLedWifi, wifiLight ? LOW : HIGH);
  // digitalWrite(pinLedBat, batLight ? LOW : HIGH);

  if (WiFi.status() == WL_CONNECTED) {
    readMessages();
  }
}

void
WiFiInterface::callback() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiLight = !wifiLight;
  } else {
    stop();
  }
}

void
WiFiInterface::stop() {
  Timer::stop();

  if (WiFi.status() == WL_CONNECTED) {
    initialized = true;
    onConnectionEvent(WiFiConnected);
  } else {
    stopWiFi();
    onConnectionEvent(WiFiDisconnected);
  }
}

//============================ WIFIINTERFACE =================================//

void
WiFiInterface::readMessages() {
  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    while (packetSize--) {
      inputOSCMessage.fill(udp.read()); // read incoming message into the bundle
    }
    if (!inputOSCMessage.hasError()) {
      router->routeWiFiMessage(inputOSCMessage);
    } else {
      router->wiFiMessageErrorCallback(inputOSCMessage);
    }

    inputOSCMessage.empty();
  }
}

bool
WiFiInterface::sendMessage(OSCMessage& msg, const char *hostIP, int portOut) {
  if (WiFi.status() == WL_CONNECTED) {
    udp.beginPacket(hostIP, portOut); // send message to computer target with "hostIP" on "port"
    msg.send(udp);
    udp.endPacket();
    return true;
  }

  return false;
}

//============================= WIFI CONNECTION ==============================//

WiFiConnectionState
WiFiInterface::getConnectionState() {
  return connectionState;
}

bool
WiFiInterface::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

//---------------------------------- START -----------------------------------//

void
WiFiInterface::startWiFi() {
  // Serial.println("starting wifi");

  if (WiFi.status() != WL_CONNECTED) {
    wifiLight = batLight = false; // turn lights off if !useWiFi

    if (config->getUseWiFi()) {
      // if (!initialized) {
        WiFi.mode(WIFI_STA);
        initialized = true;
      // } else {
      //   WiFi.forceSleepWake();
      // }

      udp.begin(config->getInputPort()); // start listening to udp messages
      WiFi.begin(config->getSsid(), config->getPassword());
      start(connectionTimeout); // inherited from Timer

      // Serial.println("started every wifi stuff");

      onConnectionEvent(WiFiConnecting);
      wifiLight = batLight = true; // start blinking;
    }
  }
}

//-------------------------------- SHUTDOWN ----------------------------------//

void
WiFiInterface::stopWiFi() {
  if (running) {
    running = false;
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    // WiFi.forceSleepBegin();
    udp.stop();
    delay(1);
  }

  wifiLight = batLight = false;
}

//---------------------------------- AWAKE -----------------------------------//

void
WiFiInterface::toggleWiFiState() {
  if (WiFi.status() == WL_CONNECTED) {
    stopWiFi();
  } else {
    startWiFi();
  }
}

void
WiFiInterface::onConnectionEvent(WiFiConnectionState s) {
  if (s == WiFiConnected) {
    wifiLight = true;
    // digitalWrite(pinLedWifi, LOW); // turn ON wifi led
  }

  connectionState = s;
  router->onWiFiConnectionEvent(s);
}

//=============================== UTILITIES ==================================//

String
WiFiInterface::getStringMacAddress() {
  char uuid[18];
  getMacAddress(uuid);

  return String(uuid);
}

void
WiFiInterface::getIPAddress(int *res) { // must be of type int[4]
  // sometimes we get weird ip address values, if it happens again check here :
  if (isConnected()) {
    IPAddress ip = WiFi.localIP();
    for (int i = 0; i < 4; i++) {
      *(res + i) = ip[i];
    }
  }
}

String
WiFiInterface::getStringIPAddress() {
  int ip[4] = { 0, 0, 0, 0 };
  getIPAddress(&ip[0]);
  String sip = String(ip[0]);
  sip += ".";
  sip += ip[1];
  sip += ".";
  sip += ip[2];
  sip += ".";
  sip += ip[3];
  return sip;
}

