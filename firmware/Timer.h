#ifndef _MOVUINO_FIRMWARE_TIMER_H_
#define _MOVUINO_FIRMWARE_TIMER_H_

// this class should be overridden to do specific things in callback

class Timer {
protected:
  unsigned long timeout;
  unsigned long period;
  unsigned long nextPeriod;
  unsigned long startDate;
  unsigned long lastDate;
  bool running;

public:
  Timer(unsigned long p) :
  timeout(0),
  period(p), nextPeriod(p),
  startDate(0), lastDate(0),
  running(false) {}

  ~Timer() {}

  // these methods need not be overridden
  virtual void setPeriod(unsigned long p);
  virtual void start(unsigned long t = 0);
  virtual void stop();
  virtual void update(); // trig the callback or stop if necessary

  // this one should be overridden first
  virtual void callback() {}
};

#endif /* _MOVUINO_FIRMWARE_TIMER_H_ */
