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
//CoreRecharge rechargeModule;
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
  /*   Modules Initialization   */
  commsModule.init(BUILD);

  audioModule.trace(Serial);
  audioModule.begin();

  motionModule.trace(Serial);
  motionModule.begin();

  ledModule.trace(Serial);
  ledModule.begin();
  
  //imuModule.trace(Serial);
  imuModule.begin();

  /*   Modules Connections   */
  imuModule.onSample(updateMeasurements);                   // Callback

  motionModule.onMute(audioModule,audioModule.EVT_MUTE);    // Simple push connector for only one action

  motionModule.onArm([] (int idx, int v, int up) { // lambda function for more actions
                        ledModule.trigger(ledModule.EVT_ARM);
  });  
  motionModule.onArmed([] (int idx, int v, int up) { // lambda function for more actions
                        ledModule.trigger(ledModule.EVT_ARMED);
                        audioModule.trigger(audioModule.EVT_ARM);
  });
  motionModule.onClash([] (int idx, int v, int up) { // lambda function for more actions
                        audioModule.trigger(audioModule.EVT_CLASH);
                        ledModule.trigger(ledModule.EVT_CLASH);
  });
  motionModule.onSwing([] (int idx, int v, int up) { // lambda function for more actions
                        audioModule.trigger(audioModule.EVT_SWING);
                        ledModule.trigger(ledModule.EVT_SWING);
  });
  motionModule.onDisarm([] (int idx, int v, int up) { // lambda function for more actions
                        audioModule.trigger(audioModule.EVT_DISARM);
                        ledModule.trigger(ledModule.EVT_DISARM);
  });

  settingsModule.init();

  CoreSettings* setPtr;
  setPtr = &settingsModule;
  //ledModule.init(setPtr);
  //rechargeModule.init();

  //CoreLed* ledPtr;
  //ledPtr = &ledModule;
  commsModule.setModule(setPtr);
  status = Status::disarmed;
  attachInterrupt(digitalPinToInterrupt(imuModule.getInt1Pin()), int1ISR, RISING);
}

void loop()
{
  imuModule.cycle();
  motionModule.cycle();
  audioModule.cycle();
  ledModule.cycle();

  commsModule.loop();

  // if we are out of needBlinkRechargeEvent and communication mode is not normal then save the settings
  if ((status != Status::disarmedInRecharge) && (commsModule.getMode() != MODE_NORMAL))
  {
    settingsModule.saveToStore();
    commsModule.setMode(MODE_NORMAL);
    //ledModule.changeColor({0,0,0,0}); //in case preview has been turned on
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
