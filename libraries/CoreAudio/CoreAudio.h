
#pragma once

#include <Automaton.h>
#include <Audio.h>
#include "CoreLogging.h"
#include "CoreSettings.h"

#define CS_PIN 10
#define SI_PIN 11
#define SO_PIN 12
#define SCK_PIN 14

#define POWER_AMP_PIN 16

#define CHANNEL_HUM 0
#define CHANNEL_SINE 1
#define CHANNEL_FX 2
#define CHANNEL_SMOOTH_SWING_A 3
#define CHANNEL_SMOOTH_SWING_B 4

#define BEEP_FREQUENCY 1000
#define AUDIO_BLOCK 16

class CoreAudio: public Machine {

 public:
  enum { IDLE, VOLUME, ARM, ARMED, CLASH, SWING, DISARM }; // STATES
  enum { EVT_VOLUME, EVT_ARM, EVT_ARMED, EVT_SWING, EVT_CLASH, EVT_DISARM, ELSE }; // EVENTS
  CoreAudio( void ) : Machine() {};
  CoreAudio& begin( CoreSettings* cSet );
  CoreAudio& trace( Stream & stream );
  CoreAudio& trigger( int event );
  int state( void );
  CoreAudio& volume( void );
  CoreAudio& arm( void );
  CoreAudio& armed( void );
  CoreAudio& swing( void );
  CoreAudio& clash( void );
  CoreAudio& disarm( void );
  void set_mute();
  void beep(int duration, float volume);
  void treeplebeep(int duration, float volume);
  void setSwingSpeed(float s);
  void setRollSpeed(float s);
  void setAngDotProduct(float s);
  bool checkSmoothSwing( void );

 private:
  enum { ENT_IDLE, LP_IDLE, EXT_IDLE, ENT_VOLUME, ENT_ARM, LP_ARMED, ENT_CLASH, ENT_SWING, LP_SWING, ENT_DISARM }; // ACTIONS
  int event( int id ); 
  void action( int id );
  void resetSmoothSwing( void );
  CoreSettings* moduleSettings;
  AudioSynthWaveformSine soundSine;
  AudioPlaySerialflashRaw soundPlayFlashRaw;
  AudioPlaySerialflashRaw soundPlayFlashFXRaw;
  AudioPlaySerialflashRaw soundPlayFlashSmoothSwingARaw;
  AudioPlaySerialflashRaw soundPlayFlashSmoothSwingBRaw;
  AudioMixer4 mainMixer;
  AudioOutputAnalog outputDac;
  AudioConnection* patchSineMixer = nullptr;
  AudioConnection* patchFlashMixer = nullptr;
  AudioConnection* patchFlashFXMixer = nullptr;
  AudioConnection* patchFlashSmoothSwingAMixer = nullptr;
  AudioConnection* patchFlashSmoothSwingBMixer = nullptr;
  AudioConnection* patchMixerDac = nullptr;
  SerialFlashFile file;
  uint32_t t0;
  uint32_t delta;
  uint32_t t1;
  String humString;
  String smoothSwingStringA;
  String smoothSwingStringB;
  float swingSpeed = 0;
  float rollSpeed = 0;
  float totalRotation = 0;
  float transitionVolume = 0;
  float transition1midPoint = 0;
  float transition2midPoint = 0;
  float swingStrength = 0;
  float humVolume = 0;
  float swingVolumeA = 0;
  float swingVolumeB = 0;
  float AngDotProduct = 0;
  bool powerSwing = false;
  // Params that can be tuned
  static constexpr float MAX_VOLUME = 1;                // 1 is the max volume. Use a lower number to be more quite e.g. at home
  float currentVolume = MAX_VOLUME;                            // Initial volume. Initally setting to max volume; may later load this from stored config
  bool firstTap = true;                                 // used to check for the first tap of a mute cycle
  bool useSmoothSwing = true;                           // smoothswing is used by default of proper files are loaded. If no smoothswing are present, then the normal swing is used automatically
                                                        // SmoothSwing V2, based on Thexter's excellent work.
                                                        // For more details, see:
                                                        // http://therebelarmory.com/thread/9138/smoothswing-v2-algorithm-description
  static constexpr float SWING_SENSITIVITY = 520;       // how hard should be a swing to get the max effect. It's in deg/s (good values are between 360 and 720)
  static constexpr float ROLL_SENSITIVITY = 600;        // how hard should be a roll swing to get the max effect. It's in deg/s
  static constexpr float MAXIMUM_HUM_DUCKING = 0.85;    // how much the main hum is reduced during the swing. Close to 1 means that the main hum is attuanuated a lot during the swing (good values between 0.7 and 0.9)
  static constexpr float SWING_SHARPNESS = 1.2;         // This gives a nice non-linear swing response. Between 1 and 2
  static constexpr float TRANSITION_1_MIN = 30;         // min midpoint angle in degreese for first transition (it's picked randomly at each swing)
  static constexpr float TRANSITION_1_MAX = 50;         // max midpoint angle in degreese for first transition (it's picked randomly at each swing)
  static constexpr float TRANSITION_1_WIDTH = 60.0;     // width angle in degreese of the first trasition 
  static constexpr float TRANSITION_2_WIDTH = 160.0;    // width angle in degreese of the second trasition, which is 180 deg away from the first transition
  static constexpr float SMOOTHING_FACTOR = 0.2;
};

/* 
Automaton::ATML::begin - Automaton Markup Language

<?xml version="1.0" encoding="UTF-8"?>
<machines>
  <machine name="CoreAudio">
    <states>
      <IDLE index="0" sleep="1" on_enter="ENT_IDLE" on_loop="LP_IDLE" on_exit="EXT_IDLE">
        <EVT_VOLUME>VOLUME</EVT_VOLUME>
        <EVT_ARM>ARM</EVT_ARM>
      </IDLE>
      <VOLUME index="1" on_enter="ENT_VOLUME">
        <EVT_DISARM>IDLE</EVT_DISARM>
      </VOLUME>
      <ARM index="2" on_enter="ENT_ARM">
        <ELSE>ARMED</ELSE>
      </ARM>
      <ARMED index="3" on_loop="LP_ARMED">
        <EVT_SWING>SWING</EVT_SWING>
        <EVT_CLASH>CLASH</EVT_CLASH>
        <EVT_DISARM>DISARM</EVT_DISARM>
      </ARMED>
      <CLASH index="4" on_enter="ENT_CLASH">
        <ELSE>ARMED</ELSE>
      </CLASH>
      <SWING index="5" on_enter="ENT_SWING" on_loop="LP_SWING">
        <EVT_ARMED>ARMED</EVT_ARMED>
        <EVT_CLASH>CLASH</EVT_CLASH>
      </SWING>
      <DISARM index="6" on_enter="ENT_DISARM">
        <EVT_ARMED>ARM</EVT_ARMED>
        <EVT_DISARM>IDLE</EVT_DISARM>
      </DISARM>
    </states>
    <events>
      <EVT_VOLUME index="0" access="MIXED"/>
      <EVT_ARM index="1" access="MIXED"/>
      <EVT_ARMED index="2" access="MIXED"/>
      <EVT_SWING index="3" access="MIXED"/>
      <EVT_CLASH index="4" access="MIXED"/>
      <EVT_DISARM index="5" access="MIXED"/>
    </events>
    <connectors>
    </connectors>
    <methods>
    </methods>
  </machine>
</machines>

Automaton::ATML::end 
*/
