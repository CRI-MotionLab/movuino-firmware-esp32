#include <Arduino.h>
// #include "I2Cdev.h"
#include "Config.h"
#include "Router.h"
#include "Sensors.h"

//================================ TIMERS ====================================//

//*
void
MagTimer::callback() {
  // sensors->readMagValuesAsync();
  sensors->readMagValues();
}
//*/

void
AccelGyroTimer::callback() {
  sensors->readAccelGyroValues();
}

void
OSCOutTimer::callback() {
  sensors->sendSensorValues();
}

//================================ SENSORS ===================================//

void
Sensors::init(Config *c, Router *r) {
  config = c;
  router = r;

  // mpu->initialize();

  Wire.begin();
  mpu->setWire(&Wire);

  setAccelRange(config->getAccelRange());
  setGyroRange(config->getGyroRange());
  mpu->beginMag(MAG_MODE_CONTINUOUS_100HZ);

  
  for (unsigned int i = 0; i < 3; ++i) {
    magRange[i * 2] = 666;
    magRange[i * 2 + 1] = -666;
  }

  readAccelGyroTimer->setPeriod(config->getOutputFramePeriod());
  readMagTimer->setPeriod(DEFAULT_READ_MAG_PERIOD);
  oscOutTimer->setPeriod(config->getOutputFramePeriod());
  
  readAccelGyroTimer->start();
  readMagTimer->start();
  oscOutTimer->start();
}

void
Sensors::update() {
  // Serial.println("updating sensors");
//*
  readAccelGyroTimer->update();

  if (config->getReadMag()) {
    readMagTimer->update();
  }
  
  oscOutTimer->update();
//*/
}

////////// SENSOR RANGES

int
Sensors::getAccelRange() {
  // return mpu->getFullScaleAccelRange();
  return accelRange;
}

void
Sensors::setAccelRange(int r) {
  // mpu->setFullScaleAccelRange(r);
  switch (r) {
    case 0:
      mpu->beginAccel(ACC_FULL_SCALE_2_G);
      break;
    case 1:
      mpu->beginAccel(ACC_FULL_SCALE_4_G);
      break;
    case 2:
      mpu->beginAccel(ACC_FULL_SCALE_8_G);
      break;
    case 3:
      mpu->beginAccel(ACC_FULL_SCALE_16_G);
      break;
    default:
      return;
  }

  accelRange = r;
}

int
Sensors::getGyroRange() {
  // return mpu->getFullScaleGyroRange();
  return gyroRange;
}

void
Sensors::setGyroRange(int r) {
  // mpu->setFullScaleGyroRange(r);
  switch (r) {
    case 0:
      mpu->beginGyro(GYRO_FULL_SCALE_250_DPS);
      break;
    case 1:
      mpu->beginGyro(GYRO_FULL_SCALE_500_DPS);
      break;
    case 2:
      mpu->beginGyro(GYRO_FULL_SCALE_1000_DPS);
      break;
    case 3:
      mpu->beginGyro(GYRO_FULL_SCALE_2000_DPS);
      break;
    default:
      return;
  }

  gyroRange = r;
}

void
Sensors::setReadMagPeriod(int p) {
  readMagTimer->setPeriod(p);
}

void
Sensors::setOutputFramePeriod(int p) {
  readAccelGyroTimer->setPeriod(p);
  oscOutTimer->setPeriod(p);
}

//-------------------------------- PRIVATE -----------------------------------//

// original readMag method, to allow  synchronous reading
// (Timers must be off and sendSensorValues must be called explicitly)
/*
void
Sensors::readMagValuesSync() {
  // set i2c bypass enable pin to true to access magnetometer
  I2Cdev::writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x02);
  delay(10);
  // enable the magnetometer
  I2Cdev::writeByte(MPU9150_RA_MAG_ADDRESS, 0x0A, 0x01);
  delay(10);
  // read it !
  I2Cdev::readBytes(MPU9150_RA_MAG_ADDRESS, MPU9150_RA_MAG_XOUT_L, 6, magBuffer);
  mx = (((int16_t)magBuffer[1]) << 8) | magBuffer[0];
  my = (((int16_t)magBuffer[3]) << 8) | magBuffer[2];
  mz = (((int16_t)magBuffer[5]) << 8) | magBuffer[4];
  updateMagValues();
}
//*/

/**
 * adapted to be non blocking using a state flag
 * from the original sparkun / Jeff Rowberg MPU6050 library:
 * MPU6050::getMag(int16_t *mx, int16_t *my, int16_t *mz)
 * there used to be delay(10)'s between calls to writeByte and readBytes,
 * this function is now called in a non-blocking loop (a period of 10 ms works fine)
 * and keeps its own state up to date.
 */
////////// MagTimer callback :
/*
void
Sensors::readMagValuesAsync() {
  if (!config->getReadMag()) {
    values[6] = 0;
    values[7] = 0;
    values[8] = 0;
    return;
  }

  if (readMagState == 0) {
    // set i2c bypass enable pin to true to access magnetometer
    I2Cdev::writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x02);
  } else if (readMagState == 1) {
    // enable the magnetometer
    I2Cdev::writeByte(MPU9150_RA_MAG_ADDRESS, 0x0A, 0x01);
  } else {
    // read it !
    I2Cdev::readBytes(MPU9150_RA_MAG_ADDRESS, MPU9150_RA_MAG_XOUT_L, 6, magBuffer);
    mx = (((int16_t)magBuffer[1]) << 8) | magBuffer[0];
    my = (((int16_t)magBuffer[3]) << 8) | magBuffer[2];
    mz = (((int16_t)magBuffer[5]) << 8) | magBuffer[4];
    updateMagValues();
  }

  readMagState = (readMagState + 1) % 3;
}
//*/

void
Sensors::readMagValues() {
  mpu->magUpdate();

  // values[6] = mpu->magX();
  // values[7] = mpu->magY();
  // values[8] = mpu->magZ();

  fmx = mpu->magX();
  fmy = mpu->magY();
  fmz = mpu->magZ();

  updateMagValues();
}

////////// AccelGyroTimer callback :
void
Sensors::readAccelGyroValues() {
  // mpu->getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  //////////////// ACCELEROMETER :

  mpu->accelUpdate();

  float accelDivider = 1;

  switch (accelRange) {
    case 0:
      accelDivider = 2;
      break;
    case 1:
      accelDivider = 4;
      break;
    case 2:
      accelDivider = 8;
      break;
    case 3:
      accelDivider = 16;
      break;
    default:
      break;
  }

  values[0] = mpu->accelX() / accelDivider;
  values[1] = mpu->accelY() / accelDivider;
  values[2] = mpu->accelZ() / accelDivider;

  //////////////// GYROSCOPE :

  mpu->gyroUpdate();

  float gyroDivider = 1;

  switch (gyroRange) {
    case 0:
      gyroDivider = 250;
      break;
    case 1:
      gyroDivider = 500;
      break;
    case 2:
      gyroDivider = 1000;
      break;
    case 3:
      gyroDivider = 2000;
      break;
    default:
      break;
  }

  values[3] = mpu->gyroX() / gyroDivider;
  values[4] = mpu->gyroY() / gyroDivider;
  values[5] = mpu->gyroZ() / gyroDivider;

  // updateAccelGyroValues();  
}

////////// OSCOutTimer callback :
void
Sensors::sendSensorValues() {
  router->onNewSensorValues(&(values[0]));
}

/*
void
Sensors::updateAccelGyroValues() {
  values[0] = ax / float(32768);
  values[1] = ay / float(32768);
  values[2] = az / float(32768);

  values[3] = gx / float(32768);
  values[4] = gy / float(32768);
  values[5] = gz / float(32768);
}
//*/

//*
void
Sensors::updateMagValues() {
  magnetometerAutoCalibration();
  // or use some future calibration procedure result

  values[6] = fmx;
  values[7] = fmy;
  values[8] = fmz;
}
//*/

void
Sensors::magnetometerAutoCalibration() {
  float magVal[] = { fmx, fmy, fmz };

  for (int i = 0; i < 3; i++) {
    // Compute magnetometer range
    if (magVal[i] < magRange[2 * i]) {
      magRange[2 * i] = magVal[i]; // update minimum values on each axis
    }

    if (magVal[i] > magRange[2 * i + 1]) {
      magRange[2 * i + 1] = magVal[i]; // update maximum values on each axis
    }

    // Scale magnetometer values
    if (magRange[2*i] != magRange[2*i+1]) {
      // magVal[i] = map(magVal[i], magRange[2*i], magRange[2*i+1], -100, 100);
      magVal[i] = (magVal[i] - magRange[2 * i]) / (magRange[2 * i + 1] - magRange[2 * i]);
      magVal[i] *= 2;
      magVal[i] -= 1;
    }
  }

  // Update magnetometer values
  fmx = magVal[0];
  fmy = magVal[1];
  fmz = magVal[2];
}

