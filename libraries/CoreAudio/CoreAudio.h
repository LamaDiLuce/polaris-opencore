#include "Arduino.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "CoreLogging.h"
#include "CoreCommon.h"

//Flash
#define CS_PIN 10
#define SI_PIN 11
#define SO_PIN 12
#define SCK_PIN 14

#define POWER_AMP_PIN 16

#define CHANNEL_SINE 1
#define CHANNEL_HUM 0
#define CHANNEL_FX 2

#define BEEP_TIME 125
#define BEEP_FREQUENCY 1000
#define AUDIO_BLOCK 16

#ifndef CoreAudio_h
#define CoreAudio_h

class CoreAudio
{
  public:
    //Costructor
    CoreAudio();
    //Init
    void init(bool debug);
    //Process loop
    void loop(bool &rNeedSwing, bool &rNeedClash, Status &rStatus, bool &rNeedArm, bool &rNeedDisarm);
    //Commands
    void arm();
    void disarm();
    void beep();
    
  private:
    AudioSynthWaveformSine soundSine;
    AudioPlaySerialflashRaw soundPlayFlashRaw;
    AudioPlaySerialflashRaw soundPlayFlashFXRaw;
    AudioMixer4 mainMixer;
    AudioOutputAnalog outputDac;
    AudioConnection *patchSineMixer = NULL;
    AudioConnection *patchFlashMixer = NULL;
    AudioConnection *patchFlashFXMixer = NULL;

    AudioConnection *patchMixerDac = NULL;

    bool debugMode = false;
    CoreLogging logger;
    SerialFlashFile file;

    bool swinging = false;
    bool changeColorStarted = false; 
    Status status = Status::disarmed;

    void clash();
    void swing();
    void checkHum();
    void checkArming();
    void checkDisarming();
    void changeColorMode();
    void checkPowerAmp();
    
};

#endif