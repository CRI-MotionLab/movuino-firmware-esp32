#ifndef _MOVUINO_FIRMWARE_GLOBALS_H_
#define _MOVUINO_FIRMWARE_GLOBALS_H_

#include <Arduino.h>

#define MOVUINO_FIRMWARE_VERSION_MAJOR 1
#define MOVUINO_FIRMWARE_VERSION_MINOR 0
#define MOVUINO_FIRMWARE_VERSION_PATCH 0

#define MOVUINO_READ_MAG_ASYNC true

#define MAX_CONFIG_STRING_SIZE 32
#define MAX_TOTAL_CONFIG_LENGTH 32
#define MAX_TOTAL_CONFIG_STRING_SIZE 256

#define MAX_OSC_ADDRESSES 100
#define MAX_INPUT_OSC_ADDRESSES 20
#define MAX_OUTPUT_OSC_ADDRESSES 20
#define MAX_OSC_ADDRESS_LENGTH 64
#define MAX_OSC_STRING_ARG_LENGTH 32

#define WIFI_CONNECTION_TIMEOUT 20000

#define DEFAULT_WEB_SERVER_PORT 80
#define DEFAULT_WEBSOCKET_SERVER_PORT 81

#define DEFAULT_NETWORK_SSID "XXXX-147e"
#define DEFAULT_NETWORK_PASSWORD "phewvomnamregcuvyeb2"
#define DEFAULT_HOST_IP "192.168.0.12"
#define DEFAULT_OSC_INPUT_PORT 7401
#define DEFAULT_OSC_OUTPUT_PORT 7400

/*
 * From the MPU6050 library docs :
 * 0 = +/- 2g
 * 1 = +/- 4g
 * 2 = +/- 8g
 * 3 = +/- 16g
 */
#define DEFAULT_ACCEL_RANGE 1 // +/- 4g

/*
 * From the MPU6050 library docs :
 * 0 = +/- 250 degrees/sec
 * 1 = +/- 500 degrees/sec
 * 2 = +/- 1000 degrees/sec
 * 3 = +/- 2000 degrees/sec
 */
#define DEFAULT_GYRO_RANGE 2 // +/- 1000 degrees/sec

#define DEFAULT_USE_WIFI true
#define DEFAULT_USE_SERIAL true // for sensors, button and vibrator
#define DEFAULT_SEND_SINGLE_FRAME true // pure UDP, no websocket

#define DEFAULT_READ_MAG true
#define DEFAULT_READ_MAG_PERIOD 10
#define DEFAULT_OUTPUT_FRAME_PERIOD 10
#define DEFAULT_BUTTON_HOLD_DURATION 500

#define BUTTON_BOOT_HOLD_DURATION 1000

#define LOW_BLINK_PERIOD 200
#define FAST_BLINK_PERIOD 50

const int pinBtn = 13;     // the number of the pushbutton pin
const int pinLedWifi = 2;  // wifi led indicator
const int pinLedBat = 0;   // battery led indicator
const int pinVibro = 14;   // vibrator pin
const int pinNeoPix = 15;  // neopixel led pin

//================================= ENUMS ====================================//

enum ButtonState {
  ButtonNone = 0,
  ButtonPressed,
  ButtonReleased,
  ButtonHolding
};

enum WiFiBootMode {
  WiFiStation = 0,
  WiFiAccessPoint
};

enum WiFiConnectionState {
  WiFiDisconnected = 0,
  WiFiConnecting,
  WiFiConnected
};

enum APCommand {
  APUnknown = 0,
  APClear,
  APSettings
};

//=============================== BOOT MODE ==================================//

// return true if AP mode required by holding button from boot
// during BUTTON_BOOT_HOLD_DURATION ms

static WiFiBootMode wifiBootMode = WiFiStation;

static WiFiBootMode checkBootMode() {
  bool btnOn = digitalRead(pinBtn) == 0;
  unsigned long elapsedTime = 0;
  unsigned long bootDate = millis();

  while (btnOn && elapsedTime < BUTTON_BOOT_HOLD_DURATION) {
    delay(10);
    digitalWrite(pinLedWifi, LOW);
    btnOn = digitalRead(pinBtn) == 0;
    elapsedTime = millis() - bootDate;
  }

  // in case we release the button in the same loop where elapsedTime becomes
  // >= BUTTON_BOOT_HOLD_DURATION
  if (btnOn && elapsedTime >= BUTTON_BOOT_HOLD_DURATION) {
    bool light = true;

    // blink fast until release
    while (btnOn) {
      digitalWrite(pinLedWifi, light ? LOW : HIGH); // turn OFF wifi led
      delay(FAST_BLINK_PERIOD);
      btnOn = digitalRead(pinBtn) == 0;
      light = !light;
    }

    digitalWrite(pinLedWifi, LOW);
    return WiFiAccessPoint;
  }

  return WiFiStation;
}

//============================ SOME UTIL(S) ==================================//

static int getConnectionStateOSCValue(WiFiConnectionState s) {
  int val = 0;

  if (s == WiFiConnecting) { // send 2
    val = 2;
  } else if (s == WiFiConnected) {
    val = 1;
  } else if (s == WiFiDisconnected) {
    val = 0;
  }

  return val;
}

//============================= OSC ADDRESSES ================================//

enum oscGetSet { oscGet = 0, oscSet };

//----------------------------------------------------------------------------//

enum oscAddress {
  oscHello = 0,
  oscGetId,
  oscSetId,
  oscSerialEnable,
  oscMagnetoEnable,
  oscWifiEnable,
  oscWifiState,
  oscGetWifi,
  oscSetWifi,
  oscGetPorts,
  oscSetPorts,
  oscGetRange,
  oscSetRange,
  oscGetFrameperiod,
  oscSetFrameperiod,
  oscFrame,
  oscVibroPulse,
  oscVibroNow,
  oscNeopix,
  nbOscAddresses // always keep this in the last place
};

static char oscAddresses[nbOscAddresses][MAX_OSC_ADDRESS_LENGTH];

static void initOscAddresses() {
  strcpy(oscAddresses[oscHello], "/hello");
  strcpy(oscAddresses[oscGetId], "/id/get");
  strcpy(oscAddresses[oscSetId], "/id/set");
  strcpy(oscAddresses[oscSerialEnable], "/serial/enable");
  strcpy(oscAddresses[oscMagnetoEnable], "/magneto/enable");
  strcpy(oscAddresses[oscWifiEnable], "/wifi/enable");
  strcpy(oscAddresses[oscWifiState], "/wifi/state");
  strcpy(oscAddresses[oscGetWifi], "/wifi/get");
  strcpy(oscAddresses[oscSetWifi], "/wifi/set");
  strcpy(oscAddresses[oscGetPorts], "/ports/get");
  strcpy(oscAddresses[oscSetPorts], "/ports/set");
  strcpy(oscAddresses[oscGetRange], "/range/get");
  strcpy(oscAddresses[oscSetRange], "/range/set");
  strcpy(oscAddresses[oscGetFrameperiod], "/frameperiod/get");
  strcpy(oscAddresses[oscSetFrameperiod], "/frameperiod/set");
  strcpy(oscAddresses[oscFrame], "/movuino"); // both serial and wifi (out)
  strcpy(oscAddresses[oscVibroPulse], "/vibro/pulse"); // both serial and wifi (in)
  strcpy(oscAddresses[oscVibroNow], "/vibro/now"); // both serial and wifi (in)
  strcpy(oscAddresses[oscNeopix], "/neopix"); // both serial and wifi (in)
}

#endif /* _MOVUINO_FIRMWARE_GLOBALS_H_ */
