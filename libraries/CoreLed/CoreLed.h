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
  void reset_color_timer();
  void batteryCheck( void );
  void pulse(const ColorLed& cLed);

 private:
  enum { ENT_IDLE, LP_IDLE, ENT_RECHARGE, LP_RECHARGE, ENT_ARM, LP_ARM, EXT_ARM, ENT_ARMED, ENT_CLASH, ENT_SWING, ENT_DISARM }; // ACTIONS
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
  static constexpr int PULSE_DELAY = 30;
  static constexpr int PULSE_TIME = 500;
  CoreSettings* moduleSettings;
  int numberOfColorChanged = 0;
  bool batteryCharged = false;
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
        <EVT_ARMED>ARMED</EVT_ARM>
      </IDLE>
      <RECHARGE index="1" on_enter="ENT_RECHARGE" on_loop="LP_RECHARGE">
        <EVT_IDLE>IDLE</EVT_IDLE>
        <EVT_ARM>ARM</EVT_ARM>
      </RECHARGE>
      <ARM index="2" on_enter="ENT_ARM" on_loop="LP_ARM" on_exit="EXT_ARM">
        <EVT_ARMED>ARMED</EVT_ARMED>
      </ARM>
      <ARMED index="3" on_enter="ENT_ARMED">
        <EVT_SWING>SWING</EVT_SWING>
        <EVT_CLASH>CLASH</EVT_CLASH>
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

