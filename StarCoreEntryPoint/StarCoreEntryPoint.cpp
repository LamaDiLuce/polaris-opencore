#include "Arduino.h"
#include "StarCoreEntryPoint.h"

//Costructor
StarCoreEntryPoint::StarCoreEntryPoint()
{
}

/*
 * Public Methods
 */

//Init
void StarCoreEntryPoint::init(bool pDebug, String pBuild, String pSerial)
{
    debugMode = pDebug;
    build = pBuild;
    serial = pSerial;

    Serial.begin(BAUD_RATE); //It needs for PC communication services

    logger.init(debugMode);

    logger.writeParamString("Build", build);
    logger.writeParamString("Serial Number", serial);

    audioModule.init(debugMode);
    sensorModule.init(debugMode);
    ledModule.init(debugMode);
    rechargeModule.init(debugMode);
}

//Process loop
void StarCoreEntryPoint::loop()
{
    sensorModule.loop(needSwingEvent, needClashEvent, status, verticalPosition,
                      needArmEvent, horizontalPosition, needDisarmEvent);
    audioModule.loop(needSwingEvent, needClashEvent, status, needArmEvent, needDisarmEvent);
    rechargeModule.loop(status, needBlinkRechargeEvent);
    ledModule.loop(needSwingEvent, needClashEvent, status,
                   needArmEvent, needDisarmEvent, needBlinkRechargeEvent);

    releaseStatus();

    checkSerials();
}

void StarCoreEntryPoint::releaseStatus()
{
    needSwingEvent = false;
    needClashEvent = false;
    verticalPosition = false;
    needArmEvent = false;
}

int StarCoreEntryPoint::getInt1Pin()
{
    return sensorModule.getInt1Pin();
}

void StarCoreEntryPoint::incrementInt1ISR()
{
    sensorModule.int1Status++;
}

void StarCoreEntryPoint::checkSerials()
{
    int incomingByte;

    if (Serial.available() > 0)
    {
        
        incomingByte = Serial.read();
        //Serial.print(String(incomingByte);
        if (incomingByte == (byte)STX)
        {
            //audioModule.beep();
            incomingMessage = "";
        }
        else if (incomingByte == (byte)ETX)
        {
            
            processIncomingMessage(incomingMessage);
            incomingMessage = "";
        }
        else
        {
            incomingMessage += (char)incomingByte;
        }
    }
}

void StarCoreEntryPoint::processIncomingMessage(String pIncomingMessage)
{
    if (pIncomingMessage == "V")
    {
        Serial.write(STX);
        Serial.print("Build: ");
        Serial.println(build);
        Serial.print("Serial Number: ");
        Serial.println(serial);
        Serial.write(ETX);
    }
}