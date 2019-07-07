#ifndef _MOVUINO_FIRMWARE_CONFIG_H_
#define _MOVUINO_FIRMWARE_CONFIG_H_

#include <Arduino.h>
// #include <EEPROM.h>
#include "globals.h"

class Config {
private:
  // stays false if there is a problem with SPIFFS on init,
  // thus ignoring calls to load / store.
  bool disabled;
  bool configExists;

  char initialized[MAX_CONFIG_STRING_SIZE];
  char movuinoId[MAX_CONFIG_STRING_SIZE];
  char firmwareVersion[MAX_CONFIG_STRING_SIZE];

  char userId[MAX_CONFIG_STRING_SIZE];
  char ssid[MAX_CONFIG_STRING_SIZE];
  char password[MAX_CONFIG_STRING_SIZE];
  char hostIP[MAX_CONFIG_STRING_SIZE];
  int portIn;
  int portOut;

  int accelRange;
  int gyroRange;

  // except for sensors, button and vibrator,
  // serial comm is never really disabled
  bool useWiFi;
  bool useSerial;
  // bool sendSingleFrame;
  // for sensors
  bool readMag;
  int readMagPeriod;
  int outputFramePeriod;
  int buttonHoldDuration;

  // enable / disable features like heartbeat, handshake, etc here

public:
  Config() :
  disabled(true),
  configExists(false),
  portIn(DEFAULT_OSC_INPUT_PORT),
  portOut(DEFAULT_OSC_OUTPUT_PORT),
  accelRange(DEFAULT_ACCEL_RANGE),
  gyroRange(DEFAULT_GYRO_RANGE),
  useWiFi(DEFAULT_USE_WIFI),
  useSerial(DEFAULT_USE_SERIAL),
  // sendSingleFrame(DEFAULT_SEND_SINGLE_FRAME),
  readMag(DEFAULT_READ_MAG),
  readMagPeriod(DEFAULT_READ_MAG_PERIOD),
  outputFramePeriod(DEFAULT_OUTPUT_FRAME_PERIOD),
  buttonHoldDuration(DEFAULT_BUTTON_HOLD_DURATION) {
    strcpy(initialized, "uninitialized");
    strcpy(movuinoId, "");
    strcpy(firmwareVersion, "");
    strcpy(userId, "1");
    strcpy(ssid, DEFAULT_NETWORK_SSID);
    strcpy(password, DEFAULT_NETWORK_PASSWORD);
    strcpy(hostIP, DEFAULT_HOST_IP);

    // EEPROM.begin(512);
  }

  ~Config() {}

  void init();
  void load();
  void store();
  void reset();

  //========================= ALL GETTERS AND SETTERS ========================//
  bool getInitialized();
  void setInitialized(bool b);

  const char *getMovuinoId();
  const char *getFirmwareVersion();

  const char *getUserId();
  void setUserId(const char *id);

  bool getUseWiFi();
  void setUseWiFi(bool b);

  const char *getSsid();
  void setSsid(const char *s);

  const char *getPassword();
  void setPassword(const char *p);

  const char *getHostIP();
  void setHostIP(const char *ip);

  int getInputPort();
  void setInputPort(int p);

  int getOutputPort();
  void setOutputPort(int p);

  int getAccelRange();
  void setAccelRange(int r);

  int getGyroRange();
  void setGyroRange(int r);

  bool getUseSerial();
  void setUseSerial(bool b);

  // bool getSendSingleFrame();
  // void setSendSingleFrame(bool b);

  bool getReadMag();
  void setReadMag(bool b);

  int getReadMagPeriod();
  void setReadMagPeriod(int p);

  int getOutputFramePeriod();
  void setOutputFramePeriod(int p);

  int getButtonHoldDuration();
  void setButtonHoldDuration(int d);

  void setParameter(String paramName, String strValue);


private:
  //===================== EEPROM READ / WRITE UTILITIES ======================//

  /*
  char readChar(int *address);
  void writeChar(int *address, char c);

  void readCharArray(int *address, char *str, unsigned int len);
  void writeCharArray(int *address, char *str, unsigned int len);

  unsigned int readUnsignedInt(int *address);
  void writeUnsignedInt(int *address, unsigned int v);
  //*/
};

#endif /* _MOVUINO_FIRMWARE_CONFIG_H_ */
