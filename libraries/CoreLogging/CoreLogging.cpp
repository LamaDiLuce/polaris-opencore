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

