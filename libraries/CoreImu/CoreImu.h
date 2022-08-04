#pragma once

#include <Automaton.h>
#include <Wire.h>
#include "LSM6DS3.h"
#include "CoreLogging.h"

#define IMU_INT1_PIN 0
#define SDO_PIN 17

#define SAMPLE_RATE 20

class CoreImu: public Machine {

 public:
  enum { IDLE, SAMPLING, HIGH_FREQ_SAMPLING, DEEP_SLEEP }; // STATES
  enum { EVT_START_SAMPLING, EVT_HIGH_FREQ_SAMPLING, EVT_DEEP_SLEEP, EVT_TIMER_SAMPLE, ELSE }; // EVENTS
  CoreImu( void ) : Machine() {};
  CoreImu& begin( void );
  CoreImu& trace( Stream & stream );
  CoreImu& trigger( int event );
  int state( void );
  CoreImu& onSample( Machine& machine, int event = 0 );
  CoreImu& onSample( atm_cb_push_t callback, int idx = 0 );
  CoreImu& start_sampling( void );
  CoreImu& high_freq_sampling( void );
  CoreImu& deep_sleep( void );
  float getGyroX();
  float getGyroY();
  float getGyroZ();
  float getAccelX();
  float getAccelY();
  float getAccelZ();
  float getSwingSpeed();
  float getRollSpeed();
  float getAngDotProduct();
  int getInt1Pin();


 private:
  enum { LP_IDLE, ENT_SAMPLING, LP_HIGH_FREQ_SAMPLING, ENT_DEEP_SLEEP, LP_DEEP_SLEEP }; // ACTIONS
  enum { ON_SAMPLE, CONN_MAX }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  int event( int id ); 
  void action( int id ); 
  void sample();
  uint8_t dataToWrite = 0;
  atm_timer_millis timer;
  float AccelX;
  float AccelY;
  float AccelZ;
  float GyroX = 0;
  float GyroY = 0;
  float GyroZ = 0;
  float AngAccX = 0;
  float AngAccY = 0;
  float AngAccZ = 0;
  float PrevGyroX = 0;
  float PrevGyroY = 0;
  float PrevGyroZ = 0;
  float PrevAngAccX = 0;
  float PrevAngAccY = 0;
  float PrevAngAccZ = 0;
  float AngDotProduct = 0;
  static constexpr float SMOOTHING_FACTOR = 0.4;  // This is the alpha factor in the exponential moving average
  static constexpr int CLASH_TRESHOLD = 0x0B; // min 0x00 (0) max 0x1F (31) 5 bits, middle 0x0F (15)
};

/* 
Automaton::ATML::begin - Automaton Markup Language

<?xml version="1.0" encoding="UTF-8"?>
<machines>
  <machine name="CoreImu">
    <states>
      <IDLE index="0" on_loop="LP_IDLE">
        <EVT_START_SAMPLING>SAMPLING</EVT_START_SAMPLING>
        <EVT_HIGH_FREQ_SAMPLING>HIGH_FREQ_SAMPLING</EVT_HIGH_FREQ_SAMPLING>
        <EVT_DEEP_SLEEP>DEEP_SLEEP</EVT_DEEP_SLEEP>
        <EVT_TIMER_SAMPLE>SAMPLING</EVT_TIMER_SAMPLE>
      </IDLE>
      <SAMPLING index="1" on_enter="ENT_SAMPLING">
        <EVT_HIGH_FREQ_SAMPLING>HIGH_FREQ_SAMPLING</EVT_HIGH_FREQ_SAMPLING>
        <EVT_DEEP_SLEEP>DEEP_SLEEP</EVT_DEEP_SLEEP>
        <ELSE>IDLE</ELSE>
      </SAMPLING>
      <HIGH_FREQ_SAMPLING index="2" on_loop="LP_HIGH_FREQ_SAMPLING">
        <EVT_START_SAMPLING>SAMPLING</EVT_START_SAMPLING>
        <EVT_DEEP_SLEEP>DEEP_SLEEP</EVT_DEEP_SLEEP>
      </HIGH_FREQ_SAMPLING>
      <DEEP_SLEEP index="3" sleep="1" on_enter="ENT_DEEP_SLEEP" on_loop="LP_DEEP_SLEEP">
        <EVT_START_SAMPLING>SAMPLING</EVT_START_SAMPLING>
        <EVT_DEEP_SLEEP>DEEP_SLEEP</EVT_DEEP_SLEEP>
      </DEEP_SLEEP>
    </states>
    <events>
      <EVT_START_SAMPLING index="0" access="MIXED"/>
      <EVT_HIGH_FREQ_SAMPLING index="1" access="MIXED"/>
      <EVT_DEEP_SLEEP index="2" access="MIXED"/>
      <EVT_TIMER_SAMPLE index="3" access="PRIVATE"/>
    </events>
    <connectors>
      <SAMPLE autostore="0" broadcast="0" dir="PUSH" slots="1"/>
    </connectors>
    <methods>
    </methods>
  </machine>
</machines>

Automaton::ATML::end 
*/
