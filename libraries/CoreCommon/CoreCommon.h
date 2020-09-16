#pragma once

static constexpr bool DEBUG = false;

enum class Status
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

enum class Requests
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
#define ORANGE 1
#define YELLOW 2
#define GREEN 3
#define WHITE 4
#define ACQUA 5
#define BLUE 6
#define PURPLE 7
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
  bool chargeSequence;
  bool needRecharge;
  int colorRecharge;
};
