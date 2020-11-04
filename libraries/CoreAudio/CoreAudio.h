#pragma once

#include <Automaton.h>
#include <Audio.h>

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

class CoreAudio: public Machine {

 public:
  enum { IDLE, MUTE, ARM, ARMED, CLASH, SWING, DISARM }; // STATES
  enum { EVT_MUTE, EVT_ARM, EVT_SWING, EVT_CLASH, EVT_DISARM, ELSE }; // EVENTS
  CoreAudio( void ) : Machine() {};
  CoreAudio& begin( void );
  CoreAudio& trace( Stream & stream );
  CoreAudio& trigger( int event );
  int state( void );
  CoreAudio& mute( void );
  CoreAudio& arm( void );
  CoreAudio& swing( void );
  CoreAudio& clash( void );
  CoreAudio& disarm( void );
  void beep( void );

 private:
  enum { ENT_IDLE, LP_IDLE, EXT_IDLE, ENT_MUTE, ENT_ARM, LP_ARMED, ENT_CLASH, ENT_SWING, ENT_DISARM }; // ACTIONS
  int event( int id ); 
  void action( int id ); 
  
  int clashId = 1;
  String clashString;
  int swingId = 1;
  String swingString;
  AudioSynthWaveformSine soundSine;
  AudioPlaySerialflashRaw soundPlayFlashRaw;
  AudioPlaySerialflashRaw soundPlayFlashFXRaw;
  AudioMixer4 mainMixer;
  AudioOutputAnalog outputDac;
  AudioConnection* patchSineMixer = nullptr;
  AudioConnection* patchFlashMixer = nullptr;
  AudioConnection* patchFlashFXMixer = nullptr;
  AudioConnection* patchMixerDac = nullptr;
  SerialFlashFile file;
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
      <SWING index="5" on_enter="ENT_SWING">
        <EVT_CLASH>CLASH</EVT_CLASH>
        <ELSE>ARMED</ELSE>
      </SWING>
      <DISARM index="6" on_enter="ENT_DISARM">
        <ELSE>IDLE</ELSE>
      </DISARM>
    </states>
    <events>
      <EVT_MUTE index="0" access="MIXED"/>
      <EVT_ARM index="1" access="MIXED"/>
      <EVT_SWING index="2" access="MIXED"/>
      <EVT_CLASH index="3" access="MIXED"/>
      <EVT_DISARM index="4" access="MIXED"/>
    </events>
    <connectors>
    </connectors>
    <methods>
    </methods>
  </machine>
</machines>

Automaton::ATML::end
*/

