#pragma once

#include <Automaton.h>
#include "CoreLogging.h"
#include "CoreSettings.h"

class CoreLed: public Machine {

 public:
  enum { IDLE, RECHARGE, ARM, ARMED, CLASH, SWING, DISARM }; // STATES
  enum { EVT_IDLE, EVT_RECHARGE, EVT_ARM, EVT_ARMED, EVT_SWING, EVT_CLASH, EVT_DISARM, ELSE }; // EVENTS
  CoreLed( void ) : Machine() {};
  CoreLed& begin( CoreSettings* cSet );
  CoreLed& trace( Stream & stream );
  CoreLed& trigger( int event );
  int state( void );
  CoreLed& onArm( Machine& machine, int event = 0 );
  CoreLed& onArm( atm_cb_push_t callback, int idx = 0 );
  CoreLed& onArmed( Machine& machine, int event = 0 );
  CoreLed& onArmed( atm_cb_push_t callback, int idx = 0 );
  CoreLed& onClash( Machine& machine, int event = 0 );
  CoreLed& onClash( atm_cb_push_t callback, int idx = 0 );
  CoreLed& onDisarm( Machine& machine, int event = 0 );
  CoreLed& onDisarm( atm_cb_push_t callback, int idx = 0 );
  CoreLed& onNextcolor( Machine& machine, int event = 0 );
  CoreLed& onNextcolor( atm_cb_push_t callback, int idx = 0 );
  CoreLed& onRecharge( Machine& machine, int event = 0 );
  CoreLed& onRecharge( atm_cb_push_t callback, int idx = 0 );
  CoreLed& onSwing( Machine& machine, int event = 0 );
  CoreLed& onSwing( atm_cb_push_t callback, int idx = 0 );
  CoreLed& idle( void );
  CoreLed& recharge( void );
  CoreLed& arm( void );
  CoreLed& armed( void );
  CoreLed& swing( void );
  CoreLed& clash( void );
  CoreLed& disarm( void );
  void setAccelX(float value);
  void setAccelY(float value);
  void setAccelZ(float value);

 private:
  enum { ENT_IDLE, LP_IDLE, ENT_RECHARGE, LP_RECHARGE, ENT_ARM, LP_ARM, EXT_ARM, ENT_ARMED, LP_ARMED, ENT_CLASH, ENT_SWING, ENT_DISARM }; // ACTIONS
  enum { ON_ARM, ON_ARMED, ON_CLASH, ON_DISARM, ON_NEXTCOLOR, ON_RECHARGE, ON_SWING, CONN_MAX }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  int event( int id );
  void action( int id );
  void turnOff();
  String decodeColorSetId(int colorSetId);
  void getCurrentColorSet();
  void setCurrentColorSet(int colorSetId);
  void changeColor(const ColorLed& cLed);
  void fadeIn();
  void fadeOut();
  atm_timer_millis timer_blink;
  atm_timer_millis timer_color_selection;
  int currentColorSetId = OFF;
  int nextColorSetId = OFF;
  ColorLed mainColor;
  ColorLed clashColor;
  ColorLed swingColor;
  ColorLed singleStepColor;
  static constexpr int BLINK_RECHARGE_STATUS_TIMER = 8000;
  static constexpr int BLINK_RECHARGED_STATUS_TIMER = 12000;
  static constexpr int CHARGE_SEQUENCE_BLINK_TIME = 300;
  static constexpr int RECHARGING_BLINK_TIME = 300;
  static constexpr int ARMING_BLINK_TIME = 200;
  static constexpr int TIME_FOR_START_COLOR_SELECTION = 5000;
  static constexpr int TIME_FOR_COLOR_SELECTION = 1000;
  static constexpr int COLOR_SELECTION_BLINK_TIME = 500;
  static constexpr int CLASH_BLINK_TIME = 200;
  static constexpr int FADE_DELAY = 30;
  static constexpr int FADE_IN_TIME = 300;
  static constexpr int FADE_OUT_TIME = 1000;
  CoreSettings* moduleSettings;
  int numberOfColorChanged = 0;  
  float AccelX;
  float AccelY;
  float AccelZ;
  const int lights[360]={
    0,   0,   0,   0,   0,   1,   1,   2, 
    2,   3,   4,   5,   6,   7,   8,   9, 
  11,  12,  13,  15,  17,  18,  20,  22, 
  24,  26,  28,  30,  32,  35,  37,  39, 
  42,  44,  47,  49,  52,  55,  58,  60, 
  63,  66,  69,  72,  75,  78,  81,  85, 
  88,  91,  94,  97, 101, 104, 107, 111, 
  114, 117, 121, 124, 127, 131, 134, 137, 
  141, 144, 147, 150, 154, 157, 160, 163, 
  167, 170, 173, 176, 179, 182, 185, 188, 
  191, 194, 197, 200, 202, 205, 208, 210, 
  213, 215, 217, 220, 222, 224, 226, 229, 
  231, 232, 234, 236, 238, 239, 241, 242, 
  244, 245, 246, 248, 249, 250, 251, 251, 
  252, 253, 253, 254, 254, 255, 255, 255, 
  255, 255, 255, 255, 254, 254, 253, 253, 
  252, 251, 251, 250, 249, 248, 246, 245, 
  244, 242, 241, 239, 238, 236, 234, 232, 
  231, 229, 226, 224, 222, 220, 217, 215, 
  213, 210, 208, 205, 202, 200, 197, 194, 
  191, 188, 185, 182, 179, 176, 173, 170, 
  167, 163, 160, 157, 154, 150, 147, 144, 
  141, 137, 134, 131, 127, 124, 121, 117, 
  114, 111, 107, 104, 101,  97,  94,  91, 
  88,  85,  81,  78,  75,  72,  69,  66, 
  63,  60,  58,  55,  52,  49,  47,  44, 
  42,  39,  37,  35,  32,  30,  28,  26, 
  24,  22,  20,  18,  17,  15,  13,  12, 
  11,   9,   8,   7,   6,   5,   4,   3, 
    2,   2,   1,   1,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0};
};

/* 
Automaton::ATML::begin - Automaton Markup Language

<?xml version="1.0" encoding="UTF-8"?>
<machines>
  <machine name="CoreLed">
    <states>
      <IDLE index="0" on_enter="ENT_IDLE" on_loop="LP_IDLE">
        <EVT_RECHARGE>RECHARGE</EVT_RECHARGE>
        <EVT_ARM>ARM</EVT_ARM>
      </IDLE>
      <RECHARGE index="1" on_enter="ENT_RECHARGE" on_loop="LP_RECHARGE">
        <EVT_IDLE>IDLE</EVT_IDLE>
        <EVT_ARM>ARM</EVT_ARM>
      </RECHARGE>
      <ARM index="2" on_enter="ENT_ARM" on_loop="LP_ARM" on_exit="EXT_ARM">
        <EVT_ARMED>ARMED</EVT_ARMED>
      </ARM>
      <ARMED index="3" on_enter="ENT_ARMED" on_loop="LP_ARMED">
        <EVT_DISARM>DISARM</EVT_DISARM>
      </ARMED>
      <CLASH index="4" on_enter="ENT_CLASH">
        <EVT_ARMED>ARMED</EVT_ARMED>
      </CLASH>
      <SWING index="5" on_enter="ENT_SWING">
        <EVT_ARMED>ARMED</EVT_ARMED>
        <EVT_CLASH>CLASH</EVT_CLASH>
      </SWING>
      <DISARM index="6" on_enter="ENT_DISARM">
        <EVT_ARMED>ARMED</EVT_ARMED>
        <EVT_DISARM>IDLE</EVT_DISARM>
      </DISARM>
    </states>
    <events>
      <EVT_IDLE index="0" access="MIXED"/>
      <EVT_RECHARGE index="1" access="MIXED"/>
      <EVT_ARM index="2" access="MIXED"/>
      <EVT_ARMED index="3" access="MIXED"/>
      <EVT_SWING index="4" access="MIXED"/>
      <EVT_CLASH index="5" access="MIXED"/>
      <EVT_DISARM index="6" access="MIXED"/>
    </events>
    <connectors>
      <ARM autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <ARMED autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <CLASH autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <DISARM autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <NEXTCOLOR autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <RECHARGE autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <SWING autostore="0" broadcast="0" dir="PUSH" slots="1"/>
    </connectors>
    <methods>
    </methods>
  </machine>
</machines>

Automaton::ATML::end 
*/

