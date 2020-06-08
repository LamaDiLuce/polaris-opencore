#include "Arduino.h"
#include "CoreLed.h"

CoreLed::CoreLed()
{
}

void CoreLed::init(bool pDebug)
{
    debugMode = pDebug;
    logger.init(debugMode);

    pinMode(PIN_RED, OUTPUT);
    pinMode(PIN_GREEN, OUTPUT);
    pinMode(PIN_BLUE, OUTPUT);
    pinMode(PIN_WHITE, OUTPUT);

    /*
    if (debugMode)
    {
        logger.writeLine("Test led:");
        for (int i = 0; i < COLORS; i++)
        {
            logger.writeLine(decodeColorSetId(i));
            setColor(i, false);
            delay(1000);
        }
    }
    */
    getCurrentColorSet();

    turnOff();
}

String CoreLed::decodeColorSetId(int colorSetId)
{
    String colors[9] = {"RED", "GREEN", "BLUE", "YELLOW", "ACQUA", "PURPLE", "ORANGE", "WHITE", "OFF"};

    if (colorSetId <= (int) sizeof(colors))
        return colors[colorSetId];

    return OFF;
}

void CoreLed::getCurrentColorSet()
{
    if (EEPROM.read(0) == CHECK_VALUE)
    {
        currentColorSetId = EEPROM.read(REG_COLORSET);
    }
    else
    {
        currentColorSetId = BLUE;
        setCurrentColorSet(currentColorSetId);
    }
    currentColorSet = colorSet[currentColorSetId];
    currentChangeColorSetId = currentColorSetId;

    if (currentColorSetId == WHITE)
    {
        clashColorSetId = CLASH_COLOR_FOR_WHITE;
    }
    else
    {
        clashColorSetId = CLASH_COLOR_FOR_NO_WHITE;
    }
    clashColorSet = colorSet[clashColorSetId];

    logger.writeParamString("Color Set", decodeColorSetId(currentColorSetId));
}

void CoreLed::setCurrentColorSet(int colorSetId)
{
    logger.writeLine("Saving colorset...");
    //logger.write(decodeColorSetId(colorSetId));
    EEPROM.write(REG_COLORSET, colorSetId);
    EEPROM.write(REG_CHECK, CHECK_VALUE);

    getCurrentColorSet();

    //logger.writeLine(": OK");
}

void CoreLed::changeColor(int colorSetId)
{
    analogWrite(PIN_RED, !COMMON_GND ? colorSet[colorSetId].red : 255 - colorSet[colorSetId].red);
    analogWrite(PIN_GREEN, !COMMON_GND ? colorSet[colorSetId].green : 255 - colorSet[colorSetId].green);
    analogWrite(PIN_BLUE, !COMMON_GND ? colorSet[colorSetId].blue : 255 - colorSet[colorSetId].blue);
    analogWrite(PIN_WHITE, !COMMON_GND ? colorSet[colorSetId].white : 255 - colorSet[colorSetId].white);

    if (colorSetId != OFF)
    {
        logger.write("Change color: ");
        logger.writeLine(decodeColorSetId(colorSetId));
    }
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
        logger.writeLine("Blink");
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
        logger.writeLine("Blink change color");
        changeColor(currentChangeColorSetId);
        delay(TIME_BLINK_WAITARM_WITH_COLOR);
        turnOff();
    }
}

void CoreLed::fadeOut()
{
    logger.writeLine("fadeOut");

    //changeColor(currentColorSetId);
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
    logger.writeLine("fadeIn");

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
    logger.writeLine("Clash:");
    logger.writeParamString("Color Set", decodeColorSetId(currentColorSetId));
    changeColor(clashColorSetId);
    delay(CLASH_TIME);
    changeColor(currentColorSetId);
}

void CoreLed::blinkRecharge(NeedBlinkRecharge needBlinkRecharge)
{
    if (currentStatus == Status::disarmedInRecharge)
    {
        logger.writeLine("Blink recharging");
        changeColor(needBlinkRecharge.colorRecharge);
        delay(BLINK_TIME);
        changeColor(OFF);
    }
}

//Process loop
void CoreLed::loop(bool &rNeedSwing, bool &rNeedClash, Status &rStatus,
                       bool &rNeedArm, bool &rNeedDisarm, NeedBlinkRecharge &rNeedBlinkRecharge)
{
    if ((currentStatus == waitArmWithChangeColorNext) &&
        (rStatus == waitArmWithChangeColor))
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
        logger.writeParamString("Led", "NeedArm");
        logger.writeParamStatus(currentStatus);
        if (currentStatus == Status::armingWithChangeColor)
        {
            setCurrentColorSet(currentChangeColorSetId);
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

    if(currentBlinkRechargeStatus.chargeSecuence){
        displayChargeSecuence();
    }

    if (currentBlinkRechargeStatus.needRecharge)
    {
        blinkRecharge(currentBlinkRechargeStatus);
        currentBlinkRechargeStatus = {false, 0};
    }
}