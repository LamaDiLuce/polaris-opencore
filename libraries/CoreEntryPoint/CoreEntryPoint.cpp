#include "CoreEntryPoint.h"

// Costructor
CoreEntryPoint::CoreEntryPoint()
{
}

// Init
void CoreEntryPoint::init(String pBuild)
{
  // init serial IF

  commsModule.init(pBuild);
  audioModule.trace(Serial);
  audioModule.begin();

  settingsModule.init();
  sensorModule.init();

  CoreSettings* setPtr;
  setPtr = &settingsModule;
  ledModule.init(setPtr);
  rechargeModule.init();

  CoreLed* ledPtr;
  ledPtr = &ledModule;
  commsModule.setModule(ledPtr, setPtr);
  status = Status::disarmed;
}

// Process loop
void CoreEntryPoint::loop()
{
  sensorModule.loop(needSwingEvent, needClashEvent, status, verticalPosition, needArmEvent, horizontalPosition,
                    needDisarmEvent);
  audioModule.cycle();
  if (needArmEvent)
  {
    audioModule.trigger(audioModule.EVT_ARM);
  }
  if (needDisarmEvent)
  {
    audioModule.trigger(audioModule.EVT_DISARM);
  }
  if (needClashEvent)
  {
    audioModule.trigger(audioModule.EVT_CLASH);
  }
  if (needSwingEvent)
  {
    audioModule.trigger(audioModule.EVT_SWING);
  }
  if (audioModule.state() == audioModule.ARMED)
  {
    status=Status::armed;
  }
  if (audioModule.state() == audioModule.DISARM)
  {
    status=Status::disarmed;
  }  

  //audioModule.loop(needSwingEvent, needClashEvent, status, needArmEvent, needDisarmEvent);
  rechargeModule.loop(status, needBlinkRechargeEvent);

  ledModule.loop(needSwingEvent, needClashEvent, status, needArmEvent, needDisarmEvent, needBlinkRechargeEvent);
  releaseStatus();

  commsModule.loop();

  // if we are out of needBlinkRechargeEvent and communication mode is not normal then save the settings
  if ((status != Status::disarmedInRecharge) && (commsModule.getMode() != MODE_NORMAL))
  {
    settingsModule.saveToStore();
    commsModule.setMode(MODE_NORMAL);
    ledModule.changeColor({0,0,0,0}); //in case preview has been turned on
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
