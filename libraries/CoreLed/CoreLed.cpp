#include "CoreLed.h"

CoreLed::CoreLed()
{
}

void CoreLed::init()
{
  //set up temporary static colors
  //temporary to be moved to settings module and only used if no settings file or reset
  //turning the saber off will loose use amended settings until we have a non-volatile settings
  loadDefaultColors();
  //temporary 
  
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_WHITE, OUTPUT);

  getCurrentColorSet();

  turnOff();
}
//these functions to be implmented in non-volatile settings module
void CoreLed::loadDefaultColors()
{
  colorSet[RED]    = {255, 0, 0, 0};      //0 RED
  colorSet[GREEN]  = {0, 255, 0, 0};      //1 GREEN
  colorSet[BLUE]   = {0, 0, 255, 0};      //2 BLUE
  colorSet[YELLOW] = {100, 255, 0, 60};   //3 YELLOW
  colorSet[ACQUA]  = {0, 255, 240, 80};   //4 ACQUA
  colorSet[PURPLE] = {35, 10, 255, 10};   //5 PURPLE
  colorSet[ORANGE] = {150, 255, 0, 20};   //6 ORANGE
  colorSet[WHITE]  = {25, 170, 150, 255}; //7 WHITE
  colorSet[OFF]= {0, 0, 0, 0};            //8 OFF

  clashSet[RED] = {25, 170, 150, 255}; 
  clashSet[GREEN] = {25, 170, 150, 255}; 
  clashSet[BLUE] = {25, 170, 150, 255}; 
  clashSet[YELLOW] = {25, 170, 150, 255}; 
  clashSet[ACQUA] = {25, 170, 150, 255}; 
  clashSet[PURPLE] = {25, 170, 150, 255}; 
  clashSet[ORANGE] = {25, 170, 150, 255}; 
  clashSet[WHITE] = {0, 255, 240, 80};    //WHITE flashes ACQUA 
  clashSet[OFF] = {0, 0, 0, 0};
}
int CoreLed::getCurrentColorSetId()
{ if (EEPROM.read(0) == CHECK_VALUE)
  {
    return EEPROM.read(REG_COLORSET);
  }
  else
  { setCurrentColorSet(BLUE);
    return BLUE;
  }
}
ColorLed CoreLed::getMainColor(int bank) const
{ return colorSet[bank];
}
ColorLed CoreLed::getClashColor(int bank) const
{ return clashSet[bank];
}
void CoreLed::setMainColor(int bank, ColorLed cc)
{ colorSet[bank]=cc;
}
void CoreLed::setClashColor(int bank, ColorLed cc)
{ clashSet[bank]=cc;
}

String CoreLed::decodeColorSetId(int colorSetId)
{
    String colors[9] = {"RED", "GREEN", "BLUE", "YELLOW", "ACQUA", "PURPLE", "ORANGE", "WHITE", "OFF"};

    if (colorSetId < int(sizeof(colors) / sizeof(colors[0])))
        return colors[colorSetId];

    return OFF;
}

void CoreLed::getCurrentColorSet()
{
    currentColorSetId = getCurrentColorSetId();
    currentColorSet = colorSet[currentColorSetId];
    currentChangeColorSetId = currentColorSetId;
    
    //clashColorSetId = CLASH_COLOR_FOR_NO_WHITE;
    clashColorSet = clashSet[currentColorSetId];

    CoreLogging::writeParamString("Color Set", decodeColorSetId(currentColorSetId));
}

void CoreLed::setCurrentColorSet(int colorSetId)
{
    CoreLogging::writeLine("Saving colorset...");
    EEPROM.write(REG_COLORSET, colorSetId);
    EEPROM.write(REG_CHECK, CHECK_VALUE);

    getCurrentColorSet();
}

void CoreLed::changeColor(int colorSetId)
{
  changeColor(colorSet[colorSetId]);
  if (colorSetId != OFF)
  {
    CoreLogging::write("Change color: ");
    CoreLogging::writeLine(decodeColorSetId(colorSetId));
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
        CoreLogging::writeLine("Blink");
        changeColor(WHITE);
        delay(TIME_BLINK_WAITARM);
        turnOff();
    }
}

void CoreLed::displayChargeSecuence(){
    changeColor(RED);
    delay(TIME_CHARGE_SECUENCE_BLINK);
    changeColor (GREEN);
    delay(TIME_CHARGE_SECUENCE_BLINK);
    changeColor (BLUE);
    delay(TIME_CHARGE_SECUENCE_BLINK);
    changeColor (WHITE);
    delay(TIME_CHARGE_SECUENCE_BLINK);
    turnOff();
}

void CoreLed::changeColorBlink()
{
    if (!alreadyBlinked)
    {
        CoreLogging::writeLine("Blink change color");
        changeColor(currentChangeColorSetId);
        delay(TIME_BLINK_WAITARM_WITH_COLOR);
        turnOff();
    }
}

void CoreLed::fadeOut()
{
    CoreLogging::writeLine("fadeOut");

    singleStepColorSet.red = currentColorSet.red / FADE_DELAY;
    singleStepColorSet.green = currentColorSet.green / FADE_DELAY;
    singleStepColorSet.blue = currentColorSet.blue / FADE_DELAY;
    singleStepColorSet.white = currentColorSet.white / FADE_DELAY;
    for (int i = 0; i <= FADE_DELAY; i++)
    {
        setGradientColor(gradientColorSet.red - singleStepColorSet.red,
                         gradientColorSet.green - singleStepColorSet.green,
                         gradientColorSet.blue - singleStepColorSet.blue,
                         gradientColorSet.white - singleStepColorSet.white);
        delay(1000 / FADE_DELAY);
    }

    turnOff();
}

void CoreLed::fadeIn()
{
    CoreLogging::writeLine("fadeIn");

    setGradientColor(0, 0, 0, 0);
    singleStepColorSet.red = currentColorSet.red / FADE_DELAY;
    singleStepColorSet.green = currentColorSet.green / FADE_DELAY;
    singleStepColorSet.blue = currentColorSet.blue / FADE_DELAY;
    singleStepColorSet.white = currentColorSet.white / FADE_DELAY;
    for (int i = 0; i <= FADE_DELAY; i++)
    {
        setGradientColor(gradientColorSet.red + singleStepColorSet.red,
                         gradientColorSet.green + singleStepColorSet.green,
                         gradientColorSet.blue + singleStepColorSet.blue,
                         gradientColorSet.white + singleStepColorSet.white);
        delay(250 / FADE_DELAY);
    }

    changeColor(currentColorSetId);
}

void CoreLed::clash()
{
    CoreLogging::writeLine("Clash:");
    CoreLogging::writeParamString("Color Set", decodeColorSetId(currentColorSetId));
    changeColor(clashSet[currentColorSetId]);
    delay(CLASH_TIME);
    changeColor(colorSet[currentColorSetId]);
}

void CoreLed::blinkRecharge(NeedBlinkRecharge needBlinkRecharge)
{
    if (currentStatus == Status::disarmedInRecharge)
    {
        CoreLogging::writeLine("Blink recharging");
        changeColor(needBlinkRecharge.colorRecharge);
        delay(BLINK_TIME);
        changeColor(OFF);
    }
}

//Process loop
void CoreLed::loop(bool &rNeedSwing, bool &rNeedClash, Status &rStatus,
                       bool &rNeedArm, bool &rNeedDisarm, NeedBlinkRecharge &rNeedBlinkRecharge)
{
    if ((currentStatus == Status::waitArmWithChangeColorNext) &&
        (rStatus == Status::waitArmWithChangeColor))
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

    if (rNeedClash)
    {
        clash();
        rNeedClash = false;
    }

    if (rNeedArm)
    {
        CoreLogging::writeParamString("Led", "NeedArm");
        CoreLogging::writeParamStatus(currentStatus);
        if (currentStatus == Status::armingWithChangeColor)
        {
            setCurrentColorSet(currentChangeColorSetId);
        }
        else
        { //refresh actual color in case it has been changed via serial port command
          currentColorSet=colorSet[currentColorSetId];
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

    if(currentBlinkRechargeStatus.chargeSequence){
        displayChargeSecuence();
    }

    if (currentBlinkRechargeStatus.needRecharge)
    {
        blinkRecharge(currentBlinkRechargeStatus);
        currentBlinkRechargeStatus = {false, 0};
    }
}
