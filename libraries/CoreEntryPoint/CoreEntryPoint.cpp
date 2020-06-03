#include "Arduino.h"
#include "CoreEntryPoint.h"

//Costructor
CoreEntryPoint::CoreEntryPoint()
{
}

/*
 * Public Methods
 */

//Init
void CoreEntryPoint::init(bool pDebug, String pBuild, String pSerial)
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
void CoreEntryPoint::loop()
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

void CoreEntryPoint::releaseStatus()
{
    needSwingEvent = false;
    needClashEvent = false;
    verticalPosition = false;
    needArmEvent = false;
}

int CoreEntryPoint::getInt1Pin()
{
    return sensorModule.getInt1Pin();
}

void CoreEntryPoint::incrementInt1ISR()
{
    sensorModule.int1Status++;
}

void CoreEntryPoint::checkSerials()
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

void CoreEntryPoint::processIncomingMessage(String pIncomingMessage)
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