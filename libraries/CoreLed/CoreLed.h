#pragma once

#include <Arduino.h>
#include <EEPROM.h>

#include "CoreCommon.h"
#include "CoreLogging.h"

//LED RGBW
#define PIN_RED 3
#define PIN_GREEN 4
#define PIN_BLUE 5
#define PIN_WHITE 6
#define COMMON_GND false

#define REG_CHECK 0
#define CHECK_VALUE 33
#define REG_COLORSET 1
#define FADE_DELAY 30
#define FADE_STEPS_CLASH 8
#define FADE_DELAY_CLASH 15
#define TIME_BLINK_WAITARM 200
#define TIME_BLINK_WAITARM_WITH_COLOR 500
#define TIME_CHARGE_SECUENCE_BLINK 300
#define CLASH_TIME 200

class CoreLed
{
public:
  //Constructor
  CoreLed();
  //Init
  void init();
  //Process loop
  void loop(bool &rNeedSwing, bool &rNeedClash, Status &rStatus, bool &rNeedArm,
            bool &rNeedDisarm, NeedBlinkRecharge &rNeedBlinkRecharge);
  //Tools
  void setColor(int colorSetId, bool save);
  void setGradientColor(int red, int green, int blue, int white);
  void changeColor(int colorSetId);
  void changeColor(const ColorLed& cLed);
  void turnOff();
  void fadeOut();
  void fadeIn();
  void clash();
  void getCurrentColorSet();
  void setCurrentColorSet(int colorSetId);
  void blink();
  void changeColorBlink();
  void blinkRecharge(NeedBlinkRecharge needBlinkRecharge);
  void displayChargeSecuence();

  //these functions to be implemented in non-volatile settings module
  void loadDefaultColors();
  int getCurrentColorSetId();
  ColorLed getMainColor(int bank) const;
  ColorLed getClashColor(int bank) const;
  void setMainColor(int bank, ColorLed cc);
  void setClashColor(int bank, ColorLed cc);
  
private:
  bool startedEvent = false;
  int currentColorSetId = OFF;
  int currentChangeColorSetId = OFF;
  ColorLed currentColorSet;
  ColorLed gradientColorSet;
  ColorLed singleStepColorSet;
  ColorLed singleStepClashColorSet;
  Requests currentRequest = Requests::none;
  Status currentStatus = Status::disarmed;
  ColorLed clashColorSet;
  bool fadingOut = false;
  bool fadingIn = false;
  bool alreadyBlinked = false;
  NeedBlinkRecharge currentBlinkRechargeStatus = {false, 0};

  //COLORSETS, these variables to be moved into non-volatile settings module
  ColorLed colorSet[9];
  ColorLed clashSet[9];

  String decodeColorSetId(int colorSetId);
  int setColorDelta(int color);
};
