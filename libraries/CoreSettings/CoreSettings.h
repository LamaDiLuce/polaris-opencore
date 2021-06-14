#pragma once

#include <Arduino.h>
//#include <EEPROM.h>
#include <SerialFlash.h>

#include "ArduinoJson.h"
#include "CoreLogging.h"
#include "SoundList.h"

#define CURRENTVERSION "2020.04"


struct SaberSettings
{
  String version;
  int activeBank;
  ColorLed colorSet[9];
  ColorLed clashSet[9];
  ColorLed swingSet[9];

  int soundEngine;
  SoundList on;
  SoundList off;
  SoundList hum;
  SoundList clash;
  SoundList swing;
};

class CoreSettings
{
public:
  // Constructor
  CoreSettings();
  void init();

  void loadDefaults();
  void loadDefaultSounds();
  void loadDefaultColors();
  void readFromStore();
  void saveToStore();

  void setActiveBank(int iBank);
  int getActiveBank();

  ColorLed getMainColor(int bank);
  ColorLed getClashColor(int bank);
  ColorLed getSwingColor(int bank);
  void setMainColor(int bank, ColorLed cc);
  void setClashColor(int bank, ColorLed cc);
  void setSwingColor(int bank, ColorLed cc);

  String getOnSounds();
  String getRandomOnSound();
  void setOnSounds(String csv);

  String getOffSounds();
  String getRandomOffSound();
  void setOffSounds(String csv);

  String getHumSounds();
  String getRandomHumSound();
  void setHumSounds(String csv);

  String getSwingSounds();
  String getRandomSwingSound();
  void setSwingSounds(String csv);

  String getClashSounds();
  String getRandomClashSound();
  void setClashSounds(String csv);

  int32_t getFileSize(const char* filen);
  void printFile(const char* filen, boolean ignore);

private:
  SaberSettings liveSettings;
};
