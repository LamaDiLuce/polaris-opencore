#include "CoreImu.h"

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

CoreImu& CoreImu::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                             ON_ENTER                ON_LOOP  ON_EXIT  EVT_START_SAMPLING  EVT_HIGH_FREQ_SAMPLING  EVT_DEEP_SLEEP  EVT_TIMER_SAMPLE  ELSE */
    /*               IDLE */             -1,               LP_IDLE,      -1,           SAMPLING,     HIGH_FREQ_SAMPLING,     DEEP_SLEEP,         SAMPLING,   -1,
    /*           SAMPLING */   ENT_SAMPLING,                    -1,      -1,                 -1,     HIGH_FREQ_SAMPLING,     DEEP_SLEEP,               -1, IDLE,
    /* HIGH_FREQ_SAMPLING */             -1, LP_HIGH_FREQ_SAMPLING,      -1,           SAMPLING,                     -1,     DEEP_SLEEP,               -1,   -1,
    /*         DEEP_SLEEP */ ENT_DEEP_SLEEP,             ATM_SLEEP,      -1,           SAMPLING,                     -1,     DEEP_SLEEP,               -1,   -1,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );
  timer.set( SAMPLE_RATE );
  pinMode(SDO_PIN, OUTPUT);
  digitalWrite(SDO_PIN, HIGH);
  Wire.setSDA(18);
  Wire.setSCL(19);
  if (imu.begin() == 0)
  {
    dataToWrite = 0;
    dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;

    imu.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);
    imu.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
    dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);

    imu.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x8E);
    imu.writeRegister(LSM6DS3_ACC_GYRO_TAP_THS_6D, CLASH_TRESHOLD);
    imu.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x7F);
    imu.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80);
    imu.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x48);
  }
  else
  {
    delay(5000);
    CoreLogging::writeLine("ERROR, IMU cannot be initialized");
  }
 
  pinMode(IMU_INT1_PIN, INPUT);
  return *this;          
}

/* Add C++ code for each internally handled event (input) 
 * The code must return 1 to trigger the event
 */

int CoreImu::event( int id ) {
  switch ( id ) {
    case EVT_START_SAMPLING:
      return 0;
    case EVT_HIGH_FREQ_SAMPLING:
      return 0;
    case EVT_DEEP_SLEEP:
      return 0;
    case EVT_TIMER_SAMPLE:
      return timer.expired( this );
  }
  return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push( connectors, ON_SAMPLE, 0, <v>, <up> );
 */

void CoreImu::action( int id ) {
  switch ( id ) {
    case LP_IDLE:
      return;
    case ENT_SAMPLING:
      sample();
      push( connectors, ON_SAMPLE, 0, 0, 0 );
      return;
    case LP_HIGH_FREQ_SAMPLING:
      sample();
      push( connectors, ON_SAMPLE, 0, 0, 0 );
      return;
    case ENT_DEEP_SLEEP:
      return;
    case LP_DEEP_SLEEP:
      return;
  }
}

void CoreImu::sample() {
  // Exponential moving average
  GyroX  = (SMOOTHING_FACTOR * imu.readFloatGyroX())  + (1.0 - SMOOTHING_FACTOR) * GyroX;
  GyroY  = (SMOOTHING_FACTOR * imu.readFloatGyroY())  + (1.0 - SMOOTHING_FACTOR) * GyroY;
  GyroZ  = (SMOOTHING_FACTOR * imu.readFloatGyroZ())  + (1.0 - SMOOTHING_FACTOR) * GyroZ;
  AccelX = (SMOOTHING_FACTOR * imu.readFloatAccelX()) + (1.0 - SMOOTHING_FACTOR) * AccelX;
  AccelY = (SMOOTHING_FACTOR * imu.readFloatAccelY()) + (1.0 - SMOOTHING_FACTOR) * AccelY;
  AccelZ = (SMOOTHING_FACTOR * imu.readFloatAccelZ()) + (1.0 - SMOOTHING_FACTOR) * AccelZ;
  //CoreLogging::writeLine("%f %f %f %f %f %f", GyroX, GyroY, GyroZ, AccelX, AccelY, AccelZ);
}

float CoreImu::getGyroX() {
  return GyroX;
}

float CoreImu::getGyroY() {
  return GyroY;
}

float CoreImu::getGyroZ() {
  return GyroZ;
}

float CoreImu::getAccelX() {
  return AccelX;
}

float CoreImu::getAccelY() {
  return AccelY;
}

float CoreImu::getAccelZ() {
  return AccelZ;
}

float CoreImu::getSwingSpeed() {
  return sqrtf(GyroX * GyroX + GyroY * GyroY);
}

float CoreImu::getRollSpeed() {
  return abs(GyroZ);
}


int CoreImu::getInt1Pin() {
  return IMU_INT1_PIN;
}

/* Optionally override the default trigger() method
 * Control how your machine processes triggers
 */

CoreImu& CoreImu::trigger( int event ) {
  Machine::trigger( event );
  return *this;
}

/* Optionally override the default state() method
 * Control what the machine returns when another process requests its state
 */

int CoreImu::state( void ) {
  return Machine::state();
}

/* Nothing customizable below this line                          
 ************************************************************************************************
*/

/* Public event methods
 *
 */

CoreImu& CoreImu::start_sampling() {
  trigger( EVT_START_SAMPLING );
  return *this;
}

CoreImu& CoreImu::high_freq_sampling() {
  trigger( EVT_HIGH_FREQ_SAMPLING );
  return *this;
}

CoreImu& CoreImu::deep_sleep() {
  trigger( EVT_DEEP_SLEEP );
  return *this;
}

/*
 * onSample() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

CoreImu& CoreImu::onSample( Machine& machine, int event ) {
  onPush( connectors, ON_SAMPLE, 0, 1, 1, machine, event );
  return *this;
}

CoreImu& CoreImu::onSample( atm_cb_push_t callback, int idx) {
  onPush( connectors, ON_SAMPLE, 0, 1, 1, callback, idx );
  return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */

CoreImu& CoreImu::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "COREIMU\0EVT_START_SAMPLING\0EVT_HIGH_FREQ_SAMPLING\0EVT_DEEP_SLEEP\0EVT_TIMER_SAMPLE\0ELSE\0IDLE\0SAMPLING\0HIGH_FREQ_SAMPLING\0DEEP_SLEEP" );
  return *this;
}

