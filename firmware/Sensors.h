#ifndef _MOVUINO_FIRMWARE_SENSORS_H_
#define _MOVUINO_FIRMWARE_SENSORS_H_

// #include "MPU6050.h"
#include <MPU9250_asukiaaa.h>
#include "Timer.h"

class Config;
class Router;
class Sensors;

//----------------------------------------------------------------------------//
// child class of Timer specialized in reading magnetometer sensor values

//*
class MagTimer : public Timer {
private:
  Sensors *sensors;
public:
  MagTimer(Sensors *s, unsigned long period) :
  Timer(period), sensors(s) {}
  virtual ~MagTimer() {}
  virtual void callback();
};
//*/

//----------------------------------------------------------------------------//
// child class of Timer specialized in reading accel / gyro sensor values

class AccelGyroTimer : public Timer {
private:
  Sensors *sensors;
public:
  AccelGyroTimer(Sensors *s, unsigned long period) :
  Timer(period), sensors(s) {}
  virtual ~AccelGyroTimer() {}
  virtual void callback();
};

//----------------------------------------------------------------------------//

class OSCOutTimer : public Timer {
private:
  Sensors *sensors;
public:
  OSCOutTimer(Sensors *s, unsigned long period) :
  Timer(period), sensors(s) {}
  virtual ~OSCOutTimer() {}
  virtual void callback();
};

//=========================== ACTUAL SENSORS CLASS ===========================//

class Sensors {
  friend class MagTimer;
  friend class AccelGyroTimer;
  friend class OSCOutTimer;

private:
  // composition over inheritance !
  // unsigned int readMagState;
  MagTimer *readMagTimer;
  AccelGyroTimer *readAccelGyroTimer;
  OSCOutTimer *oscOutTimer;

  // MPU6050 *mpu;
  MPU9250 *mpu;

  int accelRange;
  int gyroRange;

  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  int16_t mx, my, mz;

  float fmx, fmy, fmz;
  float values[9];
  bool readMag;
  float magRange[6];// = {666, -666, 666, -666, 666, -666}; // magneto range values for calibration
  uint8_t magBuffer[14];

  Config *config;
  Router *router;

public:
  Sensors() /* : readMagState(0) */ {
    readMagTimer = new MagTimer(this, 10);
    readAccelGyroTimer = new AccelGyroTimer(this, 10);
    oscOutTimer = new OSCOutTimer(this, 10);
    // mpu = new MPU6050(0x69);

    // mpu = new MPU9250(MPU9250_ADDRESS_AD0_LOW); // default (see MPU9250_asukiaaa.h)
    mpu = new MPU9250(MPU9250_ADDRESS_AD0_HIGH); // this is the right one for movuino esp32
  }

  ~Sensors() {
    delete readMagTimer;
    delete readAccelGyroTimer;
    delete oscOutTimer;
    delete mpu;
  }

  void init(Config *c, Router *r); // initialize device, set ranges and start timer
  void update(); // accel and gyro values are read on each call to update()

  int getAccelRange();
  void setAccelRange(int r);

  int getGyroRange();
  void setGyroRange(int r);

  void setReadMagPeriod(int p);
  void setOutputFramePeriod(int p);

private:
  // only executed by friend class MagTimer
  void readMagValues();
  // void readMagValuesAsync();
  // void readMagValuesSync(); // blocking version, not used

  // only executed by friend class AccelGyroTimer  
  void readAccelGyroValues();

  // only executed by friend class OSCOutTimer
  void sendSensorValues();

  void updateAccelGyroValues();
  void updateMagValues();
  void magnetometerAutoCalibration();
};

#endif /* _MOVUINO_FIRMWARE_SENSORS_H_ */
