#include "Arduino.h"
#include "StarCoreSensor.h"

//Costructor
StarCoreSensor::StarCoreSensor()
{
}

/*
 * Public Methods
 */

//Init
void StarCoreSensor::init(bool pDebug)
{
    debugMode = pDebug;
    logger.init(debugMode);

    pinMode(SDO_PIN, OUTPUT);
    digitalWrite(SDO_PIN, HIGH);

    Wire.setSDA(18);
    Wire.setSCL(19);

    core.init(I2C_MODE, 0x6B);

    logger.write("Init device: ");
    device.begin();
    logger.writeLine("OK");

    logger.write("Init core: ");
    if (core.beginCore() != 0)
    {
        logger.writeLine("Error");
    }
    else
    {
        dataToWrite = 0;
        dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
        dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
        dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;

        errorAccumulator += core.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);
        errorAccumulator += core.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
        dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);

        errorAccumulator += core.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x0E);
        errorAccumulator += core.writeRegister(LSM6DS3_ACC_GYRO_TAP_THS_6D, CLASH_TRESHOLD); //min 0x00 (0) max 0x1F (31) 5 bits, middle 0x0F (15)
        errorAccumulator += core.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x7F);
        errorAccumulator += core.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80);
        errorAccumulator += core.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x48);

        if (errorAccumulator)
        {
            logger.writeLine("Configuration problem");
        }
        else
        {
            logger.writeLine("OK");
        }
    }

    pinMode(IMU_INT1_PIN, INPUT);
}

//Get if need exec swing event
bool StarCoreSensor::needSwing()
{
    gyroAvg = abs((device.readFloatGyroX() + device.readFloatGyroY() + device.readFloatGyroZ()) / 3.0);
    //max1 = max(abs(device.readFloatGyroX()), abs(device.readFloatGyroY()));
    //max2 = max(abs(device.readFloatGyroZ()), max1);
    logger.writeParamFloat("AVG for SWING", gyroAvg);
    return gyroAvg > SWING_THRESHOLD;
}

//Get if need exec clash event
bool StarCoreSensor::needClash()
{
    bool needClashEvent = false;

    if (int1Status > 0)
    {
        //delay(300);
        needClashEvent = (int1Status == 1);

        int1Status = 0;
    }

    return needClashEvent;
}

int StarCoreSensor::getInt1Pin()
{
    return IMU_INT1_PIN;
}

bool StarCoreSensor::containVertical(float pValue)
{
    /*
    logger.write(minValue);
    logger.write("=>");
    logger.write(pValue);
    logger.write("<=");
    logger.writeLine(maxValue);
    */
    //logger.writeParamFloat("VERTICAL ACC Z", pValue);
    //int checkValue = (int)pValue;
    return ((pValue >= minValue) && (pValue <= maxValue));
    //return ((checkValue >= minValue) && (checkValue <= maxValue));
}

bool StarCoreSensor::containArm(float pValue)
{
    /*
    logger.write(minValue);
    logger.write("=>");
    logger.write(pValue);
    logger.write("<=");
    logger.writeLine(maxValue);
    */
    return ((pValue >= minArmValue) && (pValue <= maxArmValue));
}

bool StarCoreSensor::containHorizontal(float pValue)
{

    logger.write("containHorizontal: ");
    logger.write(minHValue);
    logger.write("=>");
    logger.write(pValue);
    logger.write("<=");
    logger.writeLine(maxHValue);

    return ((pValue >= minHValue) && (pValue <= maxHValue));
}

bool StarCoreSensor::needArm()
{
    if ((status == Status::disarmed) ||
        ((status == Status::disarmedInRecharge) && debugMode))
    {
        if ((!lastIsVerticalPosition) && (isVerticalPosition))
        {
            time = millis();
            logger.writeLine("Start vertical position...");
        }

        if ((lastIsVerticalPosition) && (isVerticalPosition))
        {
            if (millis() - time > TIME_FOR_ARM)
            {
                logger.writeLine("Waiting arm...");
                status = Status::waitArm;
                //logger.writeParamStatus(status);
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
                logger.writeLine("Request arm...");
                //delay(100);
                isVerticalPosition = false;
                lastIsVerticalPosition = false;
                return true;
            }
        }
        else
        {
            //Starting change color mode
            status = Status::waitArmWithChangeColor;
            logger.writeLine("Waiting arm with change color...");
            time = millis();
            //status = Status::disarmed;
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
                logger.writeLine("Request arm and change color...");
                //delay(100);
                status = Status::armingWithChangeColor;
                isVerticalPosition = false;
                lastIsVerticalPosition = false;
                return true;
            }
        }
        else
        {
            //Starting AGAIN change color mode with next colorset
            status = Status::waitArmWithChangeColorNext;
            logger.writeLine("Waiting arm with change next color...");
            time = millis();
            //status = Status::disarmed;
            lastIsVerticalPosition = isVerticalPosition;
            return false;
        }
    }

    //logger.writeParamStatus(status);

    lastIsVerticalPosition = isVerticalPosition;
    return false;
}

bool StarCoreSensor::needDisarm()
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

Status StarCoreSensor::getStatus()
{
    return status;
}

void StarCoreSensor::updateAverageHorizontalData()
{
    if (filterSensorData.readingCount == FILTER_SENSOR_ITEMS)
    {
        //valueAccel = filterSensorData.progressAverageValue;
        //filterSensorData.previousAverageValue = valueAccel;
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
            filterSensorData.progressAverageValue =
                (filterSensorData.progressAverageValue + data) / 2.0;
        }

        logger.writeParamFloat("ReadingCount", filterSensorData.readingCount);
        logger.writeParamFloat("Accel", data);
        logger.writeParamFloat("progressAverageValue", filterSensorData.progressAverageValue);
        logger.writeParamFloat("previousAverageValue", filterSensorData.previousAverageValue);
    }
}

//Process loop
void StarCoreSensor::loop(bool &rNeedSwing, bool &rNeedClash, Status &rStatus,
                          bool &rVerticalPosition, bool &rNeedArm, bool &rHorizontalPosition,
                          bool &rNeedDisarm)
{
    status = rStatus;

    if (status == Status::armed)
    {
        //valueAccel = PROTOTYPE ? device.readFloatAccelX() : device.readFloatAccelZ();
        updateAverageHorizontalData();
        isHorizontalPosition = containHorizontal(filterSensorData.previousAverageValue);
        rHorizontalPosition = isHorizontalPosition;

        rNeedDisarm = needDisarm();

        rNeedSwing = needSwing();
        rNeedClash = needClash();
    }
    else if ((status == Status::disarmed) ||
             (status == Status::waitArm) ||
             (status == Status::waitArmWithChangeColor) ||
             (status == Status::waitArmWithChangeColorNext) ||
             ((status == Status::disarmedInRecharge) && debugMode))
    {
        if ((status == Status::disarmed) ||
            ((status == Status::disarmedInRecharge) && debugMode))
        {
            valueAccel = PROTOTYPE ? device.readFloatAccelX() : device.readFloatAccelZ();
            rVerticalPosition = containVertical(valueAccel);
            //delay(10);
            isVerticalPosition = rVerticalPosition;
        }
        rNeedArm = needArm();
    }
    else if (status == Status::disarmedInRecharge)
    {
    }

    rStatus = status;

    //logger.writeParamFloat("Accel Z", PROTOTYPE ? device.readFloatAccelX() : device.readFloatAccelZ());

    //logger.writeParamStatus(status);
}