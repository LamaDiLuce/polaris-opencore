#pragma once

static constexpr bool DEBUG = false;
static constexpr bool DEBUG_SMOOTHSWING = false;

static constexpr int ANALOG_REF_BATTERY_LOW = 1490;  // max 1 hour of battery left
static constexpr int ANALOG_REF_BATTERY_DEPLETED = 1763; // safe threshold to avoid under discharge

// LED RGBW
#define PIN_RED 3
#define PIN_GREEN 4
#define PIN_BLUE 5
#define PIN_WHITE 6
#define COMMON_GND false

#define CHARGE_PIN 13
#define STANDBY_PIN 15
#define USB_PIN 23

#define HW_VERSION_1 1   // Pin 40
#define HW_VERSION_2 32  // Pin 41
#define HW_VERSION_3 25  // Pin 42

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
