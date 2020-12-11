#pragma once

static constexpr bool DEBUG = true;

// LED RGBW
#define PIN_RED 3
#define PIN_GREEN 4
#define PIN_BLUE 5
#define PIN_WHITE 6
#define COMMON_GND false

#define CHARGE_PIN 13
#define STANDBY_PIN 15
#define USB_PIN 23

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

struct ColorLed
{
  int red;
  int green;
  int blue;
  int white;
};
