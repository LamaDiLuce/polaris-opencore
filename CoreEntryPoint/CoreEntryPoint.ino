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

#define BUILD "2.2.7"

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
  modulesInit();
  
  modulesConnections();

  attachInterrupt(digitalPinToInterrupt(imuModule.getInt1Pin()), int1ISR, RISING);
  
  if (RCM_SRS0 & RCM_SRS0_WDOG)
  {
    // FW reset due to watchdog timeout, probably issue with the speaker and audio module
    recovery();
  }
  else
  {
    audioModule.beep(100, 0.1);
  }

  initWatchdog();
}

void loop()
{
  refreshWatchdog();

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

void modulesInit()
{
  commsModule.init(BUILD);

  CoreSettings* setPtr;
  setPtr = &settingsModule;

  audioModule.trace(Serial);
  audioModule.begin(setPtr);

  motionModule.trace(Serial);
  motionModule.begin();

  settingsModule.init();
  
  commsModule.setModule(setPtr);

  // ledModule.trace(Serial);
  ledModule.begin(setPtr);
  
  //imuModule.trace(Serial);
  imuModule.begin();
}

void modulesConnections()
{
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
  audioModule.setAngDotProduct(imuModule.getAngDotProduct());  
  ledModule.setAccelX(imuModule.getAccelX());
  ledModule.setAccelY(imuModule.getAccelY());
  ledModule.setAccelZ(imuModule.getAccelZ());
}

void initWatchdog()
{
// Setup WDT
  noInterrupts();                                         // don't allow interrupts while setting up WDOG
  WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;                         // unlock access to WDOG registers
  WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;
  delayMicroseconds(1);

  // 4 seconds
  WDOG_TOVALH = 0x01b7;
  WDOG_TOVALL = 0x7400;

  // This sets prescale clock so that the watchdog timer ticks at 7.2MHz
  WDOG_PRESC  = 0x400;

  // Set options to enable WDT. You must always do this as a SINGLE write to WDOG_CTRLH
  WDOG_STCTRLH |= WDOG_STCTRLH_ALLOWUPDATE |
      WDOG_STCTRLH_WDOGEN | WDOG_STCTRLH_WAITEN |
      WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_CLKSRC;
  interrupts();

}

void refreshWatchdog()
{
  noInterrupts();
  WDOG_REFRESH = 0xA602;
  WDOG_REFRESH = 0xB480;
  interrupts();
}

void recovery()
{
  // we want to go back in ARMED status with all modules, but with audio module in MUTE state
  // a safe way is to perform the arming procedure with all the modules

  // first cycles of all modules (IDLE state)
  imuModule.cycle();
  motionModule.cycle();
  audioModule.cycle();
  ledModule.cycle();

  // trigger ARM event
  motionModule.trigger(motionModule.EVT_ARM);
  motionModule.cycle();
  audioModule.cycle();
  ledModule.cycle();

  // trigger MUTE event and cycle Motion module two times to go back in ARM
  motionModule.trigger(motionModule.EVT_MUTE);
  motionModule.cycle();
  audioModule.cycle();
  motionModule.cycle();

  // trigger ARMED event
  motionModule.trigger(motionModule.EVT_ARMED);
  motionModule.cycle();
  audioModule.cycle();
  ledModule.cycle();
}
