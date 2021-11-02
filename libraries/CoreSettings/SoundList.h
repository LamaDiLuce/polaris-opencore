#pragma once
#include <Arduino.h>

#define MAX_SOUNDS 25

class SoundList
{

public:
  SoundList();

  String getRandom();
  void setCSV(String csv);
  String getCSV();

  String sounds[MAX_SOUNDS];
  int count;
  int lastRandom;

private:
  
};