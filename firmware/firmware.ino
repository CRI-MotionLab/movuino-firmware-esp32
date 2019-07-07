#include "Router.h"
#include "Config.h"
#include "Neopix.h"
#include "Button.h"
#include "Vibrator.h"
#include "Sensors.h"
#include "SerialInterface.h"
#include "WiFiInterface.h"
// #include "AccessPoint.h"

#include "globals.h"

Router router;
Config config;
Button button;
Sensors sensors;
Vibrator vibrator;
Neopix neopix;
SerialInterface serial;
WiFiInterface wifi;

// AccessPoint *ap;

WiFiBootMode mode;

void setup() {
  pinMode(pinBtn, INPUT_PULLUP); // pin for the button
  // pinMode(pinLedWifi, OUTPUT); // pin for the wifi led
  // pinMode(pinLedBat, OUTPUT); // pin for the battery led
  // pinMode(pinVibro, OUTPUT); // pin for the vibrator

  // mode = checkBootMode();

  // if (mode == WiFiStation) {
    // Wire.begin();
    // Wire.setClock(400000);

  // Serial.begin(115200);
  // delay(5000);
  // Serial.println("about to init router");
  // delay(5000);
      
    router.init(&config, &button, &neopix, &sensors, &vibrator, &serial, &wifi);
  // } else {
  //   ap = new AccessPoint();
  //   ap->init(&config);
  // }
}

void loop() {
  // if (mode == WiFiStation) {
    router.update();
  // } else {
  //   ap->update();
  // }

  delay(1); // let the board breathe
  // Serial.println("looping");
}
