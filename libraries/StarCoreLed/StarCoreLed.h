#include "Arduino.h"
#include "StarCoreCommon.h"
#include <EEPROM.h>
#include "Logging.h"

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
#define CLASH_TIME 200
#define CLASH_COLOR_FOR_WHITE ACQUA
#define CLASH_COLOR_FOR_NO_WHITE WHITE

#ifndef StarCoreLed_h
#define StarCoreLed_h

class StarCoreLed
{
public:
  //Costructor
  StarCoreLed();
  //Init
  void init(bool debug);
  //Process loop
  void loop(bool &rNeedSwing, bool &rNeedClash, Status &rStatus, bool &rNeedArm,
            bool &rNeedDisarm, NeedBlinkRecharge &rNeedBlinkRecharge);
  //Tools
  void setColor(int colorSetId, bool save);
  void setGradientColor(int red, int green, int blue, int white);
  void changeColor(int colorSetId);
  void turnOff();
  void fadeOut();
  void fadeIn();
  void clash();
  void getCurrentColorSet();
  void setCurrentColorSet(int colorSetId);
  void blink();
  void changeColorBlink();
  void blinkRecharge(NeedBlinkRecharge needBlinkRecharge);

private:
  bool debugMode = false;
  Logging logger;
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
  int clashColorSetId;
  bool fadingOut = false;
  bool fadingIn = false;
  bool alreadyBlinked = false;
  NeedBlinkRecharge currentBlinkRechargeStatus = {false, 0};

  //COLORSET
  ColorLed colorSet[9] = {{255, 0, 0, 0},   //RED
                          {0, 255, 0, 0},   //GREEN
                          {0, 0, 255, 0},   //BLUE
                          {100, 255, 0, 60},  //YELLOW
                          {0, 255, 240, 80}, //ACQUA
                          {35, 10, 255, 10},  //PURPLE
                          {150, 255, 0, 20},  //ORANGE
                          {25, 170, 150, 255},   //WHITE
                          {0, 0, 0, 0}};    //OFF

  String decodeColorSetId(int colorSetId);
  int setColorDelta(int color);
};

#endif