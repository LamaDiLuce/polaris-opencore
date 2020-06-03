#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <CoreLogging.h>
#include "CoreAudio.h"
#include "CoreSensor.h"
#include "CoreLed.h"
#include "CoreRecharge.h"
#include "CoreCommon.h"

#define BAUD_RATE 9600
#define STX 0x02
#define ETX 0x03
#define REQUEST_VER 'V'

#ifndef CoreEntryPoint_h
#define CoreEntryPoint_h

class CoreEntryPoint
{
    public:

        //Costructor
        CoreEntryPoint();
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
        CoreAudio audioModule;
        CoreSensor sensorModule;
        CoreLed ledModule;
        CoreRecharge rechargeModule;
        CoreLogging logger;
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
