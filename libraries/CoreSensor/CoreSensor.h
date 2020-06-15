#pragma once

#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"
#include "CoreLogging.h"
#include "CoreCommon.h"
#include "SparkFunLSM6DS3.h"

#define PROTOTYPE false

//IMU
#define IMU_INT1_PIN 0
#define SDO_PIN 17

//Core
#define SWING_THRESHOLD 90 //AVG of 3 gyro axes
#define VERTICAL_ACC 8.0
#define VERTICAL_ARM -6.0
#define HORIZONTAL_ACC 0.0
#define TOLERANCE_ARM 0.7
#define TOLERANCE_DISARM 1.0
#define TIME_FOR_ARM 1000
#define TIME_FOR_DISARM 4000
#define TIME_FOR_CONFIRM_ARM 2000
#define CLASH_TRESHOLD 0x0A //min 0x00 (0) max 0x1F (31) 5 bits, middle 0x0F (15)

#define FILTER_SENSOR_ITEMS 20

class CoreSensor
{
public:
  //Costructor
  CoreSensor();
  //Init
  void init(bool debug);
  //Process loop
  void loop(bool &rNeedSwing, bool &rNeedClash, Status &rStatus,
            bool &rVerticalPosition, bool &rNeedArm, bool &rHorizontalPosition,
            bool &rNeedDisarm);
  //Interrupt status for clash
  uint8_t int1Status;
  //Get Interrupt Pin
  int getInt1Pin();
  Status getStatus();

private:
  bool debugMode = false;
  CoreLogging logger;

  FilterSensor filterSensorData = {0, 0.0, 0.0};
  LSM6DS3 device;

  unsigned long time;

  uint8_t errorAccumulator = 0;
  uint8_t dataToWrite = 0;

  float valueAccel = 0.0;
  float minValue = VERTICAL_ACC - TOLERANCE_ARM;
  float maxValue = VERTICAL_ACC + TOLERANCE_ARM;
  float minArmValue = VERTICAL_ARM - TOLERANCE_ARM;
  float maxArmValue = VERTICAL_ARM + TOLERANCE_ARM;
  float minHValue = HORIZONTAL_ACC - TOLERANCE_DISARM;
  float maxHValue = HORIZONTAL_ACC + TOLERANCE_DISARM;

  float gyroAvg = 0.0;
  //float max2 = 0.0;

  Status status = Status::disarmed;

  bool isVerticalPosition = false;
  bool isHorizontalPosition = false;
  bool lastIsVerticalPosition = false;
  bool lastIsHorizontalPosition = false;

  bool needSwing(); //Get if need exec swing event
  bool needClash(); //Get if need exec clash event
  bool containVertical(float pValue);
  bool containHorizontal(float pValue);
  bool containArm(float pValue);
  bool needArm();
  bool needDisarm();
  void updateAverageHorizontalData();
};
