#include "Arduino.h"
#include "StarCoreAudio.h"
#include <string.h>

//Costructor
StarCoreAudio::StarCoreAudio()
{
}

/*
 * Public Methods
 */

//Init
void StarCoreAudio::init(bool pDebug)
{
    debugMode = pDebug;
    logger.init(debugMode);

    SPI.setSCK(SCK_PIN);
    SPI.setMOSI(SI_PIN);
    SPI.setMISO(SO_PIN);

    pinMode(POWER_AMP_PIN, OUTPUT);
    digitalWrite(POWER_AMP_PIN, LOW);
    //digitalWrite(POWER_AMP_PIN, HIGH);

    logger.write("SerialFlash connecting: ");
    if (!SerialFlash.begin(CS_PIN))
    {
        logger.writeLine("Error");
    }
    else
    {
        logger.writeLine("OK");
    }

    patchSineMixer = new AudioConnection(soundSine, 0, mainMixer, CHANNEL_SINE);
    patchFlashMixer = new AudioConnection(soundPlayFlashRaw, 0, mainMixer, CHANNEL_HUM);
    patchFlashFXMixer = new AudioConnection(soundPlayFlashFXRaw, 0, mainMixer, CHANNEL_FX);
    patchMixerDac = new AudioConnection(mainMixer, outputDac);

    AudioMemory(AUDIO_BLOCK);

    mainMixer.gain(CHANNEL_HUM, 1); //HUM
    mainMixer.gain(CHANNEL_FX, 1);  //FX: Clash and Swing
    mainMixer.gain(CHANNEL_SINE, 0);
    soundSine.amplitude(1);
    soundSine.frequency(BEEP_FREQUENCY);

    //randomSeed(analogRead(0));
}

void StarCoreAudio::arm()
{
    if ((status == Status::waitArm) ||
        (status == Status::waitArmWithChangeColor) ||
        (status == Status::waitArmWithChangeColorNext) ||
        (status == Status::armingWithChangeColor))
    {
        changeColorStarted = false;

        soundPlayFlashRaw.play("POWERON_0.RAW");

        if (status != Status::armingWithChangeColor)
        {
            status = Status::arming;
        }
    }
}

void StarCoreAudio::disarm()
{
    if (status == Status::armed)
    {
        soundPlayFlashRaw.play("POWEROFF_0.RAW");
        status = Status::disarming;
    }
}

void StarCoreAudio::clash()
{
    if (status == Status::armed)
    {
        //if (!soundPlayFlashFXRaw.isPlaying())
        //{
        if (soundPlayFlashRaw.isPlaying())
            soundPlayFlashRaw.stop();

        int clashId = random(1, 10);

        String clash = "CLASH_" + String(clashId) + "_0.RAW";
        soundPlayFlashFXRaw.play(clash.c_str());
    }
}

void StarCoreAudio::swing()
{
    if ((status == Status::armed) && (!swinging))
    {
        if (!soundPlayFlashFXRaw.isPlaying())
        {
            if (soundPlayFlashRaw.isPlaying())
                soundPlayFlashRaw.stop();

            int swingId = random(1, 8);
            String swing = "SWING_" + String(swingId) + "_0.RAW";

            soundPlayFlashFXRaw.play(swing.c_str());
        }
    }
}

void StarCoreAudio::checkHum()
{
    if (status == Status::armed)
    {
        if (!soundPlayFlashRaw.isPlaying())
        {
            if (swinging)
            {
                swinging = false;
            }
            soundPlayFlashRaw.play("HUM_0.RAW");
        }
    }
}

void StarCoreAudio::checkArming()
{
    if ((status == Status::arming) ||
        (status == Status::armingWithChangeColor))
    {
        if (!soundPlayFlashRaw.isPlaying())
        {
            status = Status::armed;
            logger.writeLine("Armed");
        }
    }
}

void StarCoreAudio::checkDisarming()
{
    if (status == Status::disarming)
    {
        if (!soundPlayFlashRaw.isPlaying())
        {
            status = Status::disarmed;
            logger.writeLine("Disarmed");
        }
    }
}

void StarCoreAudio::changeColorMode()
{
    if ((status == Status::waitArmWithChangeColor) && (!changeColorStarted))
    {
        digitalWrite(POWER_AMP_PIN, HIGH);
        delay(50);
        mainMixer.gain(CHANNEL_SINE, 1);
        delay(BEEP_TIME);
        mainMixer.gain(CHANNEL_SINE, 0);
        changeColorStarted = true;
        digitalWrite(POWER_AMP_PIN, LOW);
    }
}

void StarCoreAudio::checkPowerAmp()
{
    if ((status == Status::disarmed) ||
        (status == Status::disarmedInRecharge) ||
        (status == Status::waitArmWithChangeColor) ||
        (status == Status::waitArmWithChangeColorNext))
    {
        digitalWrite(POWER_AMP_PIN, LOW);
    }
    else
    {
        digitalWrite(POWER_AMP_PIN, HIGH);
    }
}

void StarCoreAudio::beep()
{
    digitalWrite(POWER_AMP_PIN, HIGH);
    delay(250);
    mainMixer.gain(CHANNEL_SINE, 1);
    delay(BEEP_TIME);
    mainMixer.gain(CHANNEL_SINE, 0);
    digitalWrite(POWER_AMP_PIN, LOW);
}

//Process loop
void StarCoreAudio::loop(bool &rNeedSwing, bool &rNeedClash, Status &rStatus,
                         bool &rNeedArm, bool &rNeedDisarm)
{
    status = rStatus;
	if (rNeedClash)
    {
        clash();
    }
	
    checkHum();

    checkPowerAmp();

    changeColorMode();



    if ((rNeedSwing) && (!rNeedClash))
    {
        swing();
    }

    if (rNeedArm)
    {
        arm();
    }

    if (rNeedDisarm)
    {
        disarm();
    }

    checkArming();
    checkDisarming();

    rStatus = status;
}