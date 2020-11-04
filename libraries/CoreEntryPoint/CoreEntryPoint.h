#pragma once

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
#include "CoreSensor.h"
#include "CoreSettings.h"
#include "CoreImu.h"
#include "CoreMotion.h"

class CoreEntryPoint
{
public:
  // Costructor
  CoreEntryPoint();
  // Init
  void init(String build);
  // Process loop
  void loop();
  // Support
  void incrementInt1ISR();
  // Get Interrupt Pin
  int getInt1Pin();
  // Reset status events
  void releaseStatus();

private:
  String incomingMessage;
  CoreAudio audioModule;
  CoreImu imuModule;
  CoreMotion motionModule;
  CoreSensor sensorModule;
  CoreLed ledModule;
  CoreRecharge rechargeModule;
  CoreComms commsModule;
  CoreSettings settingsModule;
  Requests request;
  void updateMeasurements(int idx, int v, int up);

  // STATUS
  bool needClashEvent;
  bool needSwingEvent;
  Status status;
  bool verticalPosition;
  bool horizontalPosition;
  bool needArmEvent;
  bool needDisarmEvent;
  NeedBlinkRecharge needBlinkRechargeEvent;
};
