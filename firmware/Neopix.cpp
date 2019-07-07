#include <Arduino.h>
#include "Config.h"
#include "Router.h"
#include "Neopix.h"
#include "globals.h"

void
Neopix::init(Config *c, Router *r) {
  config = c;
  router = r;

  strip.begin();
  strip.show();
}

void
Neopix::update() {
  if (millis() - timerLedShow > timeRefresh) {
    strip.show();
    timerLedShow = millis();
  }
}

void
Neopix::setColor(int red_, int green_, int blue_) {
  strip.setPixelColor(0, strip.Color(red_, green_, blue_));
}
