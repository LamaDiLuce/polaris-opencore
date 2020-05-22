#include "Arduino.h"
#include "StarCoreLed.h"

//Costructor
StarCoreLed::StarCoreLed()
{
}

/*
 * Public Methods
 */

//Init
void StarCoreLed::init(bool pDebug)
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

String StarCoreLed::decodeColorSetId(int colorSetId)
{
    String output = "";
    switch (colorSetId)
    {
    case RED:
    {
        output = "RED";
    }
    break;
    case GREEN:
    {
        output = "GREEN";
    }
    break;
    case BLUE:
    {
        output = "BLUE";
    }
    break;
    case YELLOW:
    {
        output = "YELLOW";
    }
    break;
    case ACQUA:
    {
        output = "ACQUA";
    }
    break;
    case PURPLE:
    {
        output = "PURPLE";
    }
    break;
    case ORANGE:
    {
        output = "ORANGE";
    }
    break;
    case WHITE:
    {
        output = "WHITE";
    }
    break;
    case OFF:
    {
        output = "OFF";
    }
    break;
    }

    return output;
}

void StarCoreLed::getCurrentColorSet()
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
void StarCoreLed::setCurrentColorSet(int colorSetId)
{
    logger.writeLine("Saving colorset...");
    //logger.write(decodeColorSetId(colorSetId));
    EEPROM.write(REG_COLORSET, colorSetId);
    EEPROM.write(REG_CHECK, CHECK_VALUE);

    getCurrentColorSet();

    //logger.writeLine(": OK");
}

void StarCoreLed::changeColor(int colorSetId)
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

void StarCoreLed::setGradientColor(int red, int green, int blue, int white)
{
    if (red < 0)
    {
        red = 0;
    }
    if (green < 0)
    {
        green = 0;
    }
    if (blue < 0)
    {
        blue = 0;
    }
    if (white < 0)
    {
        white = 0;
    }
    if (red > 255)
    {
        red = 255;
    }
    if (green > 255)
    {
        green = 255;
    }
    if (blue > 255)
    {
        blue = 255;
    }
    if (white > 255)
    {
        white = 255;
    }
    if (!COMMON_GND)
    {
        analogWrite(PIN_RED, red);
        analogWrite(PIN_GREEN, green);
        analogWrite(PIN_BLUE, blue);
        analogWrite(PIN_WHITE, white);
    }
    else
    {
        analogWrite(PIN_RED, 255 - red);
        analogWrite(PIN_GREEN, 255 - green);
        analogWrite(PIN_BLUE, 255 - blue);
        analogWrite(PIN_WHITE, 255 - white);
    }

    gradientColorSet = {red, green, blue, white};
}

void StarCoreLed::turnOff()
{
    changeColor(OFF);
}

void StarCoreLed::blink()
{
    if (!alreadyBlinked)
    {
        logger.writeLine("Blink");
        changeColor(WHITE);
        delay(TIME_BLINK_WAITARM);
        turnOff();
    }
}

void StarCoreLed::changeColorBlink()
{
    if (!alreadyBlinked)
    {
        logger.writeLine("Blink change color");
        changeColor(currentChangeColorSetId);
        delay(TIME_BLINK_WAITARM_WITH_COLOR);
        turnOff();
    }
}

void StarCoreLed::fadeOut()
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
void StarCoreLed::fadeIn()
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
void StarCoreLed::clash()
{
    logger.writeLine("Clash:");
    logger.writeParamString("Color Set", decodeColorSetId(currentColorSetId));
    changeColor(clashColorSetId);
    delay(CLASH_TIME);
    changeColor(currentColorSetId);
}

void StarCoreLed::blinkRecharge(NeedBlinkRecharge needBlinkRecharge)
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
void StarCoreLed::loop(bool &rNeedSwing, bool &rNeedClash, Status &rStatus,
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

    if (currentBlinkRechargeStatus.needRecharge)
    {
        blinkRecharge(currentBlinkRechargeStatus);
        currentBlinkRechargeStatus = {false, 0};
    }
}