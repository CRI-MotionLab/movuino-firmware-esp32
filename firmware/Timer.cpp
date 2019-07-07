#include <Arduino.h>
#include "Timer.h"

void
Timer::setPeriod(unsigned long p) {
  period = p;
}

void
Timer::start(unsigned long t) {
  if (!running) {
    timeout = t;
    nextPeriod = period;
    startDate = lastDate = millis();
    running = true;
    // callback();
  }
}

void
Timer::stop() {
  if (running) {
    running = false;
  }
}

void
Timer::update() {
  if (!running) return;

  unsigned long now = millis();

  // with error compensation (doesn't work ATM, todo: hunt bugs) :

  // long error = now - (lastDate + nextPeriod);

  // if (error >= 0) {
  //   if (timeout == 0 || now - startDate < timeout) {
  //     callback();
  //   } else {
  //     stop();
  //     return;
  //   }

  //   // recompute in case callback took too long
  //   now = millis();
  //   error = now - (lastDate + nextPeriod);

  //   if (period > error) {
  //     nextPeriod = period - error;
  //   } else { // // in case the code takes too long to execute, we ignore the error
  //     nextPeriod = period;
  //   }

  //   lastDate = now;
  // }

  // dumber version :
  // seems to work well enough in case there are problems with the version above

  if (now >= lastDate + period) {
    lastDate = now;

    if (timeout == 0 || now - startDate < timeout) {
      callback();
    } else {
      stop();
    }
  }
}
