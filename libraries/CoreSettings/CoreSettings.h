#pragma once

#include <Arduino.h>
//#include <EEPROM.h>
#include <SerialFlash.h>

#include "ArduinoJson.h"
#include "CoreLogging.h"

#define CURRENTVERSION "2020.02"

struct SaberSettings
{
  String version;
  int activeBank;
  ColorLed colorSet[9];
  ColorLed clashSet[9];
};

class CoreSettings
{
public:
  // Constructor
  CoreSettings();
  void init();

  void loadDefaults();
  void loadDefaultColors();
  void readFromStore();
  void saveToStore();

  void setActiveBank(int iBank);
  int getActiveBank();

  ColorLed getMainColor(int bank);
  ColorLed getClashColor(int bank);
  void setMainColor(int bank, ColorLed cc);
  void setClashColor(int bank, ColorLed cc);

  int32_t getFileSize(const char* filen);
  void printFile(const char* filen, boolean ignore);

private:
  SaberSettings liveSettings;
};
