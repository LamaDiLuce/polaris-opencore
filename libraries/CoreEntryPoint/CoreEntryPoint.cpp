#include "Arduino.h"
#include "CoreEntryPoint.h"

//Costructor
CoreEntryPoint::CoreEntryPoint()
{
}

//Init
void CoreEntryPoint::init(bool pDebug, String pBuild)
{
    debugMode = pDebug;
    build = pBuild;
    serial = kinetisUID();

    Serial.begin(BAUD_RATE); //It needs for PC communication services

    logger.init(debugMode);

    logger.writeParamString("Build", build);
    logger.writeParamString("Serial Number", serial);

    audioModule.init(debugMode);
    sensorModule.init(debugMode);
    ledModule.init(debugMode);
    rechargeModule.init(debugMode);
}

//Process loop
void CoreEntryPoint::loop()
{
    sensorModule.loop(needSwingEvent, needClashEvent, status, verticalPosition,
                      needArmEvent, horizontalPosition, needDisarmEvent);
    audioModule.loop(needSwingEvent, needClashEvent, status, needArmEvent, needDisarmEvent);
    rechargeModule.loop(status, needBlinkRechargeEvent);
    ledModule.loop(needSwingEvent, needClashEvent, status,
                   needArmEvent, needDisarmEvent, needBlinkRechargeEvent);

    releaseStatus();

    checkSerials();
}

void CoreEntryPoint::releaseStatus()
{
    needSwingEvent = false;
    needClashEvent = false;
    verticalPosition = false;
    needArmEvent = false;
}

int CoreEntryPoint::getInt1Pin()
{
    return sensorModule.getInt1Pin();
}

void CoreEntryPoint::incrementInt1ISR()
{
    sensorModule.int1Status++;
}

void CoreEntryPoint::kinetisUID(uint32_t *uid)
{
    uid[0] = SIM_UIDMH;
    uid[1] = SIM_UIDML;
    uid[2] = SIM_UIDL;
}

String CoreEntryPoint::kinetisUID()
{
    uint32_t uid[3];
    char uidString[27];
    kinetisUID(uid);
    snprintf(uidString, sizeof(uidString), "%08lx-%08lx-%08lx", uid[0], uid[1], uid[2]);
    return String(uidString);
}

void CoreEntryPoint::checkSerials()
{
    int incomingByte;

    if (Serial.available() > 0)
    {
        incomingByte = Serial.read();
        //Serial.print(String(incomingByte);
        if (incomingByte == (byte)STX)
        {
            //audioModule.beep();
            incomingMessage = "";
        }
        else if ((incomingByte == ETX) || (incomingByte == LF))
        {
            processIncomingMessage(incomingMessage);
            incomingMessage = "";
        }
        else
        {
            incomingMessage += (char)incomingByte;
        }
    }
}

void CoreEntryPoint::processIncomingMessage(String pIncomingMessage)
{
    if (pIncomingMessage == "V")
    {
        Serial.write(STX);
        Serial.print("Build: ");
        Serial.println(build);
        Serial.print("Serial Number: ");
        Serial.println(serial);
        Serial.print("SerialFlash connecting...\n");
        SerialFlash.opendir();
        unsigned char buf[256];
        Serial.println("Files on memory:");

        while (1)
        {
            //List all files to PC
            char filename[64];
            uint32_t filesize;

            if (SerialFlash.readdir(filename, sizeof(filename), filesize))
            {
                Serial.print(filename);
                Serial.print(" ");
                spaces(20 - strlen(filename));
                Serial.print(" (");
                Serial.print(filesize);
                Serial.print(")\n");
            }
            else
            {
                break; // no more files
            }
        }

        Serial.println("Read Chip Identification:");
        SerialFlash.readID(buf);
        Serial.print("  JEDEC ID:     ");
        Serial.print(buf[0], HEX);
        Serial.print(" ");
        Serial.print(buf[1], HEX);
        Serial.print(" ");
        Serial.println(buf[2], HEX);
        Serial.print("  Memory Size:  ");
        uint32_t chipsize = SerialFlash.capacity(buf);
        Serial.print(chipsize);
        Serial.println(" bytes");
        Serial.print("  Block Size:   ");
        Serial.print(SerialFlash.blockSize());
        Serial.println(" bytes");

        Serial.write(ETX);
    }
}

void CoreEntryPoint::spaces(int num)
{
    for (int i = 0; i < num; i++)
    {
        Serial.print(" ");
    }
}