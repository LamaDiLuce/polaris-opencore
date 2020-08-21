#include "CoreAudio.h"

#include <string.h>

#include "Arduino.h"

// Costructor
CoreAudio::CoreAudio()
{
}

/*
 * Public Methods
 */

// Init
void CoreAudio::init()
{
  SPI.setSCK(SCK_PIN);
  SPI.setMOSI(SI_PIN);
  SPI.setMISO(SO_PIN);

  pinMode(POWER_AMP_PIN, OUTPUT);
  digitalWrite(POWER_AMP_PIN, LOW);
  // digitalWrite(POWER_AMP_PIN, HIGH);

  CoreLogging::writeLine("CoreAudio: SerialFlash connecting: ");
  if (!SerialFlash.begin(CS_PIN))
  {
    CoreLogging::writeLine("CoreAudio: Serial connection error");
  }
  else
  {
    CoreLogging::writeLine("CoreAudio: Serial connection OK");
  }

  patchSineMixer = new AudioConnection(soundSine, 0, mainMixer, CHANNEL_SINE);
  patchFlashMixer = new AudioConnection(soundPlayFlashRaw, 0, mainMixer, CHANNEL_HUM);
  patchFlashFXMixer = new AudioConnection(soundPlayFlashFXRaw, 0, mainMixer, CHANNEL_FX);
  patchMixerDac = new AudioConnection(mainMixer, outputDac);

  AudioMemory(AUDIO_BLOCK);

  mainMixer.gain(CHANNEL_HUM, 1); // HUM
  mainMixer.gain(CHANNEL_FX, 1);  // FX: Clash and Swing
  mainMixer.gain(CHANNEL_SINE, 0);
  soundSine.amplitude(1);
  soundSine.frequency(BEEP_FREQUENCY);

  // randomSeed(analogRead(0));
}

void CoreAudio::arm()
{
  if ((status == Status::waitArm) || (status == Status::waitArmWithChangeColor) ||
      (status == Status::waitArmWithChangeColorNext) || (status == Status::armingWithChangeColor))
  {
    changeColorStarted = false;

    soundPlayFlashRaw.play("POWERON_0.RAW");

    if (status != Status::armingWithChangeColor)
    {
      status = Status::arming;
    }
  }
}

void CoreAudio::disarm()
{
  if (status == Status::armed)
  {
    soundPlayFlashRaw.play("POWEROFF_0.RAW");
    status = Status::disarming;
    muted = false;
  }
}

void CoreAudio::clash()
{
  if (status == Status::armed)
  {
    if (soundPlayFlashRaw.isPlaying())
    {
      soundPlayFlashRaw.stop();
    }
    int clashId = random(1, 10);

    String clash = "CLASH_" + String(clashId) + "_0.RAW";
    soundPlayFlashFXRaw.play(clash.c_str());
  }
  else if (status == Status::waitArm || 
           status == Status::waitArmWithChangeColor || 
           status == Status::waitArmWithChangeColorNext)
  {
    beep();
    beep();
    muted = true;
    CoreLogging::writeLine("CoreAudio: Muted");
  }
}

void CoreAudio::swing()
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

void CoreAudio::checkHum()
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

void CoreAudio::checkArming()
{
  if ((status == Status::arming) || (status == Status::armingWithChangeColor))
  {
    if (!soundPlayFlashRaw.isPlaying())
    {
      status = Status::armed;
      CoreLogging::writeLine("CoreAudio: Armed");
    }
  }
}

void CoreAudio::checkDisarming()
{
  if (status == Status::disarming)
  {
    if (!soundPlayFlashRaw.isPlaying())
    {
      status = Status::disarmed;
      CoreLogging::writeLine("CoreAudio: Disarmed");
    }
  }
}

void CoreAudio::changeColorMode()
{
  if ((status == Status::waitArmWithChangeColor) && (!changeColorStarted))
  {
    beep();
    changeColorStarted = true;
  }
}

void CoreAudio::checkPowerAmp()
{
  if ((status == Status::disarmed) || 
      (status == Status::disarmedInRecharge) ||
      (status == Status::waitArmWithChangeColor) || 
      (status == Status::waitArmWithChangeColorNext) ||
      (muted))
  {
    digitalWrite(POWER_AMP_PIN, LOW);
  }
  else
  {
    digitalWrite(POWER_AMP_PIN, HIGH);
  }
}

void CoreAudio::beep()
{
  digitalWrite(POWER_AMP_PIN, HIGH);
  delay(50);
  mainMixer.gain(CHANNEL_SINE, 1);
  delay(BEEP_TIME);
  mainMixer.gain(CHANNEL_SINE, 0);
  digitalWrite(POWER_AMP_PIN, LOW);
}

// Process loop
void CoreAudio::loop(bool& rNeedSwing, bool& rNeedClash, Status& rStatus, bool& rNeedArm, bool& rNeedDisarm)
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