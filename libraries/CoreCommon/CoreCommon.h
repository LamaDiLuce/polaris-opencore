#pragma once

enum Status
{
    disarmed = 0,
    armed = 1,
    waitArm = 2,
    arming = 3,
    disarming = 4,
    waitDisarm = 5,
    waitArmWithChangeColor = 6,
    waitArmWithChangeColorNext = 7,
    armingWithChangeColor = 8,
    disarmedInRecharge = 9
};

enum Requests
{
    lowClash = 0,
    highCLash = 1,
    swing = 2,
    arm = 3,
    disarm = 4,
    none = 5
};

#define COLORS 7
#define RED 0
#define GREEN 1
#define BLUE 2
#define YELLOW 3
#define ACQUA 4
#define PURPLE 5
#define ORANGE 6
#define WHITE 7
#define OFF 8

#define BLINK_TIME 300
#define RECHARGE_COLOR RED
#define RECHARGED_COLOR BLUE

struct ColorLed
{
    int red;
    int green;
    int blue;
    int white;
};

struct FilterSensor
{
    int readingCount;
    float progressAverageValue;
    float previousAverageValue;
};

struct NeedBlinkRecharge
{
    bool chargeSecuence;
    bool needRecharge;
    int colorRecharge;
};
