#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "CoreCommon.h"
#include "CoreLogging.h"
#include "SparkFunLSM6DS3.h"

#define PROTOTYPE false

// IMU
#define IMU_INT1_PIN 0
#define SDO_PIN 17

// Core
static constexpr int SWING_THRESHOLD = 90; // AVG of 3 gyro axes
static constexpr int ARM_THRESHOLD = 300;
static constexpr float VERTICAL_ACC = 8.0;
static constexpr float VERTICAL_ARM = -6.0;
static constexpr float HORIZONTAL_ACC = 0.0;
static constexpr float TOLERANCE_ARM = 0.7;
static constexpr float TOLERANCE_DISARM = 1.0;
static constexpr int TIME_FOR_ARM = 150;
static constexpr int TIME_FOR_DISARM = 4000;
static constexpr int TIME_FOR_CONFIRM_ARM = 2000;
static constexpr int CLASH_TRESHOLD = 0x0A; // min 0x00 (0) max 0x1F (31) 5 bits, middle 0x0F (15)

#define FILTER_SENSOR_ITEMS 20

class CoreSensor
{
public:
  // Costructor
  CoreSensor();
  // Init
  void init();
  // Process loop
  void loop(bool& rNeedSwing, bool& rNeedClash, Status& rStatus, bool& rVerticalPosition, bool& rNeedArm,
            bool& rHorizontalPosition, bool& rNeedDisarm);
  // Interrupt status for clash
  uint8_t int1Status;
  // Get Interrupt Pin
  int getInt1Pin();

private:
  FilterSensor filterSensorData = {0, 0.0, 0.0};
  LSM6DS3 device;

  unsigned long time;

  uint8_t errorAccumulator = 0;
  uint8_t dataToWrite = 0;

  float valueAccel = 0.0;
  float valueGyro = 0.0;
  static constexpr float minValue = VERTICAL_ACC - TOLERANCE_ARM;
  static constexpr float maxValue = VERTICAL_ACC + TOLERANCE_ARM;
  static constexpr float minArmValue = VERTICAL_ARM - TOLERANCE_ARM;
  static constexpr float maxArmValue = VERTICAL_ARM + TOLERANCE_ARM;
  static constexpr float minHValue = HORIZONTAL_ACC - TOLERANCE_DISARM;
  static constexpr float maxHValue = HORIZONTAL_ACC + TOLERANCE_DISARM;

  float gyroAvg = 0.0;
  // float max2 = 0.0;

  Status status = Status::disarmed;

  bool isVerticalPosition = false;
  bool isHorizontalPosition = false;
  bool lastIsVerticalPosition = false;
  bool lastIsHorizontalPosition = false;

  bool needSwing(); // Get if need exec swing event
  bool needClash(); // Get if need exec clash event
  bool containVertical(float pValue);
  bool containHorizontal(float pValue);
  bool containArm(float pValue);
  bool needArm();
  bool needDisarm();
  void updateAverageHorizontalData();
};
