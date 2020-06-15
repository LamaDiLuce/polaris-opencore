#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "CoreLogging.h"
#include "CoreAudio.h"
#include "CoreSensor.h"
#include "CoreLed.h"
#include "CoreRecharge.h"
#include "CoreCommon.h"

#define BAUD_RATE 9600
static constexpr int STX = 0x02;
static constexpr int ETX = 0x03;
static constexpr int LF = 0x0a;

class CoreEntryPoint
{
public:
    //Costructor
    CoreEntryPoint();
    //Init
    void init(bool debug, String build);
    //Process loop
    void loop();
    //Support
    void incrementInt1ISR();
    //Get Interrupt Pin
    int getInt1Pin();
    //Reset status events
    void releaseStatus();
    //Keep serial communication
    void checkSerials();

    //Hardware Serial Numbers
    void kinetisUID(uint32_t *uid);
    String kinetisUID();

private:
    bool debugMode = false;
    String build = "";
    String serial = "";
    String incomingMessage = "";
    CoreAudio audioModule;
    CoreSensor sensorModule;
    CoreLed ledModule;
    CoreRecharge rechargeModule;
    CoreLogging logger;
    Requests request;

    void processIncomingMessage(String message);
    void spaces(int num);

    //STATUS
    bool needClashEvent;
    bool needSwingEvent;
    Status status;
    bool verticalPosition;
    bool horizontalPosition;
    bool needArmEvent;
    bool needDisarmEvent;
    NeedBlinkRecharge needBlinkRechargeEvent;
};
