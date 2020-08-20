#include "CoreLogging.h"
#include "Arduino.h"

// Write message with return
void CoreLogging::writeToSerial(const char* message)
{
  if (DEBUG)
  {
    Serial.print("#");
    Serial.println(message);
  }
}

// Write status decoded with return
const char* CoreLogging::statusToString(Status status)
{
  switch (status)
  {
  case Status::armed: {
    return "Armed";
  }
  case Status::disarmed: {
    return "Disarmed";
  }
  case Status::waitArm: {
    return "Waiting Arm";
  }
  case Status::waitDisarm: {
    return "Waiting Disarm";
  }
  case Status::arming: {
    return "Arming";
  }
  case Status::disarming: {
    return "Disarming";
  }
  case Status::armingWithChangeColor: {
    return "Arming With Change Color";
  }
  case Status::waitArmWithChangeColorNext: {
    return "Wait arm With Change Color Next";
  }
  case Status::waitArmWithChangeColor: {
    return "Wait arm With Change Color";
  }
  case Status::disarmedInRecharge: {
    return "In Recharging";
  }
  default: {
    return "Undefined";
  }
  }
}
