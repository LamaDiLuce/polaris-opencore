#include "CoreAudio.h"

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

CoreAudio& CoreAudio::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*             ON_ENTER    ON_LOOP   ON_EXIT  EVT_MUTE  EVT_ARM  EVT_SWING  EVT_CLASH  EVT_DISARM   ELSE */
    /*   IDLE */   ENT_IDLE, ATM_SLEEP, EXT_IDLE,     MUTE,     ARM,        -1,        -1,         -1,    -1,
    /*   MUTE */   ENT_MUTE,        -1,       -1,       -1,      -1,        -1,        -1,       IDLE,    -1,
    /*    ARM */    ENT_ARM,        -1,       -1,       -1,      -1,        -1,        -1,         -1, ARMED,
    /*  ARMED */         -1,  LP_ARMED,       -1,       -1,      -1,     SWING,     CLASH,     DISARM,    -1,
    /*  CLASH */  ENT_CLASH,        -1,       -1,       -1,      -1,        -1,        -1,         -1, ARMED,
    /*  SWING */  ENT_SWING,        -1,       -1,       -1,      -1,        -1,     CLASH,         -1, ARMED,
    /* DISARM */ ENT_DISARM,        -1,       -1,       -1,      -1,        -1,        -1,         -1,  IDLE,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );
  SPI.setSCK(SCK_PIN);
  SPI.setMOSI(SI_PIN);
  SPI.setMISO(SO_PIN);
  pinMode(POWER_AMP_PIN, OUTPUT);
  digitalWrite(POWER_AMP_PIN, LOW);
  SerialFlash.begin(CS_PIN);
  patchSineMixer = new AudioConnection(soundSine, 0, mainMixer, CHANNEL_SINE);
  patchFlashMixer = new AudioConnection(soundPlayFlashRaw, 0, mainMixer, CHANNEL_HUM);
  patchFlashFXMixer = new AudioConnection(soundPlayFlashFXRaw, 0, mainMixer, CHANNEL_FX);
  patchMixerDac = new AudioConnection(mainMixer, outputDac);
  AudioMemory(AUDIO_BLOCK);
  mainMixer.gain(CHANNEL_HUM, 0.1); // HUM
  mainMixer.gain(CHANNEL_FX, 0.1);  // FX: Clash and Swing
  mainMixer.gain(CHANNEL_SINE, 0);
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
      digitalWrite(POWER_AMP_PIN, LOW);
      return;
    case LP_IDLE:
      return;
    case EXT_IDLE:
      digitalWrite(POWER_AMP_PIN, HIGH);
      return;
    case ENT_MUTE:
      digitalWrite(POWER_AMP_PIN, LOW);
      return;
    case ENT_ARM:
      soundPlayFlashRaw.play("POWERON_0.RAW");
      return;
    case LP_ARMED:
      if (!soundPlayFlashRaw.isPlaying())
      {
        soundPlayFlashRaw.play("HUM_0.RAW");
      }
      return;
    case ENT_CLASH:
      if (soundPlayFlashRaw.isPlaying())
      {
        soundPlayFlashRaw.stop();
      }
      clashId = random(1, 10);
      clashString = "CLASH_" + String(clashId) + "_0.RAW";
      soundPlayFlashFXRaw.play(clashString.c_str());
      return;
    case ENT_SWING:
      if (!soundPlayFlashFXRaw.isPlaying())
      {
        if (soundPlayFlashRaw.isPlaying())
          soundPlayFlashRaw.stop();

        swingId = random(1, 8);
        swingString = "SWING_" + String(swingId) + "_0.RAW";

        soundPlayFlashFXRaw.play(swingString.c_str());
      }
      return;
    case ENT_DISARM:
      soundPlayFlashRaw.play("POWEROFF_0.RAW");
      return;
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
    "COREAUDIO\0EVT_MUTE\0EVT_ARM\0EVT_SWING\0EVT_CLASH\0EVT_DISARM\0ELSE\0IDLE\0MUTE\0ARM\0ARMED\0CLASH\0SWING\0DISARM" );
  return *this;
}



