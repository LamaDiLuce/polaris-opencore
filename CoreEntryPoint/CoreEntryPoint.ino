#include "CoreEntryPoint.h"

#define BUILD "1.9.14"

CoreEntryPoint entryPoint;

void setup()
{
  entryPoint.init(BUILD);
  attachInterrupt(digitalPinToInterrupt(entryPoint.getInt1Pin()), int1ISR, RISING);
}

void loop()
{
  entryPoint.loop();
}

void int1ISR()
{
  entryPoint.incrementInt1ISR();
}
