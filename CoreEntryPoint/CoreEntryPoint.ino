#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Automaton.h"
#include "CoreAudio.h"
#include "CoreCommon.h"
#include "CoreComms.h"
#include "CoreLed.h"
#include "CoreLogging.h"
#include "CoreSettings.h"
#include "CoreImu.h"
#include "CoreMotion.h"

#define BUILD "2.1.0"

// Modules
String incomingMessage;
CoreAudio audioModule;
CoreImu imuModule;
CoreMotion motionModule;
CoreLed ledModule;
CoreComms commsModule;
CoreSettings settingsModule;

void setup()
{
  /*   Modules Initialization   */
  commsModule.init(BUILD);

  CoreSettings* setPtr;
  setPtr = &settingsModule;

  // audioModule.trace(Serial);
  audioModule.begin(setPtr);

  // motionModule.trace(Serial);
  motionModule.begin();

  settingsModule.init();
  
  commsModule.setModule(setPtr);

  // ledModule.trace(Serial);
  ledModule.begin(setPtr);
  
  //imuModule.trace(Serial);
  imuModule.begin();

  /*   Modules Connections   */
  imuModule.onSample(updateMeasurements);                   // Callback

  motionModule.onMute(audioModule,audioModule.EVT_MUTE);    // Simple push connector for only one action

  motionModule.onArm([] (int idx, int v, int up) { // lambda function for more actions
                        ledModule.trigger(ledModule.EVT_ARM);
  });  
  motionModule.onArmed([] (int idx, int v, int up) { // lambda function for more actions
                        if (audioModule.state() == audioModule.IDLE)
                        {
                          audioModule.trigger(audioModule.EVT_ARM);
                        }
                        else
                        {
                          audioModule.trigger(audioModule.EVT_ARMED);
                        }
                        if (audioModule.checkSmoothSwing())
                        {
                          imuModule.trigger(imuModule.EVT_HIGH_FREQ_SAMPLING);
                        }
                        ledModule.trigger(ledModule.EVT_ARMED);
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
                        if (audioModule.state() != audioModule.MUTE)
                        {
                          audioModule.trigger(audioModule.EVT_DISARM);
                        }
                        ledModule.trigger(ledModule.EVT_DISARM);
                        if (audioModule.checkSmoothSwing())
                        {
                          imuModule.trigger(imuModule.EVT_START_SAMPLING);
                        }
  });
  motionModule.onIdle([] (int idx, int v, int up) { // lambda function for more actions
                        audioModule.trigger(audioModule.EVT_DISARM);
                        ledModule.trigger(ledModule.EVT_DISARM);
  });
  ledModule.onNextcolor([] (int idx, int v, int up) { // lambda function for more actions
                        if (v == 0)
                        {
                          audioModule.beep(125, 1);
                        }
  });

  attachInterrupt(digitalPinToInterrupt(imuModule.getInt1Pin()), int1ISR, RISING);

  audioModule.beep(100, 0.1);
}

void loop()
{
  imuModule.cycle();
  motionModule.cycle();
  audioModule.cycle();
  ledModule.cycle();

  commsModule.loop();

  // if the communication mode is not normal then save the settings
  if (commsModule.getMode() != MODE_NORMAL)
  {
    settingsModule.saveToStore();
    commsModule.setMode(MODE_NORMAL);
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
  motionModule.setSwingSpeed(imuModule.getSwingSpeed());
  motionModule.setRollSpeed(imuModule.getRollSpeed());
  audioModule.setSwingSpeed(imuModule.getSwingSpeed());
  audioModule.setRollSpeed(imuModule.getRollSpeed());
}
