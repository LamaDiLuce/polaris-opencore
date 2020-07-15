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
    settingsModule.init();
    sensorModule.init();
    
    CoreSettings *setPtr;
    setPtr = &settingsModule;    
    ledModule.init(setPtr);
    rechargeModule.init();

    CoreLed *ledPtr;
    ledPtr = &ledModule;
    commsModule.setModule(ledPtr, setPtr);
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

    //if we are out of needBlinkRechargeEvent and communication mode is not normal then save the settings
    if((status!=Status::disarmedInRecharge) && (commsModule.getMode()!=MODE_NORMAL))
    { settingsModule.saveToStore();
      Serial.println("saved...");
      commsModule.setMode(MODE_NORMAL);
    }
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
