#ifndef _MOVUINO_FIRMWARE_BUTTON_H_
#define _MOVUINO_FIRMWARE_BUTTON_H_

#include "globals.h"

class Config;
class Router;

class Button {
private:
  bool btnOn;
  bool holding;
  // unsigned long btnPressTimeThresh; // pressure time needed to trig hold event
  unsigned long lastBtnDate;

  ButtonState state;

  Config *config;
  Router *router;

public:
  Button() :
  btnOn(false),
  holding(false),
  // btnPressTimeThresh(DEFAULT_BUTTON_HOLD_DURATION),
  lastBtnDate(0) {}

  ~Button() {}

  void init(Config *c, Router *r);
  void update();
  ButtonState getState();
};

#endif /* _MOVUINO_FIRMWARE_BUTTON_H_ */
