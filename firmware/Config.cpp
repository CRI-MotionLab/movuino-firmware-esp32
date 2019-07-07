#include <FS.h>
#include <SPIFFS.h>
// #include <ESP8266WiFi.h>
#include "Config.h"

#include "esp_system.h" // to read mac address

void
Config::init() {
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  sprintf(
    movuinoId,
    "%02X%02X%02X%02X%02X%02X",
    baseMac[0],
    baseMac[1],
    baseMac[2],
    baseMac[3],
    baseMac[4],
    baseMac[5]
  );

  sprintf(firmwareVersion, "%i.%i.%i",
    MOVUINO_FIRMWARE_VERSION_MAJOR,
    MOVUINO_FIRMWARE_VERSION_MINOR,
    MOVUINO_FIRMWARE_VERSION_PATCH
  );

  if (SPIFFS.begin(true)) {
    disabled = false;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  // bool configExists = false;

  while (file) {
    if (!file.isDirectory()) {
      if (strcmp(file.name(), "/config.txt") == 0) {
        configExists = true;
        break;
      } else {
        // delete file ?
        configExists = false;
      }
    }

    file = root.openNextFile();
  }

  // if (!configExists) {
  //   save();
  // } else {
  //   load();
  // }
}

void
Config::load() {
  if (!configExists) return;

  File file = SPIFFS.open("/config.txt");
  String param = "";
  String value = "";
  bool readingName = true;

  if (!file) return; // false;

  while (file.available()) {
    char c = file.read();

    // Serial.print(c);

    if (c == '\n') {
      setParameter(param, value);
      param = "";
      value = "";
      readingName = true;
    } else if (c == ' ') {
      readingName = false;
    } else {
      if (readingName) {
        param += c;
      } else {
        value += c;
      }
    }
  }

  file.close();
  // return true;

  /*
  if (disabled) return;

  bool fExists = SPIFFS.exists("/config.txt");
  File file = SPIFFS.open("/config.txt", "r");

  if (file && fExists) {
    String _initialized = file.readStringUntil('\n');

    String _userId = file.readStringUntil('\n');
    String _ssid = file.readStringUntil('\n');
    String _password = file.readStringUntil('\n');
    String _hostIP = file.readStringUntil('\n');

    portIn = file.readStringUntil('\n').toInt();
    portOut = file.readStringUntil('\n').toInt();

    accelRange = file.readStringUntil('\n').toInt();
    gyroRange = file.readStringUntil('\n').toInt();

    useWiFi = file.readStringUntil('\n').toInt() > 0;
    useSerial = file.readStringUntil('\n').toInt() > 0;
    // sendSingleFrame = file.readStringUntil('\n').toInt() > 0;

    readMag = file.readStringUntil('\n').toInt() > 0;
    readMagPeriod = file.readStringUntil('\n').toInt();
    outputFramePeriod = file.readStringUntil('\n').toInt();
    buttonHoldDuration = file.readStringUntil('\n').toInt();

    _initialized.toCharArray(initialized, _initialized.length());
    _userId.toCharArray(userId, _userId.length());
    _ssid.toCharArray(ssid, _ssid.length());
    _password.toCharArray(password, _password.length());
    _hostIP.toCharArray(hostIP, _hostIP.length());

    file.close();
  }
  //*/

  /*
  int address = 0;

  readCharArray(&address, initialized, MAX_CONFIG_STRING_SIZE);


  if (strcmp(initialized, "initialized") == 0) {
    readCharArray(&address, ssid, MAX_CONFIG_STRING_SIZE);
    readCharArray(&address, password, MAX_CONFIG_STRING_SIZE);
    readCharArray(&address, hostIP, MAX_CONFIG_STRING_SIZE);

    portIn = (int) readUnsignedInt(&address);
    portOut = (int) readUnsignedInt(&address);

    accelRange = (int) readUnsignedInt(&address);
    gyroRange = (int) readUnsignedInt(&address);

    useWiFi = readChar(&address) == '1';
    useSerial = readChar(&address) == '1';
    sendSingleFrame = readChar(&address) == '1';

    readMagPeriod = (int) readUnsignedInt(&address);
    outputFramePeriod = (int) readUnsignedInt(&address);
    buttonHoldDuration = (int) readUnsignedInt(&address);
  }
  //*/
}

void
Config::store() {
  File file = SPIFFS.open("/config.txt", FILE_WRITE);

  if (!file) return; // false;

  String line;

  line = String("userId ") + userId + "\n";
  file.print(line.c_str());
  line = String("ssid ") + ssid + "\n";
  file.print(line.c_str());
  line = String("password ") + password + "\n";
  file.print(line.c_str());
  line = String("hostIP ") + hostIP + "\n";
  file.print(line.c_str());
  line = String("portIn ") + portIn + "\n";
  file.print(line.c_str());
  line = String("portOut ") + portOut + "\n";
  file.print(line.c_str());
  line = String("accelRange ") + accelRange + "\n";
  file.print(line.c_str());
  line = String("gyroRange ") + gyroRange + "\n";
  file.print(line.c_str());
  line = String("useWiFi ") + (useWiFi ? 1 : 0)  + "\n";
  file.print(line.c_str());
  line = String("useSerial ") + (useSerial ? 1 : 0) + "\n";
  file.print(line.c_str());
  line = String("readMag ") + (readMag ? 1 : 0) + "\n";
  file.print(line.c_str());
  line = String("readMagPeriod ") + readMagPeriod + "\n";
  file.print(line.c_str());
  line = String("outputFramePeriod ") + outputFramePeriod + "\n";
  file.print(line.c_str());
  line = String("buttonHoldDuration ") + buttonHoldDuration + "\n";
  file.print(line.c_str());

  file.close();
  // return true;

  /*
  if (disabled) return;

  File file = SPIFFS.open("/config.txt", "w+");

  if (file) {
    if (strcmp(initialized, "initialized") != 0){
      strcpy(initialized, "initialized");
    }

    file.println(initialized);

    file.println(userId);
    file.println(ssid);
    file.println(password);
    file.println(hostIP);
    file.println(portIn);
    file.println(portOut);

    file.println(accelRange);
    file.println(gyroRange);

    file.println(useWiFi ? "1" : "0");
    file.println(useSerial ? "1" : "0");
    // file.println(sendSingleFrame ? "1" : "0");
    file.println(readMag ? "1" : "0");
    file.println(readMagPeriod);
    file.println(outputFramePeriod);
    file.println(buttonHoldDuration);

    file.close();
  }
  //*/

  /*
  int address = 0;

  writeCharArray(&address, "initialized", MAX_CONFIG_STRING_SIZE);
  writeCharArray(&address, ssid, MAX_CONFIG_STRING_SIZE);
  writeCharArray(&address, password, MAX_CONFIG_STRING_SIZE);
  writeCharArray(&address, hostIP, MAX_CONFIG_STRING_SIZE);

  writeUnsignedInt(&address, (unsigned int) portIn);
  writeUnsignedInt(&address, (unsigned int) portOut);

  writeUnsignedInt(&address, (unsigned int) accelRange);
  writeUnsignedInt(&address, (unsigned int) gyroRange);

  writeChar(&address, useWiFi ? '1' : '0');
  writeChar(&address, useSerial ? '1' : '0');
  writeChar(&address, sendSingleFrame ? '1' : '0');

  writeUnsignedInt(&address, (unsigned int) readMagPeriod);
  writeUnsignedInt(&address, (unsigned int) outputFramePeriod);
  writeUnsignedInt(&address, (unsigned int) buttonHoldDuration);

  EEPROM.commit();
  //*/
}

void
Config::reset() {
  setInitialized(false);

  setSsid(DEFAULT_NETWORK_SSID);
  setPassword(DEFAULT_NETWORK_PASSWORD);
  setHostIP(DEFAULT_HOST_IP);
  setInputPort(DEFAULT_OSC_INPUT_PORT);
  setOutputPort(DEFAULT_OSC_OUTPUT_PORT);
  setAccelRange(DEFAULT_ACCEL_RANGE);
  setGyroRange(DEFAULT_GYRO_RANGE);
  setUseWiFi(DEFAULT_USE_WIFI);
  setUseSerial(DEFAULT_USE_SERIAL);
  // setSendSingleFrame(DEFAULT_SEND_SINGLE_FRAME);
  setReadMag(DEFAULT_READ_MAG);
  setReadMagPeriod(DEFAULT_READ_MAG_PERIOD);
  setOutputFramePeriod(DEFAULT_OUTPUT_FRAME_PERIOD);
  setButtonHoldDuration(DEFAULT_BUTTON_HOLD_DURATION);

  if (disabled) return;

  if (configExists) {
    SPIFFS.remove("/config.txt");
  }

  // if (SPIFFS.exists("/config.txt")) {
  //   SPIFFS.remove("/config.txt");
  // }

  // disabled = true;
  // SPIFFS.end();
  // if (SPIFFS.format() && SPIFFS.begin()) {
  //   disabled = false;
  // }
}

//========================= GETTERS AND SETTERS ==============================//

bool
Config::getInitialized() {
  return strcmp(initialized, "initialized") == 0;
}

void
Config::setInitialized(bool b) {
  strcpy(initialized, b ? "initialized" : "uninitialized");
}

const char *
Config::getMovuinoId() {
  return movuinoId;
}

const char *
Config::getFirmwareVersion() {
  return firmwareVersion;
}

const char *
Config::getUserId() {
  return userId;
}

void
Config::setUserId(const char *id) {
  strcpy(userId, id);
}

////////// WIFI SETTINGS

const char *
Config::getSsid() {
  return ssid;
}

void
Config::setSsid(const char *s) {
  strcpy(ssid, s);
}

const char *
Config::getPassword() {
  return password;
}

void
Config::setPassword(const char *p) {
  strcpy(password, p);
}

const char *
Config::getHostIP() {
  return hostIP;
}

void
Config::setHostIP(const char *ip) {
  strcpy(hostIP, ip);
}

////////// INPUT / OUTPUT OSC PORTS

int
Config::getInputPort() {
  return portIn;
}

void
Config::setInputPort(int p) {
  portIn = p;
}

int
Config::getOutputPort() {
  return portOut;
}

void
Config::setOutputPort(int p) {
  portOut = p;
}

////////// ACCEL / GYRO SENSITIVITY :

/*
 * From the MPU6050 library's doc :
 * 0 = +/- 2g
 * 1 = +/- 4g
 * 2 = +/- 8g
 * 3 = +/- 16g
 */

int
Config::getAccelRange() {
  return accelRange;
}

void
Config::setAccelRange(int r) {
  accelRange = r < 0 ? 0 : (r > 3 ? 3 : r);
}

/*
 * From the MPU6050 library's doc :
 * 0 = +/- 250 degrees/sec
 * 1 = +/- 500 degrees/sec
 * 2 = +/- 1000 degrees/sec
 * 3 = +/- 2000 degrees/sec
 */

int
Config::getGyroRange() {
  return gyroRange;
}

void
Config::setGyroRange(int r) {
  gyroRange = r < 0 ? 0 : (r > 3 ? 3 : r);
}

////////// OTHER CONFIG PARAMETERS

bool
Config::getUseWiFi() {
  return useWiFi;
}

void
Config::setUseWiFi(bool b) {
  useWiFi = b;
}

bool
Config::getUseSerial() {
  return useSerial;
}

void
Config::setUseSerial(bool b) {
  useSerial = b;
}

// bool
// Config::getSendSingleFrame() {
//   return sendSingleFrame;
// }

// void
// Config::setSendSingleFrame(bool b) {
//   sendSingleFrame = b;
// }

bool
Config::getReadMag() {
  return readMag;
}

void
Config::setReadMag(bool b) {
  readMag = b;
}

int
Config::getReadMagPeriod() {
  return readMagPeriod;
}

void
Config::setReadMagPeriod(int p) {
  readMagPeriod = p;
}

int
Config::getOutputFramePeriod() {
  return outputFramePeriod;
}

void
Config::setOutputFramePeriod(int p) {
  outputFramePeriod = p;
}

int
Config::getButtonHoldDuration() {
  return buttonHoldDuration;
}

void
Config::setButtonHoldDuration(int p) {
  buttonHoldDuration = p;
}

    // file.println(userId);
    // file.println(ssid);
    // file.println(password);
    // file.println(hostIP);
    // file.println(portIn);
    // file.println(portOut);

    // file.println(accelRange);
    // file.println(gyroRange);

    // file.println(useWiFi ? "1" : "0");
    // file.println(useSerial ? "1" : "0");
    // // file.println(sendSingleFrame ? "1" : "0");
    // file.println(readMag ? "1" : "0");
    // file.println(readMagPeriod);
    // file.println(outputFramePeriod);
    // file.println(buttonHoldDuration);

void
Config::setParameter(String paramName, String strValue) {
  if (paramName == "userId") {
    setUserId(strValue.c_str());
  } else if (paramName == "ssid") {
    setSsid(strValue.c_str());
  } else if (paramName == "password") {
    setPassword(strValue.c_str());
  } else if (paramName == "hostIP") {
    setHostIP(strValue.c_str());
  } else if (paramName == "portIn") {
    setInputPort(strValue.toInt());
  } else if (paramName == "portOut") {
    setOutputPort(strValue.toInt());
  } else if (paramName == "accelRange") {
    setAccelRange(strValue.toInt());
  } else if (paramName == "gyroRange") {
    setGyroRange(strValue.toInt());
  } else if (paramName == "useWiFi") {
    setUseWiFi(strValue.toInt() == 1);
  } else if (paramName == "useSerial") {
    setUseSerial(strValue.toInt() == 1);
  } else if (paramName == "readMag") {
    setReadMag(strValue.toInt() == 1);
  } else if (paramName == "readMagPeriod") {
    setReadMagPeriod(strValue.toInt());
  } else if (paramName == "outputFramePeriod") {
    setOutputFramePeriod(strValue.toInt());
  } else if (paramName == "buttonHoldDuration") {
    setButtonHoldDuration(strValue.toInt());
  }
}

