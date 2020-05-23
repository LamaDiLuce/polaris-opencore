#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Logging.h>

#include "StarCoreAudio.h"
#include "StarCoreSensor.h"
#include "StarCoreLed.h"
#include "StarCoreRecharge.h"
#include "StarCoreCommon.h"

#define BAUD_RATE 9600
#define STX 0x02
#define ETX 0x03
#define REQUEST_VER 'V'

#ifndef StarCoreEntryPoint_h
#define StarCoreEntryPoint_h

class StarCoreEntryPoint
{
    public:

        //Costructor
        StarCoreEntryPoint();
        //Init
        void init(bool debug, String build, String serial);
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

    private:

        bool debugMode = false;
        String build = "";
        String serial = "";
        String incomingMessage = "";
        StarCoreAudio audioModule;
        StarCoreSensor sensorModule;
        StarCoreLed ledModule;
        StarCoreRecharge rechargeModule;
        Logging logger;
        Requests request;

        void processIncomingMessage(String message);

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

#endif
