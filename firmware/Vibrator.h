#ifndef _MOVUINO_FIRMWARE_VIBRATOR_H_
#define _MOVUINO_FIRMWARE_VIBRATOR_H_

class Config;
class Router;

class Vibrator {
private:
  bool isPulsing;
  bool isConstantlyVibrating;
  bool isActuallyVibrating;

  unsigned long dVibOn, dVibOff, dVibTotal;
  float rVib;
  unsigned long vibTimer;
  long nVib;

  Config *config;
  Router *router;

public:
  Vibrator() :
  isPulsing(false), isConstantlyVibrating(false), isActuallyVibrating(false),
  dVibOn(0), dVibOff(0), dVibTotal(0), rVib(0), vibTimer(0), nVib(0) {}

  ~Vibrator() {}

  void init(Config *c, Router *r);
  void update();

  // pulse has higher priority over constant vibration
  void pulse(unsigned long onDuration, unsigned long offDuration, long nb);
  void vibrate(bool on);

  bool isVibrating();
};

#endif /* _MOVUINO_FIRMWARE_VIBRATOR_H_ */
