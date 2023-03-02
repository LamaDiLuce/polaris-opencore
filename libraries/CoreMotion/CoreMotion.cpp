#include "CoreMotion.h"

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

CoreMotion& CoreMotion::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                 ON_ENTER    ON_LOOP  ON_EXIT  EVT_VOLUME  EVT_DISARM  EVT_SWING  EVT_CLASH  EVT_ARMED  EVT_ARM   ELSE */
    /*   IDLE */       ENT_IDLE,   LP_IDLE,      -1,       -1,         -1,        -1,        -1,     ARMED,     ARM,    -1,
    /*    ARM */        ENT_ARM,    LP_ARM,      -1,   VOLUME,     DISARM,        -1,        -1,     ARMED,      -1,    -1,
    /*  ARMED */      ENT_ARMED,  LP_ARMED,      -1,       -1,     DISARM,     SWING,     CLASH,        -1,      -1,    -1,
    /* DISARM */     ENT_DISARM,        -1,      -1,       -1,       IDLE,     ARMED,        -1,        -1,      -1,    -1,
    /*  CLASH */      ENT_CLASH,        -1,      -1,       -1,         -1,        -1,        -1,        -1,      -1, ARMED,
    /*  SWING */      ENT_SWING,        -1,      -1,       -1,         -1,        -1,     CLASH,     ARMED,      -1,    -1,
    /*   VOLUME */   ENT_VOLUME,        -1,      -1,       -1,         -1,        -1,        -1,        -1,      -1,   ARM,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );
  return *this;          
}

/* Add C++ code for each internally handled event (input) 
 * The code must return 1 to trigger the event
 */

int CoreMotion::event( int id ) {
  switch ( id ) {
    case EVT_VOLUME:
      return int1Status > 0;
    case EVT_DISARM:
      return timer_horizontal.expired( this );
    case EVT_SWING:
      return (swingSpeed > SWING_THRESHOLD ||
              rollSpeed > ROLL_SPEED_THRESHOLD_HIGH);
    case EVT_CLASH:
      return ( int1Status > 0 );
    case EVT_ARMED:
      return (
        (this->state() == this->ARM || this->state() == this->SWING) && 
        (
          timer_arm.expired(this) &&
          (swingSpeed < SWING_THRESHOLD) &&
          (rollSpeed < ROLL_SPEED_THRESHOLD_LOW)
        )
      ) ||
      (
        this->state() == this->IDLE && 
        abs(GyroZ) > ARM_ALT_THRESHOLD_Z && 
        swingSpeed < SWING_THRESHOLD_HIGH &&
        (!timer_no_vertical.expired( this )) &&
        ((digitalRead(USB_PIN) == LOW) || DEBUG)
      );
    case EVT_ARM:
      return (
              swingSpeed < SWING_THRESHOLD &&
              abs(GyroZ) > ARM_THRESHOLD_Z &&
              (!timer_vertical.expired( this )) &&
              ((digitalRead(USB_PIN) == LOW) || DEBUG)
             );
  }
  return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push( connectors, ON_ARM, 0, <v>, <up> );
 *   push( connectors, ON_ARMED, 0, <v>, <up> );
 *   push( connectors, ON_CLASH, 0, <v>, <up> );
 *   push( connectors, ON_DISARM, 0, <v>, <up> );
 *   push( connectors, ON_IDLE, 0, <v>, <up> );
 *   push( connectors, ON_MUTE, 0, <v>, <up> );
 *   push( connectors, ON_SWING, 0, <v>, <up> );
 */

void CoreMotion::action( int id ) {
  switch ( id ) {
    case ENT_IDLE:
      push( connectors, ON_IDLE, 0, 0, 0 );
      timer_vertical.setFromNow(this,0);
      timer_no_vertical.setFromNow(this,0);
      return;
    case LP_IDLE:
      if(AccelZ >= (VERTICAL_POSITION - TOLERANCE_VERTICAL_POSITION)) {
        timer_vertical.setFromNow(this,TIME_FOR_ALT_START_ARM);
      } 
      if(AccelZ < (VERTICAL_POSITION - TOLERANCE_VERTICAL_POSITION)) {
        timer_no_vertical.setFromNow(this,TIME_FOR_ALT_START_ARM);
      }
      return;
    case ENT_ARM:
      int1Status = 0;
      push( connectors, ON_ARM, 0, 0, 0 );
      timer_horizontal.setFromNow(this,TIME_FOR_DISARM);
      return;
    case LP_ARM:
      if ((AccelZ < (ARM_POSITION - TOLERANCE_POSITION)) || 
          (AccelZ > (ARM_POSITION + TOLERANCE_POSITION)) || 
          (swingSpeed > SWING_THRESHOLD))
      {
        timer_arm.setFromNow(this,TIME_FOR_CONFIRM_ARM);
      }
      if ((AccelZ < (HORIZONTAL_POSITION - TOLERANCE_POSITION)) || 
          (AccelZ > (HORIZONTAL_POSITION + TOLERANCE_POSITION)))
      {
        timer_horizontal.setFromNow(this,TIME_FOR_DISARM);
      }
      return;
    case ENT_ARMED:
      int1Status = 0;
      push( connectors, ON_ARMED, 0, 0, 0 );
      timer_horizontal.setFromNow(this,TIME_FOR_DISARM);
      return;
    case LP_ARMED:
      if ((AccelZ < (HORIZONTAL_POSITION - TOLERANCE_POSITION)) || 
          (AccelZ > (HORIZONTAL_POSITION + TOLERANCE_POSITION)))
      {
        timer_horizontal.setFromNow(this,TIME_FOR_DISARM);
      }
      return;
    case ENT_DISARM:
      push( connectors, ON_DISARM, 0, 0, 0 );
      timer_horizontal.setFromNow(this,TIME_FOR_REARM);
      return;
    case ENT_CLASH:
      int1Status = 0;
      push( connectors, ON_CLASH, 0, 0, 0 );
      return;
    case ENT_SWING:
      timer_arm.set(0); // otherwise there is a delay in triggering the ARMED event. This should be not needed. To be investigated.
      push( connectors, ON_SWING, 0, 0, 0 );
      return;
    case ENT_VOLUME:
      int1Status = 0;
      push( connectors, ON_MUTE, 0, 0, 0 );
      return;
  }
}

void CoreMotion::setGyroX(float value){
  GyroX = value;
}

void CoreMotion::setGyroY(float value){
  GyroY = value;
}

void CoreMotion::setGyroZ(float value){
  GyroZ = value;
}

void CoreMotion::setAccelX(float value){
  AccelX = value;
}

void CoreMotion::setAccelY(float value){
  AccelY = value;
}

void CoreMotion::setAccelZ(float value){
  AccelZ = value;
}

void CoreMotion::setSwingSpeed(float value){
  swingSpeed = value;
}

void CoreMotion::setRollSpeed(float value){
  rollSpeed = value;
}

void CoreMotion::incInt1Status( void ){
  int1Status++;
}

/* Optionally override the default trigger() method
 * Control how your machine processes triggers
 */

CoreMotion& CoreMotion::trigger( int event ) {
  Machine::trigger( event );
  return *this;
}

/* Optionally override the default state() method
 * Control what the machine returns when another process requests its state
 */

int CoreMotion::state( void ) {
  return Machine::state();
}

/* Nothing customizable below this line                          
 ************************************************************************************************
*/

/* Public event methods
 *
 */

CoreMotion& CoreMotion::volume() {
  trigger( EVT_VOLUME );
  return *this;
}

CoreMotion& CoreMotion::disarm() {
  trigger( EVT_DISARM );
  return *this;
}

CoreMotion& CoreMotion::swing() {
  trigger( EVT_SWING );
  return *this;
}

CoreMotion& CoreMotion::clash() {
  trigger( EVT_CLASH );
  return *this;
}

CoreMotion& CoreMotion::armed() {
  trigger( EVT_ARMED );
  return *this;
}

CoreMotion& CoreMotion::arm() {
  trigger( EVT_ARM );
  return *this;
}

/*
 * onArm() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreMotion& CoreMotion::onArm( Machine& machine, int event ) {
  onPush( connectors, ON_ARM, 0, 1, 1, machine, event );
  return *this;
}

CoreMotion& CoreMotion::onArm( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_ARM, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onArmed() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreMotion& CoreMotion::onArmed( Machine& machine, int event ) {
  onPush( connectors, ON_ARMED, 0, 1, 1, machine, event );
  return *this;
}

CoreMotion& CoreMotion::onArmed( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_ARMED, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onClash() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreMotion& CoreMotion::onClash( Machine& machine, int event ) {
  onPush( connectors, ON_CLASH, 0, 1, 1, machine, event );
  return *this;
}

CoreMotion& CoreMotion::onClash( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_CLASH, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onDisarm() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreMotion& CoreMotion::onDisarm( Machine& machine, int event ) {
  onPush( connectors, ON_DISARM, 0, 1, 1, machine, event );
  return *this;
}

CoreMotion& CoreMotion::onDisarm( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_DISARM, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onIdle() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreMotion& CoreMotion::onIdle( Machine& machine, int event ) {
  onPush( connectors, ON_IDLE, 0, 1, 1, machine, event );
  return *this;
}

CoreMotion& CoreMotion::onIdle( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_IDLE, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onMute() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreMotion& CoreMotion::onMute( Machine& machine, int event ) {
  onPush( connectors, ON_MUTE, 0, 1, 1, machine, event );
  return *this;
}

CoreMotion& CoreMotion::onMute( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_MUTE, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onSwing() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreMotion& CoreMotion::onSwing( Machine& machine, int event ) {
  onPush( connectors, ON_SWING, 0, 1, 1, machine, event );
  return *this;
}

CoreMotion& CoreMotion::onSwing( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_SWING, 0, 1, 1, callback, idx );
  return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */

CoreMotion& CoreMotion::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "COREMOTION\0EVT_MUTE\0EVT_DISARM\0EVT_SWING\0EVT_CLASH\0EVT_ARMED\0EVT_ARM\0ELSE\0IDLE\0ARM\0ARMED\0DISARM\0CLASH\0SWING\0MUTE" );
  return *this;
}



