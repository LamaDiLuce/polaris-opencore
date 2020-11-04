#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Automaton.h"
#include "CoreAudio.h"
#include "CoreCommon.h"
#include "CoreComms.h"
#include "CoreLed.h"
#include "CoreLogging.h"
#include "CoreRecharge.h"
#include "CoreSettings.h"
#include "CoreImu.h"
#include "CoreMotion.h"

#define BUILD "1.9.18"

// Modules
String incomingMessage;
CoreAudio audioModule;
CoreImu imuModule;
CoreMotion motionModule;
CoreLed ledModule;
CoreRecharge rechargeModule;
CoreComms commsModule;
CoreSettings settingsModule;
Requests request;

// STATUS
bool needClashEvent;
bool needSwingEvent;
Status status;
bool verticalPosition;
bool horizontalPosition;
bool needArmEvent;
bool needDisarmEvent;
NeedBlinkRecharge needBlinkRechargeEvent;

void setup()
{
  commsModule.init(BUILD);

  audioModule.trace(Serial);
  audioModule.begin();
  motionModule.trace(Serial);
  motionModule.begin();
  motionModule.onArmed(audioModule,audioModule.EVT_ARM);
  motionModule.onClash(audioModule,audioModule.EVT_CLASH);
  motionModule.onSwing(audioModule,audioModule.EVT_SWING);
  motionModule.onDisarm(audioModule,audioModule.EVT_DISARM);
  motionModule.onMute(audioModule,audioModule.EVT_MUTE);
  //imuModule.trace(Serial);
  imuModule.begin();
  imuModule.onSample(updateMeasurements);

  settingsModule.init();

  CoreSettings* setPtr;
  setPtr = &settingsModule;
  ledModule.init(setPtr);
  rechargeModule.init();

  CoreLed* ledPtr;
  ledPtr = &ledModule;
  commsModule.setModule(ledPtr, setPtr);
  status = Status::disarmed;
  attachInterrupt(digitalPinToInterrupt(imuModule.getInt1Pin()), int1ISR, RISING);
}

void loop()
{
  imuModule.cycle();
  motionModule.cycle();
  audioModule.cycle();
  if (audioModule.state() == audioModule.ARMED)
  {
    status=Status::armed;
  }
  if (audioModule.state() == audioModule.DISARM)
  {
    status=Status::disarmed;
  }
  if (motionModule.state() == motionModule.EVT_SWING)
  {
    needSwingEvent=true;
  }
  else
  {
    needSwingEvent=false;
  }
  if (motionModule.state() == motionModule.EVT_CLASH)
  {
    needClashEvent=true;
  }
  else
  {
    needClashEvent=false;
  }

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

void int1ISR()
{
  motionModule.incInt1Status();
}

void updateMeasurements(int idx, int v, int up)
{
  motionModule.setGyroX(imuModule.getGyroX());
  motionModule.setGyroY(imuModule.getGyroY());
  motionModule.setGyroZ(imuModule.getGyroZ());
  motionModule.setAccelX(imuModule.getAccelX());
  motionModule.setAccelY(imuModule.getAccelY());
  motionModule.setAccelZ(imuModule.getAccelZ());
  motionModule.setGyrosAvg(imuModule.getGyrosAvg());
}

void releaseStatus()
{
  needSwingEvent = false;
  needClashEvent = false;
  verticalPosition = false;
  needArmEvent = false;
}