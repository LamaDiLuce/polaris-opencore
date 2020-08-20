#pragma once

#include "Arduino.h"
#include "CoreLed.h"
#include <CoreLogging.h>
#include <SerialFlash.h>

#define BAUD_RATE 9600
static constexpr int STX = 0x02;
static constexpr int ETX = 0x03;
static constexpr int LF = 0x0a;

#define MODE_NORMAL 0
#define MODE_CONFIG 1
// MODE_FILEUPLOAD for future use
#define MODE_FILEUPLOAD 2

class CoreComms
{
public:
  // Constructor
  CoreComms();
  // Init
  void init(String pBuild);
  void setModule(CoreLed* cled, CoreSettings* cSet);
  // Process loop
  void loop();
  byte getMode();
  void setMode(byte cmode);
  String colorToString(ColorLed cColor, boolean bHex);

private:
  byte commsMode = MODE_NORMAL;
  String build = "";
  String serial = "";
  String incomingMessage = "";
  CoreLed* ledmodule; // for access to color variables, to be moved to a settings module
  CoreSettings* setmodule;

  // Hardware Serial Numbers
  String getKinetisUID();

  void processIncomingMessage(const String& pIncomingMessage);
  void printDevInfo();

  char hexDigit(int n);
  ColorLed stringToColorLed(String sColor);
  String colorLedToString(ColorLed cColor, boolean bHex);

  void spaces(int num);
};
