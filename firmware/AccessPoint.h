#ifndef _MOVUINO_FIRMWARE_ACCESS_POINT_H_
#define _MOVUINO_FIRMWARE_ACCESS_POINT_H_

class Config;
// class ESP8266WebServer;
class WebServer;
class WebSocketsServer;

class AccessPoint {
private:
  bool initialized;

  static int msgLength;
  static String msg[MAX_TOTAL_CONFIG_LENGTH];
  static Config *config;
  
public:
  // static ESP8266WebServer *webServer;
  static WebServer *webServer;
  static WebSocketsServer *socketServer;

  AccessPoint() : initialized(false) {}

  ~AccessPoint() {}

  void init(Config *c);
  void update();

  static void encodeAndSendSettings(uint8_t num, bool broadcast = false);
  static void parseInputMessage(uint8_t *payload, size_t length);
  static void processInputMessage();
};

#endif /* _MOVUINO_FIRMWARE_ACCESS_POINT_H_ */
