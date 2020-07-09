#include "Arduino.h"
#include "CoreEntryPoint.h"

//Costructor
CoreEntryPoint::CoreEntryPoint()
{
}

//Init
void CoreEntryPoint::init(String pBuild)
{
    //init serial IF
    commsModule.init(pBuild);

    audioModule.init();
    sensorModule.init();
    ledModule.init();
    rechargeModule.init();

    CoreLed *ptr;
    ptr = &ledModule;
    commsModule.setModule(ptr);
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

    commsModule.loop();
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
