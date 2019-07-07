#ifndef _MOVUINO_FIRMWARE_NEOPIX_H_
#define _MOVUINO_FIRMWARE_NEOPIX_H_

#include <Adafruit_NeoPixel.h>

class Config;
class Router;

class Neopix {
  private:
    Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, pinNeoPix, NEO_GRB + NEO_KHZ800);

    long timerLedShow;
    int timeRefresh;

    Config *config;
    Router *router;

  public:
    Neopix():
    timerLedShow(millis()), timeRefresh(20) {}

    ~Neopix() {} // destructeur

    void init(Config *c, Router *r);
    void update();

    void setColor(int red_, int green_, int blue_);
};

#endif /* _MOVUINO_FIRMWARE_NEOPIX_H_ */
