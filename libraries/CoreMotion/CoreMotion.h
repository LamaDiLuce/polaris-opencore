#pragma once

#include <Automaton.h>
#include "CoreCommon.h"

class CoreMotion: public Machine {

 public:
  enum { IDLE, ARM, ARMED, DISARM, CLASH, SWING, VOLUME }; // STATES
  enum { EVT_VOLUME, EVT_DISARM, EVT_SWING, EVT_CLASH, EVT_ARMED, EVT_ARM, ELSE }; // EVENTS
  CoreMotion( void ) : Machine() {};
  CoreMotion& begin( void );
  CoreMotion& trace( Stream & stream );
  CoreMotion& trigger( int event );
  int state( void );
  CoreMotion& onArm( Machine& machine, int event = 0 );
  CoreMotion& onArm( atm_cb_push_t callback, int idx = 0 );
  CoreMotion& onArmed( Machine& machine, int event = 0 );
  CoreMotion& onArmed( atm_cb_push_t callback, int idx = 0 );
  CoreMotion& onClash( Machine& machine, int event = 0 );
  CoreMotion& onClash( atm_cb_push_t callback, int idx = 0 );
  CoreMotion& onDisarm( Machine& machine, int event = 0 );
  CoreMotion& onDisarm( atm_cb_push_t callback, int idx = 0 );
  CoreMotion& onIdle( Machine& machine, int event = 0 );
  CoreMotion& onIdle( atm_cb_push_t callback, int idx = 0 );
  CoreMotion& onMute( Machine& machine, int event = 0 );
  CoreMotion& onMute( atm_cb_push_t callback, int idx = 0 );
  CoreMotion& onSwing( Machine& machine, int event = 0 );
  CoreMotion& onSwing( atm_cb_push_t callback, int idx = 0 );
  CoreMotion& volume( void );
  CoreMotion& disarm( void );
  CoreMotion& swing( void );
  CoreMotion& clash( void );
  CoreMotion& armed( void );
  CoreMotion& arm( void );
  void setGyroX(float value);
  void setGyroY(float value);
  void setGyroZ(float value);
  void setAccelX(float value);
  void setAccelY(float value);
  void setAccelZ(float value);
  void setSwingSpeed(float value);
  void setRollSpeed(float value);
  void incInt1Status( void );

 private:
  enum { ENT_IDLE, LP_IDLE, ENT_ARM, LP_ARM, ENT_ARMED, LP_ARMED, ENT_DISARM, LP_DISARM, ENT_CLASH, ENT_SWING, ENT_VOLUME }; // ACTIONS
  enum { ON_ARM, ON_ARMED, ON_CLASH, ON_DISARM, ON_IDLE, ON_MUTE, ON_SWING, CONN_MAX }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  int event( int id ); 
  void action( int id );
  atm_timer_millis timer_vertical;
  atm_timer_millis timer_no_vertical;
  atm_timer_millis timer_horizontal;
  atm_timer_millis timer_arm;
  float AccelX;
  float AccelY;
  float AccelZ;
  float GyroX;
  float GyroY;
  float GyroZ;
  float swingSpeed;
  float rollSpeed;
  uint8_t int1Status;
  bool firstArm;
  static constexpr int SWING_THRESHOLD = 80;
  static constexpr int SWING_THRESHOLD_HIGH = 300;
  static constexpr int ROLL_SPEED_THRESHOLD_LOW = 40;
  static constexpr int ROLL_SPEED_THRESHOLD_HIGH = 150;
  static constexpr int ARM_THRESHOLD_Z = 300;
  static constexpr int ARM_ALT_THRESHOLD_Z = 1000;
  static constexpr int ARM_THRESHOLD_XY = 100;
  static constexpr float VERTICAL_POSITION = 8.0;
  static constexpr float ARM_POSITION = -6.0;
  static constexpr float HORIZONTAL_POSITION = 0.0;
  static constexpr float TOLERANCE_POSITION = 2;
  static constexpr float TOLERANCE_VERTICAL_POSITION = 0.25;
  static constexpr int TIME_FOR_START_ARM = 500;
  static constexpr int TIME_FOR_ALT_START_ARM = 300;
  static constexpr int TIME_FOR_DISARM = 3000;
  static constexpr int TIME_FOR_CONFIRM_ARM = 200;
  static constexpr int TIME_FOR_REARM = 0; // set to 0 to disable rearm possibility during disarm
};

/* 
Automaton::ATML::begin - Automaton Markup Language

<?xml version="1.0" encoding="UTF-8"?>
<machines>
  <machine name="CoreMotion">
    <states>
      <IDLE index="0" sleep="1" on_enter="ENT_IDLE" on_loop="LP_IDLE">
        <EVT_ARM>ARM</EVT_ARM>
        <EVT_ARMED>ARMED</EVT_ARM>
      </IDLE>
      <ARM index="1" on_enter="ENT_ARM" on_loop="LP_ARM">
        <EVT_VOLUME>VOLUME</EVT_VOLUME>
        <EVT_ARMED>ARMED</EVT_ARMED>
        <EVT_DISARM>DISARM</EVT_ARMED>
      </ARM>
      <ARMED index="2" on_enter="ENT_ARMED" on_loop="LP_ARMED">
        <EVT_DISARM>DISARM</EVT_DISARM>
        <EVT_SWING>SWING</EVT_SWING>
        <EVT_CLASH>CLASH</EVT_CLASH>
      </ARMED>
      <DISARM index="3" on_enter="ENT_DISARM" on_loop="LP_DISARM">
        <EVT_DISARM>IDLE</EVT_DISARM>
        <EVT_SWING>ARMED</EVT_SWING>
      </DISARM>
      <CLASH index="4" on_enter="ENT_CLASH">
        <ELSE>ARMED</ELSE>
      </CLASH>
      <SWING index="5" on_enter="ENT_SWING">
        <EVT_CLASH>CLASH</EVT_CLASH>
        <EVT_ARMED>ARMED</EVT_ARMED>
      </SWING>
      <VOLUME index="6" on_enter="ENT_VOLUME">
        <ELSE>ARM</ELSE>
      </VOLUME>
    </states>
    <events>
      <EVT_VOLUME index="0" access="MIXED"/>
      <EVT_DISARM index="1" access="MIXED"/>
      <EVT_SWING index="2" access="MIXED"/>
      <EVT_CLASH index="3" access="MIXED"/>
      <EVT_ARMED index="4" access="MIXED"/>
      <EVT_ARM index="5" access="MIXED"/>
    </events>
    <connectors>
      <ARM autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <ARMED autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <CLASH autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <DISARM autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <IDLE autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <VOLUME autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <SWING autostore="0" broadcast="0" dir="PUSH" slots="1"/>
    </connectors>
    <methods>
    </methods>
  </machine>
</machines>

Automaton::ATML::end 
*/
