#include "CoreLed.h"

CoreLed::CoreLed()
{
}

void CoreLed::init(CoreSettings* cSet)
{
  moduleSettings = cSet;
  moduleSettings->init();

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_WHITE, OUTPUT);

  turnOff();
}

String CoreLed::decodeColorSetId(int colorSetId)
{
  String colors[9] = {"RED", "ORANGE", "YELLOW", "GREEN", "WHITE", "ACQUA", "BLUE", "PURPLE", "OFF"};

  if (colorSetId < int(sizeof(colors) / sizeof(colors[0])))
    return colors[colorSetId];

  return OFF;
}

void CoreLed::getCurrentColorSet()
{
  currentColorSetId = moduleSettings->getActiveBank();
  currentColorSet = moduleSettings->getMainColor(currentColorSetId); // colorSet[currentColorSetId];
  currentChangeColorSetId = currentColorSetId;

  // clashColorSetId = CLASH_COLOR_FOR_NO_WHITE;
  clashColorSet = moduleSettings->getClashColor(currentColorSetId); // clashSet[currentColorSetId];

  CoreLogging::writeLine("CoreLed: Color Set: %s", decodeColorSetId(currentColorSetId));
}

void CoreLed::setCurrentColorSet(int colorSetId)
{
  CoreLogging::writeLine("CoreLed: Setting color set...");
  moduleSettings->setActiveBank(colorSetId);
  getCurrentColorSet();
}

void CoreLed::changeColor(int colorSetId)
{
  changeColor(moduleSettings->getMainColor(colorSetId));
  if (colorSetId != OFF)
  {
    //CoreLogging::writeLine("CoreLed: Change color to %s", decodeColorSetId(colorSetId));
  }
}

void CoreLed::changeColor(const ColorLed& cLed)
{
  analogWrite(PIN_RED, !COMMON_GND ? cLed.red : 255 - cLed.red);
  analogWrite(PIN_GREEN, !COMMON_GND ? cLed.green : 255 - cLed.green);
  analogWrite(PIN_BLUE, !COMMON_GND ? cLed.blue : 255 - cLed.blue);
  analogWrite(PIN_WHITE, !COMMON_GND ? cLed.white : 255 - cLed.white);
}

int CoreLed::setColorDelta(int color)
{
  return (color < 0) ? 0 : (color > 255) ? 255 : color;
}

void CoreLed::setGradientColor(int red, int green, int blue, int white)
{
  red = setColorDelta(red);
  green = setColorDelta(green);
  blue = setColorDelta(blue);
  white = setColorDelta(white);

  analogWrite(PIN_RED, !COMMON_GND ? red : 255 - red);
  analogWrite(PIN_GREEN, !COMMON_GND ? green : 255 - green);
  analogWrite(PIN_BLUE, !COMMON_GND ? blue : 255 - blue);
  analogWrite(PIN_WHITE, !COMMON_GND ? white : 255 - white);

  gradientColorSet = {red, green, blue, white};
}

void CoreLed::turnOff()
{
  changeColor(OFF);
}

void CoreLed::blink()
{
  if (!alreadyBlinked)
  {
    CoreLogging::writeLine("CoreLed: Blink");
    changeColor(WHITE);
    delay(TIME_BLINK_WAITARM);
    turnOff();
  }
}

void CoreLed::displayChargeSecuence()
{
  changeColor(RED);
  delay(TIME_CHARGE_SECUENCE_BLINK);
  changeColor(GREEN);
  delay(TIME_CHARGE_SECUENCE_BLINK);
  changeColor(BLUE);
  delay(TIME_CHARGE_SECUENCE_BLINK);
  changeColor(WHITE);
  delay(TIME_CHARGE_SECUENCE_BLINK);
  turnOff();
}

void CoreLed::changeColorBlink()
{
  if (!alreadyBlinked)
  {
    CoreLogging::writeLine("CoreLed: Change color to blink");
    changeColor(currentChangeColorSetId);
    delay(TIME_BLINK_WAITARM_WITH_COLOR);
    turnOff();
  }
}

void CoreLed::fadeOut()
{
  CoreLogging::writeLine("CoreLed: FadeOut");

  singleStepColorSet.red = currentColorSet.red / FADE_DELAY;
  singleStepColorSet.green = currentColorSet.green / FADE_DELAY;
  singleStepColorSet.blue = currentColorSet.blue / FADE_DELAY;
  singleStepColorSet.white = currentColorSet.white / FADE_DELAY;
  for (int i = 0; i <= FADE_DELAY; i++)
  {
    setGradientColor(gradientColorSet.red - singleStepColorSet.red, gradientColorSet.green - singleStepColorSet.green,
                     gradientColorSet.blue - singleStepColorSet.blue,
                     gradientColorSet.white - singleStepColorSet.white);
    delay(1000 / FADE_DELAY);
  }

  turnOff();
}

void CoreLed::fadeIn()
{
  CoreLogging::writeLine("CoreLed: FadeIn");

  setGradientColor(0, 0, 0, 0);
  singleStepColorSet.red = currentColorSet.red / FADE_DELAY;
  singleStepColorSet.green = currentColorSet.green / FADE_DELAY;
  singleStepColorSet.blue = currentColorSet.blue / FADE_DELAY;
  singleStepColorSet.white = currentColorSet.white / FADE_DELAY;
  for (int i = 0; i <= FADE_DELAY; i++)
  {
    setGradientColor(gradientColorSet.red + singleStepColorSet.red, gradientColorSet.green + singleStepColorSet.green,
                     gradientColorSet.blue + singleStepColorSet.blue,
                     gradientColorSet.white + singleStepColorSet.white);
    delay(250 / FADE_DELAY);
  }

  changeColor(currentColorSetId);
}

void CoreLed::clash()
{
  CoreLogging::writeLine("CoreLed:Clash:");
  CoreLogging::writeLine("CoreLed: Current color set: %s", decodeColorSetId(currentColorSetId));
  changeColor(moduleSettings->getClashColor(currentColorSetId));
  delay(CLASH_TIME);
  changeColor(moduleSettings->getMainColor(currentColorSetId));
}

void CoreLed::swingOn()
{ 
  CoreLogging::writeLine("CoreLed:Swing On:");
  CoreLogging::writeLine("CoreLed: Current color set: %s", decodeColorSetId(currentColorSetId));
  changeColor(moduleSettings->getSwingColor(currentColorSetId));
  //delay(CLASH_TIME);
  //changeColor(moduleSettings->getMainColor(currentColorSetId));
}
void CoreLed::swingOff()
{ 
  CoreLogging::writeLine("CoreLed:Swing Off:");
  CoreLogging::writeLine("CoreLed: Current color set: %s", decodeColorSetId(currentColorSetId));
  changeColor(moduleSettings->getMainColor(currentColorSetId));
}

void CoreLed::blinkRecharge(NeedBlinkRecharge needBlinkRecharge)
{
  if (currentStatus == Status::disarmedInRecharge)
  {
    //CoreLogging::writeLine("CoreLed: Blink recharging");
    changeColor(needBlinkRecharge.colorRecharge);
    delay(BLINK_TIME);
    changeColor(OFF);
  }
}

// Process loop
void CoreLed::loop(bool& rNeedSwing, bool& rNeedClash, Status& rStatus, bool& rNeedArm, bool& rNeedDisarm,
                   NeedBlinkRecharge& rNeedBlinkRecharge)
{
  if ((currentStatus == Status::waitArmWithChangeColorNext) && (rStatus == Status::waitArmWithChangeColor))
  {
    alreadyBlinked = false;
  }

  currentStatus = rStatus;
  currentBlinkRechargeStatus = rNeedBlinkRecharge;

  if (currentStatus == Status::waitArm)
  {
    blink();
    alreadyBlinked = true;
  }

  if (currentStatus == Status::waitArmWithChangeColor)
  {
    changeColorBlink();
    alreadyBlinked = true;
  }

  if (currentStatus == Status::waitArmWithChangeColorNext)
  {
    alreadyBlinked = false;
    currentChangeColorSetId++;

    if (currentChangeColorSetId > COLORS)
    {
      currentChangeColorSetId = 0;
    }

    changeColorBlink();
    alreadyBlinked = true;

    currentStatus = Status::waitArmWithChangeColor;
    rStatus = currentStatus;
  }

  if (rNeedClash && currentStatus == Status::armed)
  {
    clash();
    rNeedClash = false;
  }

  if(rStatus == Status::armed)
  { if(rNeedSwing)
    { 
      swingOn();
    }
    else
    {
      swingOff();
    }
  }

  if (rNeedArm)
  {
    CoreLogging::writeLine("CoreLed: Led NeedArm");
    CoreLogging::writeLine("CoreLed: Status: %s", currentStatus);
    if (currentStatus == Status::armingWithChangeColor)
    {
      setCurrentColorSet(currentChangeColorSetId);
    }
    else
    { // refresh actual color in case it has been changed via serial port command
      getCurrentColorSet();
    }
    fadeIn();
    rNeedArm = false;
    alreadyBlinked = false;
  }

  if (rNeedDisarm)
  {
    fadeOut();
    rNeedDisarm = false;
  }

  if (currentBlinkRechargeStatus.chargeSequence)
  {
    displayChargeSecuence();
  }

  if (currentBlinkRechargeStatus.needRecharge)
  {
    blinkRecharge(currentBlinkRechargeStatus);
    currentBlinkRechargeStatus = {false, 0};
  }
}
