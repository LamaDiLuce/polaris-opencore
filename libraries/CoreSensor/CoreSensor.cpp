#include "CoreSensor.h"

// Costructor
CoreSensor::CoreSensor()
{
}
/*
 * Public Methods
 */

// Init
void CoreSensor::init()
{

  pinMode(SDO_PIN, OUTPUT);
  digitalWrite(SDO_PIN, HIGH);

  Wire.setSDA(18);
  Wire.setSCL(19);

  CoreLogging::writeLine("CoreSensor: Init sensor device: ");
  if (device.begin() != 0)
  {
    CoreLogging::writeLine("CoreSensor: Sensor initialization error");
  }
  else
  {
    dataToWrite = 0;
    dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;

    errorAccumulator += device.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);
    errorAccumulator += device.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
    dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);

    errorAccumulator += device.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x0E);
    errorAccumulator += device.writeRegister(LSM6DS3_ACC_GYRO_TAP_THS_6D,
                                             CLASH_TRESHOLD); // min 0x00 (0) max 0x1F (31) 5 bits, middle 0x0F (15)
    errorAccumulator += device.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x7F);
    errorAccumulator += device.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80);
    errorAccumulator += device.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x48);

    if (errorAccumulator)
    {
      CoreLogging::writeLine("CoreSensor: Sensor configuration problem");
    }
    else
    {
      CoreLogging::writeLine("CoreSensor: Sensor initialization OK");
    }
  }

  pinMode(IMU_INT1_PIN, INPUT);
}

// Get if need exec swing event
bool CoreSensor::needSwing()
{
  gyroAvg = abs((device.readFloatGyroX() + device.readFloatGyroY() + device.readFloatGyroZ()) / 3.0);
  CoreLogging::writeLine("CoreSensor: Average gyro measurement for swing: %f", gyroAvg);
  return gyroAvg > SWING_THRESHOLD;
}

// Get if need exec clash event
bool CoreSensor::needClash()
{
  bool needClashEvent = false;

  if (int1Status > 0)
  {
    needClashEvent = (int1Status == 1);

    int1Status = 0;
  }

  return needClashEvent;
}

int CoreSensor::getInt1Pin()
{
  return IMU_INT1_PIN;
}

bool CoreSensor::containVertical(float pValue)
{
  return ((pValue >= minValue) && (pValue <= maxValue));
}

bool CoreSensor::containArm(float pValue)
{
  return ((pValue >= minArmValue) && (pValue <= maxArmValue));
}

bool CoreSensor::containHorizontal(float pValue)
{
  return ((pValue >= minHValue) && (pValue <= maxHValue));
}

bool CoreSensor::needArm()
{
  if ((status == Status::disarmed) || ((status == Status::disarmedInRecharge) && DEBUG))
  {
    if ((!lastIsVerticalPosition) && (isVerticalPosition))
    {
      time = millis();
      CoreLogging::writeLine("CoreSensor: Start vertical position...");
    }
    if ((lastIsVerticalPosition) && (isVerticalPosition))
    {
      valueGyroZ = abs(PROTOTYPE ? device.readFloatGyroX() : device.readFloatGyroZ());
      valueGyroX = abs(PROTOTYPE ? device.readFloatGyroY() : device.readFloatGyroX());
      valueGyroY = abs(PROTOTYPE ? device.readFloatGyroZ() : device.readFloatGyroY());
      if ((millis() - time > TIME_FOR_ARM) && 
           (valueGyroZ > ARM_THRESHOLD_Z) &&
           (valueGyroX < ARM_THRESHOLD_XY) &&
           (valueGyroY < ARM_THRESHOLD_XY))
      {
        CoreLogging::writeLine("CoreSensor: Waiting arm...");
        status = Status::waitArm;
        time = millis();  
      }
    }
  }
  else if (status == Status::waitArm)
  {
    if (millis() - time < TIME_FOR_CONFIRM_ARM)
    {
      valueAccel = PROTOTYPE ? device.readFloatAccelX() : device.readFloatAccelZ();

      if (containArm(valueAccel))
      {
        CoreLogging::writeLine("CoreSensor: Request arm...");
        isVerticalPosition = false;
        lastIsVerticalPosition = false;
        return true;
      }
    }
    else
    {
      // Starting change color mode
      status = Status::waitArmWithChangeColor;
      CoreLogging::writeLine("CoreSensor: Waiting arm with change color...");
      time = millis();
      lastIsVerticalPosition = isVerticalPosition;
      return false;
    }
  }
  else if (status == Status::waitArmWithChangeColor)
  {
    if (millis() - time < TIME_FOR_CONFIRM_ARM)
    {
      valueAccel = PROTOTYPE ? device.readFloatAccelX() : device.readFloatAccelZ();
      if (containArm(valueAccel))
      {
        CoreLogging::writeLine("CoreSensor: Request arm and change color...");
        status = Status::armingWithChangeColor;
        isVerticalPosition = false;
        lastIsVerticalPosition = false;
        return true;
      }
    }
    else
    {
      // Starting AGAIN change color mode with next colorset
      status = Status::waitArmWithChangeColorNext;
      CoreLogging::writeLine("CoreSensor: Waiting arm with change next color...");
      time = millis();

      lastIsVerticalPosition = isVerticalPosition;
      return false;
    }
  }
  lastIsVerticalPosition = isVerticalPosition;
  return false;
}

bool CoreSensor::needDisarm()
{
  if (status == Status::armed)
  {
    if ((!lastIsHorizontalPosition) && (isHorizontalPosition))
    {
      time = millis();
    }

    if ((lastIsHorizontalPosition) && (isHorizontalPosition))
    {
      if (millis() - time > TIME_FOR_DISARM)
      {
        delay(100);
        isHorizontalPosition = false;
        lastIsHorizontalPosition = false;
        return true;
      }
    }
  }

  lastIsHorizontalPosition = isHorizontalPosition;
  return false;
}

void CoreSensor::updateAverageHorizontalData()
{
  if (filterSensorData.readingCount == FILTER_SENSOR_ITEMS)
  {
    filterSensorData.previousAverageValue = filterSensorData.progressAverageValue;

    filterSensorData.readingCount = 0;
    filterSensorData.progressAverageValue = 0;
  }
  else
  {
    float data = PROTOTYPE ? device.readFloatAccelX() : device.readFloatAccelZ();

    if (filterSensorData.readingCount == 0)
    {
      filterSensorData.readingCount++;
      filterSensorData.progressAverageValue = data;
    }
    else
    {
      filterSensorData.readingCount++;
      filterSensorData.progressAverageValue = (filterSensorData.progressAverageValue + data) / 2.0;
    }

    CoreLogging::writeLine("CoreSensor: ReadingCount: %d", filterSensorData.readingCount);
    CoreLogging::writeLine("CoreSensor: Accel: %f", data);
    CoreLogging::writeLine("CoreSensor: progressAverageValue: %f", filterSensorData.progressAverageValue);
    CoreLogging::writeLine("CoreSensor: previousAverageValue: %f", filterSensorData.previousAverageValue);
  }
}

// Process loop
void CoreSensor::loop(bool& rNeedSwing, bool& rNeedClash, Status& rStatus, bool& rVerticalPosition, bool& rNeedArm,
                      bool& rHorizontalPosition, bool& rNeedDisarm)
{
  status = rStatus;

  if (status == Status::armed)
  {
    updateAverageHorizontalData();
    isHorizontalPosition = containHorizontal(filterSensorData.previousAverageValue);
    rHorizontalPosition = isHorizontalPosition;

    rNeedDisarm = needDisarm();

    rNeedSwing = needSwing();
    rNeedClash = needClash();
  }
  else if ((status == Status::disarmed) || (status == Status::waitArm) || (status == Status::waitArmWithChangeColor) ||
           (status == Status::waitArmWithChangeColorNext) || ((status == Status::disarmedInRecharge) && DEBUG))
  {
    if ((status == Status::disarmed) || ((status == Status::disarmedInRecharge) && DEBUG))
    {
      valueAccel = PROTOTYPE ? device.readFloatAccelX() : device.readFloatAccelZ();
      rVerticalPosition = containVertical(valueAccel);
      isVerticalPosition = rVerticalPosition;
    }
    rNeedArm = needArm();
  }
  else if (status == Status::disarmedInRecharge)
  {
  }

  rStatus = status;
}