#include "CoreAudio.h"

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

CoreAudio& CoreAudio::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*             ON_ENTER    ON_LOOP   ON_EXIT  EVT_MUTE  EVT_ARM  EVT_ARMED  EVT_SWING  EVT_CLASH  EVT_DISARM   ELSE */
    /*   IDLE */   ENT_IDLE, ATM_SLEEP, EXT_IDLE,     MUTE,     ARM,        -1,        -1,        -1,         -1,    -1,
    /*   MUTE */   ENT_MUTE,        -1,       -1,       -1,      -1,        -1,        -1,        -1,       IDLE,    -1,
    /*    ARM */    ENT_ARM,        -1,       -1,       -1,      -1,        -1,        -1,        -1,         -1, ARMED,
    /*  ARMED */         -1,  LP_ARMED,       -1,       -1,      -1,        -1,     SWING,     CLASH,     DISARM,    -1,
    /*  CLASH */  ENT_CLASH,        -1,       -1,       -1,      -1,        -1,        -1,        -1,         -1, ARMED,
    /*  SWING */  ENT_SWING,  LP_SWING,       -1,       -1,      -1,     ARMED,        -1,     CLASH,         -1,    -1,
    /* DISARM */ ENT_DISARM,        -1,       -1,       -1,      -1,       ARM,        -1,        -1,       IDLE,    -1,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );
  pinMode(POWER_AMP_PIN, OUTPUT);
  digitalWrite(POWER_AMP_PIN, LOW);
  SPI.setSCK(SCK_PIN);
  SPI.setMOSI(SI_PIN);
  SPI.setMISO(SO_PIN);
  SerialFlash.begin(CS_PIN);
  patchSineMixer = new AudioConnection(soundSine, 0, mainMixer, CHANNEL_SINE);
  patchFlashMixer = new AudioConnection(soundPlayFlashRaw, 0, mainMixer, CHANNEL_HUM);
  patchFlashFXMixer = new AudioConnection(soundPlayFlashFXRaw, 0, mainMixer, CHANNEL_FX);
  if (USE_SMOOTH_SWING)
  {
    patchFlashSmoothSwingAMixer = new AudioConnection(soundPlayFlashSmoothSwingARaw, 0, mainMixer, CHANNEL_SMOOTH_SWING_A);
    patchFlashSmoothSwingBMixer = new AudioConnection(soundPlayFlashSmoothSwingBRaw, 0, mainMixer, CHANNEL_SMOOTH_SWING_B);
  }
  patchMixerDac = new AudioConnection(mainMixer, outputDac);
  AudioMemory(AUDIO_BLOCK);
  mainMixer.gain(CHANNEL_HUM, MAX_VOLUME); // HUM
  mainMixer.gain(CHANNEL_FX, MAX_VOLUME);  // FX: Clash and Swing
  mainMixer.gain(CHANNEL_SINE, 0);
  if (USE_SMOOTH_SWING)
  {
    mainMixer.gain(CHANNEL_SMOOTH_SWING_A, 1);
    mainMixer.gain(CHANNEL_SMOOTH_SWING_B, 0);
  }
  soundSine.amplitude(1);
  soundSine.frequency(BEEP_FREQUENCY);
  return *this;          
}

/* Add C++ code for each internally handled event (input) 
 * The code must return 1 to trigger the event
 */

int CoreAudio::event( int id ) {
  switch ( id ) {
    case EVT_MUTE:
      return 0;
    case EVT_ARM:
      return 0;
    case EVT_ARMED:
      return 0;
    case EVT_SWING:
      return 0;
    case EVT_CLASH:
      return 0;
    case EVT_DISARM:
      return 0;
  }
  return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 */

void CoreAudio::action( int id ) {
  switch ( id ) {
    case ENT_IDLE:
        if (!soundPlayFlashRaw.isPlaying())
        {
          digitalWrite(POWER_AMP_PIN, LOW);
        }
      return;
    case LP_IDLE:
        if (!soundPlayFlashRaw.isPlaying())
        {
          digitalWrite(POWER_AMP_PIN, LOW);
        } 
      return;
    case EXT_IDLE:
      digitalWrite(POWER_AMP_PIN, HIGH);
      return;
    case ENT_MUTE:
      beep();
      beep();
      return;
    case ENT_ARM:
      mainMixer.gain(CHANNEL_HUM, MAX_VOLUME);
      mainMixer.gain(CHANNEL_FX, MAX_VOLUME);
      soundPlayFlashRaw.play("POWERON_0.RAW");
      return;
    case LP_ARMED:
      if (!soundPlayFlashRaw.isPlaying())
      {
        soundPlayFlashRaw.play("HUM_0.RAW");
      }
      if (USE_SMOOTH_SWING)
      {
        soundPlayFlashSmoothSwingARaw.stop();
        soundPlayFlashSmoothSwingBRaw.stop();
      }
      return;
    case ENT_CLASH:
      if (soundPlayFlashRaw.isPlaying())
      {
        soundPlayFlashRaw.stop();
      }
      if (USE_SMOOTH_SWING)
      {
        soundPlayFlashSmoothSwingARaw.stop();
        soundPlayFlashSmoothSwingBRaw.stop();
      }
      clashId = random(1, 10);
      clashString = "CLASH_" + String(clashId) + "_0.RAW";
      soundPlayFlashFXRaw.play(clashString.c_str());
      return;
    case ENT_SWING:
      if (USE_SMOOTH_SWING)
      {
        mainMixer.gain(CHANNEL_SMOOTH_SWING_A, 0);
        mainMixer.gain(CHANNEL_SMOOTH_SWING_B, 0);
        swingId = random(1, 8);
        if (random(2))
        {
          smoothSwingStringH = "SMOOTHSWINGH_" + String(swingId) + "_0.RAW";
          smoothSwingStringL = "SMOOTHSWINGL_" + String(swingId) + "_0.RAW";
        }
        else
        {
          smoothSwingStringH = "SMOOTHSWINGL_" + String(swingId) + "_0.RAW";
          smoothSwingStringL = "SMOOTHSWINGH_" + String(swingId) + "_0.RAW";
        }
        t0 = micros();
        totalRotation = 0;
        transitionVolume = 0;
        transition1midPoint = random(TRANSITION_1_MIN, TRANSITION_1_MAX);
        transition2midPoint = transition1midPoint + 180;
        if (DEBUG_SMOOTHSWING)
        {
          CoreLogging::writeLine("Transition1midPoint: %f", transition1midPoint);
          CoreLogging::writeLine("totalRotation, swingSpeed, swingStrength, humVolume, transitionVolume, swingVolumeA, swingVolumeB");
        }
      }
      else
      {
        if (!soundPlayFlashFXRaw.isPlaying())
        {
          if (soundPlayFlashRaw.isPlaying())
          {
            soundPlayFlashRaw.stop();
          }
          swingId = random(1, 8);
          swingString = "SWING_" + String(swingId) + "_0.RAW";
          soundPlayFlashFXRaw.play(swingString.c_str());
        }        
      }
      return;
    case LP_SWING:
      if (USE_SMOOTH_SWING)
      {
        // keep HUM playing
        if (!soundPlayFlashRaw.isPlaying())
        {
          soundPlayFlashRaw.play("HUM_0.RAW");
        }
        if (!soundPlayFlashSmoothSwingARaw.isPlaying())
        {
          soundPlayFlashSmoothSwingARaw.play(smoothSwingStringH.c_str());
        }
        if (!soundPlayFlashSmoothSwingBRaw.isPlaying())
        {
          soundPlayFlashSmoothSwingBRaw.play(smoothSwingStringL.c_str());
        }
        t1 = micros();
        delta = t1 - t0;
        t0 = t1;
        totalRotation += swingSpeed * delta / 1000000;
        // warparound
        if (totalRotation > 360)
        {
          totalRotation = 0;
        }
        // transition 1 (transitionVolume from 0 to 1)
        if (totalRotation > (transition1midPoint - TRANSITION_1_WIDTH) &&
            totalRotation < (transition1midPoint + TRANSITION_1_WIDTH))
        {
          transitionVolume = (totalRotation - transition1midPoint + TRANSITION_1_WIDTH / 2) / TRANSITION_1_WIDTH;
          transitionVolume = max(0, transitionVolume);
          transitionVolume = min(1, transitionVolume);
        }
        // transition 2 (transitionVolume from 1 to 0)
        if (totalRotation > (transition2midPoint - TRANSITION_2_WIDTH) &&
            totalRotation < (transition2midPoint + TRANSITION_2_WIDTH))
        {
          transitionVolume = 1 - (totalRotation - transition2midPoint + TRANSITION_2_WIDTH / 2) / TRANSITION_2_WIDTH;
          transitionVolume = max(0, transitionVolume);
          transitionVolume = min(1, transitionVolume);
        }

        swingStrength = min( 1, swingSpeed / SWING_SENSITIVITY );   // The strength of the swing is a normalized value based on the angular velocity. For example, if you want the swing to be at maximum volume when you're swinging it at 4*PI rad/sec (720 deg/sec), your swing strength is min( 1, angularvelocity / ( 4 * PI ) ). Note, we clamp it to 1. This value is the one that determines how loud to make the swing and how much to duck the main hum.
        swingStrength = powf(swingStrength, SWING_SHARPNESS);       // Add sharmpess to the swing. Higher value means more pronunced swing
        humVolume = 1.0 - swingStrength * MAXIMUM_HUM_DUCKING;
        swingVolumeA = swingStrength * transitionVolume;
        swingVolumeB = swingStrength * (1 - transitionVolume);
        mainMixer.gain(CHANNEL_HUM, humVolume * MAX_VOLUME);
        mainMixer.gain(CHANNEL_SMOOTH_SWING_A, swingVolumeA * MAX_VOLUME);
        mainMixer.gain(CHANNEL_SMOOTH_SWING_B, swingVolumeB * MAX_VOLUME);
        if (DEBUG_SMOOTHSWING)
        {
          CoreLogging::writeLine("%f %f %f %f %f %f %f", totalRotation, swingSpeed, swingStrength, humVolume, transitionVolume, swingVolumeA, swingVolumeB);
        }
      }
      else
      {
        if (!soundPlayFlashFXRaw.isPlaying())
        {
          if (soundPlayFlashRaw.isPlaying())
          {
            soundPlayFlashRaw.stop();
          }
          swingId = random(1, 8);
          swingString = "SWING_" + String(swingId) + "_0.RAW";
          soundPlayFlashFXRaw.play(swingString.c_str());
        }        
      }
      return;
    case ENT_DISARM:
      soundPlayFlashFXRaw.stop();
      if (USE_SMOOTH_SWING)
      {
        soundPlayFlashSmoothSwingARaw.stop();
        soundPlayFlashSmoothSwingBRaw.stop();
      }
      soundPlayFlashRaw.play("POWEROFF_0.RAW");
      return;
  }
}

void CoreAudio::beep()
{
  digitalWrite(POWER_AMP_PIN, HIGH);
  delay(50);
  mainMixer.gain(CHANNEL_SINE, MAX_VOLUME);
  delay(BEEP_TIME);
  mainMixer.gain(CHANNEL_SINE, 0);
  digitalWrite(POWER_AMP_PIN, LOW);
}

void CoreAudio::setSwingSpeed(float s){
  swingSpeed = s;
}


/* Optionally override the default trigger() method
 * Control how your machine processes triggers
 */

CoreAudio& CoreAudio::trigger( int event ) {
  Machine::trigger( event );
  return *this;
}

/* Optionally override the default state() method
 * Control what the machine returns when another process requests its state
 */

int CoreAudio::state( void ) {
  return Machine::state();
}

/* Nothing customizable below this line                          
 ************************************************************************************************
*/

/* Public event methods
 *
 */

CoreAudio& CoreAudio::mute() {
  trigger( EVT_MUTE );
  return *this;
}

CoreAudio& CoreAudio::arm() {
  trigger( EVT_ARM );
  return *this;
}

CoreAudio& CoreAudio::armed() {
  trigger( EVT_ARMED );
  return *this;
}

CoreAudio& CoreAudio::swing() {
  trigger( EVT_SWING );
  return *this;
}

CoreAudio& CoreAudio::clash() {
  trigger( EVT_CLASH );
  return *this;
}

CoreAudio& CoreAudio::disarm() {
  trigger( EVT_DISARM );
  return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */

CoreAudio& CoreAudio::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "COREAUDIO\0EVT_MUTE\0EVT_ARM\0EVT_ARMED\0EVT_SWING\0EVT_CLASH\0EVT_DISARM\0ELSE\0IDLE\0MUTE\0ARM\0ARMED\0CLASH\0SWING\0DISARM" );
  return *this;
}




