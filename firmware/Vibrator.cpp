#include <Arduino.h>
#include "Config.h"
#include "Router.h"
#include "Vibrator.h"
#include "globals.h"

void
Vibrator::init(Config *c, Router *r) {
  config = c;
  router = r;
}

void
Vibrator::update() {
  // swap to if (isConstantlyVibrating) else if (isPulsing) to give priority to constant vibration
  if (isPulsing) {
    // how many cycles we've already been through (floating point * 1000)
    int curTimeRatio1000 = (int) (1000 * (millis() - vibTimer) / (float) dVibTotal);

    if (curTimeRatio1000 % 1000 < (int) (1000 * rVib)) {
      isActuallyVibrating = true;
      digitalWrite(pinVibro, HIGH);
    } else {
      if (dVibOff != 0) {
        isActuallyVibrating = false;
        digitalWrite(pinVibro, LOW);
      }
    }

    // Shut down vibrator if number of cycles reach (set nVib to -1 for infinite cycles)
    if (nVib != -1 && (millis() - vibTimer > nVib * dVibTotal)) {
      isPulsing = isActuallyVibrating = false;
      digitalWrite(pinVibro, LOW);
    }
  } else if (isConstantlyVibrating) {
    isActuallyVibrating = true;
    digitalWrite(pinVibro, HIGH);
  } else {
    isActuallyVibrating = false;
    digitalWrite(pinVibro, LOW);
  }
}

void
Vibrator::vibrate(bool vibOnOff) {
  if (vibOnOff) {
    isConstantlyVibrating = true;
    // isConstantlyVibrating = isActuallyVibrating = true;
    // digitalWrite(pinVibro, HIGH);
  } else {
    isConstantlyVibrating = false;
    // isConstantlyVibrating = isActuallyVibrating = false;
    // digitalWrite(pinVibro, LOW);
  }
}

void
Vibrator::pulse(unsigned long onDuration, unsigned long offDuration, long nb) {
  dVibOn = onDuration;
  dVibOff = offDuration;
  nVib = nb;

  if (dVibOn == 0) {
    isPulsing = false;
    // isPulsing = isActuallyVibrating = false; // shut down vibrator if no vibration
    // digitalWrite(pinVibro, LOW);
  } else {
    dVibTotal = dVibOn + dVibOff;
    rVib = dVibOn / (float) dVibTotal;
    vibTimer = millis();
    isPulsing = true;
    // isActuallyVibrating = true;
    // digitalWrite(pinVibro, HIGH);
  }
}

bool
Vibrator::isVibrating() {
  return isActuallyVibrating;
}
