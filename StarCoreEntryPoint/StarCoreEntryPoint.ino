/*
   Starting Development Date: 02/02/2019
   Description: Star Core Entry Point
   Developer: Dario Concilio
   Company: Crystal Cave s.n.c.
*/

#include "StarCoreEntryPoint.h"

#define BUILD "1.9.12"
#define DEBUG false
#define SERIAL_NUMBER "SC1800001"

StarCoreEntryPoint entryPoint;

void setup() {
  entryPoint.init(DEBUG, BUILD, SERIAL_NUMBER);
  attachInterrupt(digitalPinToInterrupt(entryPoint.getInt1Pin()), int1ISR, RISING);
}

void loop() {
  entryPoint.loop();
}

void int1ISR()
{
  entryPoint.incrementInt1ISR();
}
