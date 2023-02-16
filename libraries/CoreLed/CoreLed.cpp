#include "CoreLed.h"

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

CoreLed& CoreLed::begin(CoreSettings* cSet) {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                 ON_ENTER      ON_LOOP  ON_EXIT  EVT_IDLE  EVT_RECHARGE  EVT_ARM  EVT_ARMED  EVT_SWING  EVT_CLASH  EVT_DISARM   ELSE */
    /*     IDLE */     ENT_IDLE,     LP_IDLE,      -1,       -1,     RECHARGE,     ARM,     ARMED,        -1,        -1,         -1,    -1,   
    /* RECHARGE */ ENT_RECHARGE, LP_RECHARGE,      -1,     IDLE,           -1,     ARM,        -1,        -1,        -1,         -1,    -1,
    /*      ARM */      ENT_ARM,      LP_ARM, EXT_ARM,       -1,           -1,      -1,     ARMED,        -1,        -1,         -1,    -1,
    /*    ARMED */    ENT_ARMED,          -1,      -1,       -1,           -1,      -1,        -1,     SWING,     CLASH,     DISARM,    -1,
    /*    CLASH */    ENT_CLASH,          -1,      -1,       -1,           -1,      -1,     ARMED,        -1,        -1,         -1,    -1,
    /*    SWING */    ENT_SWING,          -1,      -1,       -1,           -1,      -1,     ARMED,        -1,     CLASH,         -1,    -1,
    /*   DISARM */   ENT_DISARM,          -1,      -1,       -1,           -1,      -1,     ARMED,        -1,        -1,       IDLE,    -1,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_WHITE, OUTPUT);
  pinMode(CHARGE_PIN, INPUT_PULLUP);
  pinMode(STANDBY_PIN, INPUT_PULLUP);
  pinMode(USB_PIN, INPUT_PULLDOWN);
  moduleSettings = cSet;
  moduleSettings->init();
  return *this;          
}

/* Add C++ code for each internally handled event (input) 
 * The code must return 1 to trigger the event
 */

int CoreLed::event( int id ) {
  switch ( id ) {
    case EVT_IDLE:
      return (digitalRead(USB_PIN) == LOW);
    case EVT_RECHARGE:
      return (digitalRead(USB_PIN) == HIGH);
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
 *
 * Available connectors:
 *   push( connectors, ON_ARM, 0, <v>, <up> );
 *   push( connectors, ON_ARMED, 0, <v>, <up> );
 *   push( connectors, ON_CLASH, 0, <v>, <up> );
 *   push( connectors, ON_DISARM, 0, <v>, <up> );
 *   push( connectors, ON_NEXTCOLOR, 0, <v>, <up> );
 *   push( connectors, ON_RECHARGE, 0, <v>, <up> );
 *   push( connectors, ON_SWING, 0, <v>, <up> );
 */

void CoreLed::action( int id ) {
  switch ( id ) {
    case ENT_IDLE:
      turnOff();
      batteryCharged = false;
      return;
    case LP_IDLE:
      return;
    case ENT_RECHARGE:
      changeColor({255,0,0,0}); // RED
      delay(CHARGE_SEQUENCE_BLINK_TIME);
      changeColor({0,255,0,0}); // GREEN
      delay(CHARGE_SEQUENCE_BLINK_TIME);
      changeColor({0,0,255,0}); // BLUE
      delay(CHARGE_SEQUENCE_BLINK_TIME);
      changeColor({0,0,0,255}); // WHITE
      delay(CHARGE_SEQUENCE_BLINK_TIME);
      turnOff();
      timer_blink.setFromNow(this,BLINK_RECHARGE_STATUS_TIMER);
      batteryCharged = false;
      return;
    case LP_RECHARGE:
      if (timer_blink.expired( this ))
      {
        if (digitalRead(STANDBY_PIN) == LOW || batteryCharged)
        {
          batteryCharged = true;
          pulse({0,0,255,0}); // BLUE
          timer_blink.setFromNow(this,BLINK_RECHARGED_STATUS_TIMER);
        }
        else if (digitalRead(CHARGE_PIN) == LOW)
        {
          pulse({255,0,0,0}); // RED
          timer_blink.setFromNow(this,BLINK_RECHARGE_STATUS_TIMER);
        }
      }
      return;
    case ENT_ARM:
      getCurrentColorSet();
      changeColor(mainColor);
      delay(ARMING_BLINK_TIME);
      turnOff();
      timer_color_selection.setFromNow(this,TIME_FOR_START_COLOR_SELECTION);
      numberOfColorChanged = 0;
      return;
    case LP_ARM:
      if (timer_color_selection.expired( this ))
      {
        push( connectors, ON_NEXTCOLOR, 0, numberOfColorChanged, 0 );
        numberOfColorChanged = numberOfColorChanged + 1;
        if (currentColorSetId == COLORS)
        {
          currentColorSetId = 0;
        }
        else
        {
          currentColorSetId = currentColorSetId + 1;
        }
        setCurrentColorSet(currentColorSetId);
        changeColor(mainColor);
        delay(COLOR_SELECTION_BLINK_TIME);
        turnOff();
        timer_color_selection.setFromNow(this,TIME_FOR_COLOR_SELECTION);
      }
      return;
    case EXT_ARM:
      fadeIn();
      return;
    case ENT_ARMED:
      getCurrentColorSet();
      changeColor(mainColor);
      return;
    case ENT_CLASH:
      changeColor(clashColor);
      delay(CLASH_BLINK_TIME);
      return;
    case ENT_SWING:
      changeColor(swingColor);
      return;
    case ENT_DISARM:
      fadeOut();
      return;
  }
}

void CoreLed::turnOff()
{
  changeColor({0,0,0,0});
}

String CoreLed::decodeColorSetId(int colorSetId)
{
  String colors[9] = {"RED", "ORANGE", "YELLOW", "GREEN", "WHITE", "ACQUA", "BLUE", "PURPLE", "OFF"};

  if (colorSetId < int(sizeof(colors) / sizeof(colors[0])))
  {
    return colors[colorSetId];
  }
  else 
  {
    return OFF;
  }
}

void CoreLed::getCurrentColorSet()
{
  currentColorSetId = moduleSettings->getActiveBank();
  mainColor = moduleSettings->getMainColor(currentColorSetId);
  clashColor = moduleSettings->getClashColor(currentColorSetId);
  swingColor = moduleSettings->getSwingColor(currentColorSetId);

  CoreLogging::writeLine("CoreLed: Color Set: %s", decodeColorSetId(currentColorSetId));
}

void CoreLed::setCurrentColorSet(int colorSetId)
{
  CoreLogging::writeLine("CoreLed: Setting color set...");
  moduleSettings->setActiveBank(colorSetId);
  getCurrentColorSet();
}

void CoreLed::changeColor(const ColorLed& cLed)
{
  analogWrite(PIN_RED, !COMMON_GND ? cLed.red : 255 - cLed.red);
  analogWrite(PIN_GREEN, !COMMON_GND ? cLed.green : 255 - cLed.green);
  analogWrite(PIN_BLUE, !COMMON_GND ? cLed.blue : 255 - cLed.blue);
  analogWrite(PIN_WHITE, !COMMON_GND ? cLed.white : 255 - cLed.white);
}

void CoreLed::fadeIn()
{
  CoreLogging::writeLine("CoreLed: FadeIn");
  for (int i = 0; i <= FADE_DELAY; i++)
  {
    singleStepColor.red = mainColor.red / FADE_DELAY * i;
    singleStepColor.green = mainColor.green / FADE_DELAY * i;
    singleStepColor.blue = mainColor.blue / FADE_DELAY * i;
    singleStepColor.white = mainColor.white / FADE_DELAY * i;
    changeColor(singleStepColor);
    delay(FADE_IN_TIME / FADE_DELAY);
  }
}

void CoreLed::fadeOut()
{
  CoreLogging::writeLine("CoreLed: FadeOut");
  for (int i = FADE_DELAY; i >= 0; i--)
  {
    singleStepColor.red = mainColor.red / FADE_DELAY * i;
    singleStepColor.green = mainColor.green / FADE_DELAY * i;
    singleStepColor.blue = mainColor.blue / FADE_DELAY * i;
    singleStepColor.white = mainColor.white / FADE_DELAY * i;
    changeColor(singleStepColor);
    delay(FADE_OUT_TIME / FADE_DELAY);
  }
}

void CoreLed::pulse(const ColorLed& cLed)
{
  for (int i = 0; i <= PULSE_DELAY; i++)
  {
    singleStepColor.red = cLed.red / PULSE_DELAY * i;
    singleStepColor.green = cLed.green / PULSE_DELAY * i;
    singleStepColor.blue = cLed.blue / PULSE_DELAY * i;
    singleStepColor.white = cLed.white / PULSE_DELAY * i;
    changeColor(singleStepColor);
    delay(PULSE_TIME / 2 / PULSE_DELAY);
  }
  for (int i = FADE_DELAY; i >= 0; i--)
  {
    singleStepColor.red = cLed.red / PULSE_DELAY * i;
    singleStepColor.green = cLed.green / PULSE_DELAY * i;
    singleStepColor.blue = cLed.blue / PULSE_DELAY * i;
    singleStepColor.white = cLed.white / PULSE_DELAY * i;
    changeColor(singleStepColor);
    delay(PULSE_TIME / 2 / PULSE_DELAY);
  }
}

/* Optionally override the default trigger() method
 * Control how your machine processes triggers
 */

CoreLed& CoreLed::trigger( int event ) {
  Machine::trigger( event );
  return *this;
}

/* Optionally override the default state() method
 * Control what the machine returns when another process requests its state
 */

int CoreLed::state( void ) {
  return Machine::state();
}

/* Nothing customizable below this line                          
 ************************************************************************************************
*/

/* Public event methods
 *
 */

CoreLed& CoreLed::idle() {
  trigger( EVT_IDLE );
  return *this;
}

CoreLed& CoreLed::recharge() {
  trigger( EVT_RECHARGE );
  return *this;
}

CoreLed& CoreLed::arm() {
  trigger( EVT_ARM );
  return *this;
}

CoreLed& CoreLed::armed() {
  trigger( EVT_ARMED );
  return *this;
}

CoreLed& CoreLed::swing() {
  trigger( EVT_SWING );
  return *this;
}

CoreLed& CoreLed::clash() {
  trigger( EVT_CLASH );
  return *this;
}

CoreLed& CoreLed::disarm() {
  trigger( EVT_DISARM );
  return *this;
}

void CoreLed::reset_color_timer()
{
  timer_color_selection.setFromNow(this,TIME_FOR_START_COLOR_SELECTION);
}

/*
 * onArm() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreLed& CoreLed::onArm( Machine& machine, int event ) {
  onPush( connectors, ON_ARM, 0, 1, 1, machine, event );
  return *this;
}

CoreLed& CoreLed::onArm( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_ARM, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onArmed() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreLed& CoreLed::onArmed( Machine& machine, int event ) {
  onPush( connectors, ON_ARMED, 0, 1, 1, machine, event );
  return *this;
}

CoreLed& CoreLed::onArmed( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_ARMED, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onClash() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreLed& CoreLed::onClash( Machine& machine, int event ) {
  onPush( connectors, ON_CLASH, 0, 1, 1, machine, event );
  return *this;
}

CoreLed& CoreLed::onClash( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_CLASH, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onDisarm() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreLed& CoreLed::onDisarm( Machine& machine, int event ) {
  onPush( connectors, ON_DISARM, 0, 1, 1, machine, event );
  return *this;
}

CoreLed& CoreLed::onDisarm( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_DISARM, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onNextcolor() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreLed& CoreLed::onNextcolor( Machine& machine, int event ) {
  onPush( connectors, ON_NEXTCOLOR, 0, 1, 1, machine, event );
  return *this;
}

CoreLed& CoreLed::onNextcolor( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_NEXTCOLOR, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onRecharge() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreLed& CoreLed::onRecharge( Machine& machine, int event ) {
  onPush( connectors, ON_RECHARGE, 0, 1, 1, machine, event );
  return *this;
}

CoreLed& CoreLed::onRecharge( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_RECHARGE, 0, 1, 1, callback, idx );
  return *this;
}

/*
 * onSwing() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreLed& CoreLed::onSwing( Machine& machine, int event ) {
  onPush( connectors, ON_SWING, 0, 1, 1, machine, event );
  return *this;
}

CoreLed& CoreLed::onSwing( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_SWING, 0, 1, 1, callback, idx );
  return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */

CoreLed& CoreLed::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "CORELED\0EVT_IDLE\0EVT_RECHARGE\0EVT_ARM\0EVT_ARMED\0EVT_SWING\0EVT_CLASH\0EVT_DISARM\0ELSE\0IDLE\0RECHARGE\0ARM\0ARMED\0CLASH\0SWING\0DISARM" );
  return *this;
}




