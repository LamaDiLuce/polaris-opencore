
#pragma once

#include <Automaton.h>
#include <Audio.h>
#include "CoreLogging.h"

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

#define BEEP_TIME 125
#define BEEP_FREQUENCY 1000
#define AUDIO_BLOCK 16

class CoreAudio: public Machine {

 public:
  enum { IDLE, MUTE, ARM, ARMED, CLASH, SWING, DISARM }; // STATES
  enum { EVT_MUTE, EVT_ARM, EVT_ARMED, EVT_SWING, EVT_CLASH, EVT_DISARM, ELSE }; // EVENTS
  CoreAudio( void ) : Machine() {};
  CoreAudio& begin( void );
  CoreAudio& trace( Stream & stream );
  CoreAudio& trigger( int event );
  int state( void );
  CoreAudio& mute( void );
  CoreAudio& arm( void );
  CoreAudio& armed( void );
  CoreAudio& swing( void );
  CoreAudio& clash( void );
  CoreAudio& disarm( void );
  void beep( void );
  void setSwingSpeed(float s);
  static constexpr bool USE_SMOOTH_SWING = true;
  // SmoothSwing V2, based on Thexter's excellent work.
  // For more details, see:
  // http://therebelarmory.com/thread/9138/smoothswing-v2-algorithm-description



 private:
  enum { ENT_IDLE, LP_IDLE, EXT_IDLE, ENT_MUTE, ENT_ARM, LP_ARMED, ENT_CLASH, ENT_SWING, LP_SWING, ENT_DISARM }; // ACTIONS
  int event( int id ); 
  void action( int id ); 
  int clashId = 1;
  String clashString;
  int swingId = 1;
  String swingString;
  String smoothSwingStringH;
  String smoothSwingStringL;
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
  float swingSpeed = 0;
  float totalRotation = 0;
  float transitionVolume = 0;
  float transition1midPoint = 0;
  float transition2midPoint = 0;
  float swingStrength = 0;
  float humVolume = MAX_VOLUME;
  float swingVolumeA = 0;
  float swingVolumeB = 0;
  // Params that can be tuned
  static constexpr float MAX_VOLUME = 0.7;              // 1 in the max volume. Use a lower number to be more quite
  static constexpr float SWING_SENSITIVITY = 600;       // how hard should be a swing to get the max effect. It's in deg/s
  static constexpr float MAXIMUM_HUM_DUCKING = 0.9;     // how much the main hum is reduced during the swing. Close to 1 means that the main hum is attuanuated a lot during the swing
  static constexpr float SWING_SHARPNESS = 1.2;         // This gives a nice non-linear swing response. Between 1 and 2
  static constexpr float TRANSITION_1_MIN = 40;         // min midpoint angle for first transition
  static constexpr float TRANSITION_1_MAX = 80;         // max midpoint angle for first transition
  static constexpr float TRANSITION_1_WIDTH = 45.0;
  static constexpr float TRANSITION_2_WIDTH = 160.0;
};

/* 
Automaton::ATML::begin - Automaton Markup Language

<?xml version="1.0" encoding="UTF-8"?>
<machines>
  <machine name="CoreAudio">
    <states>
      <IDLE index="0" sleep="1" on_enter="ENT_IDLE" on_loop="LP_IDLE" on_exit="EXT_IDLE">
        <EVT_MUTE>MUTE</EVT_MUTE>
        <EVT_ARM>ARM</EVT_ARM>
      </IDLE>
      <MUTE index="1" on_enter="ENT_MUTE">
        <EVT_DISARM>IDLE</EVT_DISARM>
      </MUTE>
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
        <EVT_SWING>SWING</EVT_SWING>
        <EVT_CLASH>CLASH</EVT_CLASH>
      </SWING>
      <DISARM index="6" on_enter="ENT_DISARM">
        <ELSE>IDLE</ELSE>
      </DISARM>
    </states>
    <events>
      <EVT_MUTE index="0" access="MIXED"/>
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
