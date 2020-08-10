#pragma once

#include <Arduino.h>

#include "CoreCommon.h"

class CoreLogging
{
public:
  CoreLogging() = delete;
  // Write message without return
  static void write(String message);
  // Write message with return
  static void writeLine(String message, String prefix = "");
  // Write label + value (String) with return
  static void writeParamString(String param, String value, String prefix = "");
  // Write label + value (int) with return
  static void writeParamInt(String param, int value, String prefix = "");
  // Write label + value (float) with return
  static void writeParamFloat(String param, float value, String prefix = "");
  // Write status decoded with return
  static void writeParamStatus(Status status, String prefix = "");
  // Write label + value (long) with return
  static void writeParamLong(String param, long value, String prefix = "");
};
