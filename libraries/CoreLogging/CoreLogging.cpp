#include "CoreLogging.h"
#include "Arduino.h"

// Write message without return
void CoreLogging::write(String message)
{

  if (DEBUG)
    Serial.print(message);
}

// Write message with return
void CoreLogging::writeLine(String message, String prefix)
{

  if (DEBUG)
  {
    if (prefix != "")
    {
      Serial.print(prefix);
    }
    Serial.println(message);
  }
}

// Write label + value (String) with return
void CoreLogging::writeParamString(String param, String value, String prefix)
{

  if (DEBUG)
  {
    if (prefix != "")
    {
      Serial.print(prefix);
    }
    Serial.print(param);
    Serial.print(": ");
    Serial.println(value);
  }
}

// Write label + value (int) with return
void CoreLogging::writeParamInt(String param, int value, String prefix)
{

  if (DEBUG)
  {
    if (prefix != "")
    {
      Serial.print(prefix);
    }
    Serial.print(param);
    Serial.print(": ");
    Serial.println(value, 0);
  }
}

// Write status decoded with return
void CoreLogging::writeParamStatus(Status status, String prefix)
{
  if (DEBUG)
  {
    if (prefix != "")
    {
      Serial.print(prefix);
    }
    Serial.print("Status: ");
    switch (status)
    {
    case Status::armed: {
      Serial.println("Armed");
    }
    break;
    case Status::disarmed: {
      Serial.println("Disarmed");
    }
    break;
    case Status::waitArm: {
      Serial.println("Waiting Arm");
    }
    break;
    case Status::waitDisarm: {
      Serial.println("Waiting Disarm");
    }
    break;
    case Status::arming: {
      Serial.println("Arming");
    }
    break;
    case Status::disarming: {
      Serial.println("Disarming");
    }
    break;
    case Status::armingWithChangeColor: {
      Serial.println("Arming With Change Color");
    }
    break;
    case Status::waitArmWithChangeColorNext: {
      Serial.println("Wait arm With Change Color Next");
    }
    break;
    case Status::waitArmWithChangeColor: {
      Serial.println("Wait arm With Change Color");
    }
    break;
    case Status::disarmedInRecharge: {
      Serial.println("In Recharging");
    }
    break;
    }
  }
}

// Write label + value (float) with return
void CoreLogging::writeParamFloat(String param, float value, String prefix)
{

  if (DEBUG)
  {
    if (prefix != "")
    {
      Serial.print(prefix);
    }
    Serial.print(param);
    Serial.print(": ");
    Serial.println(value, 4);
  }
}

// Write label + value (long) with return
void CoreLogging::writeParamLong(String param, long value, String prefix)
{

  if (DEBUG)
  {
    if (prefix != "")
    {
      Serial.print(prefix);
    }
    Serial.print(param);
    Serial.print(": ");
    Serial.println(value);
  }
}
