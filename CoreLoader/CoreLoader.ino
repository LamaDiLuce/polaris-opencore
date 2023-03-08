#include "SparkFunLSM6DS3.h"
#include <Audio.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine sine1;          // xy=178,354
AudioPlaySerialflashRaw playFlashRaw1; // xy=432,164
AudioMixer4 mixer1;                    // xy=446,345
AudioOutputAnalog dac1;                // xy=630,334
AudioConnection patchCord1(sine1, 0, mixer1, 1);
AudioConnection patchCord2(playFlashRaw1, 0, mixer1, 0);
AudioConnection patchCord3(mixer1, dac1);
// GUItool: end automatically generated code

#define BUILD "4.1.0"

#define TEST_RAW false
#define DEBUG false

// Flash
#define CS_PIN 10
#define SI_PIN 11
#define SO_PIN 12
#define SCK_PIN 14

// Communications
#define SOH 0x01 // Start Trasmission
#define STX 0x02 // Start new file
#define TAB 0x09
#define DC1 0x11 // Start file name
#define DC2 0x12 // End filename and start file size
#define ETX 0x03 // End received file
#define EOT 0x04 // End trasmission all files
#define ENQ 0x05 // Erase flash
#define ACK 0x06
#define NAK 0x15
#define BUFFER_FILE 100000 // 100KB max
#define BUFFER 512
#define DATA_SERIAL_BAUD 2000000
#define DEBUG_SERIAL_BAUD 9600
#define COLUMN 60

// Diagnostics
#define PIN_RED 3
#define PIN_GREEN 4
#define PIN_BLUE 5
#define PIN_WHITE 6

// NeoPixel strip
#define ENABLE_NEOPIXEL // comment to disable

// Init serial connections
#define SerialData Serial
#define SerialDebug Serial1

// IMU
#define IMU_INT1_PIN 2
#define IMU_TAP_TIMES_TEST 3
#define IMU_TAP_ELAPSED_TIME 10

enum Status
{
  wait_start = 0,
  receive_file_name = 1,
  receive_file_size = 2,
  start_writing = 3,
  end_writing = 4,
  end_transmission = 5,
  error = 6,
  finished = 7
};

enum Errors
{
  cannot_open_file = 0,
  cannot_create_file = 1,
  file_size_too_long = 2,
  file_exceed_limit_size = 3,
  cannot_connect = 4,
  nothing = 5
};

enum SerialStatusFlow
{
  started = 0,
  wait = 2,
  clearing = 3,
  demo = 4
};

Status status_process = Status::wait_start;
Errors status_error = Errors::nothing;
SerialStatusFlow status_flow = SerialStatusFlow::wait;
unsigned long currentMillis = millis();

unsigned char byte_count = 0;
byte incoming_byte = 0;
String incoming_message;

byte column_index = 0;
String file_name;
uint32_t file_size;
byte file_size_bytes[4];
int index_bytes;
String data_file;
int first_data;

SerialFlashFile file;

union ArrayToInteger {
  byte array[4];
  uint32_t integer;
};

// IMU
LSM6DS3Core imu_core(I2C_MODE, 0x6B);
LSM6DS3 imu_device;
uint8_t int1Status = 0;
uint8_t errorAccumulator = 0;
uint8_t dataToWrite = 0;

// Diagnostic variables
byte diagnostic_step = 0;

/* COLOR LEGENDS
   ------ERRORS-------
   BLUE, BLUE Cannot connect to flash
   RED, RED Cannot open
   GREEN,RED Cannot created
   BLUE,RED File size information too long
   WHITE,RED File has exceeded the maximum limit
   ------MESSAGE-------
   GREEN All file written
   BLUE Erasing flash
*/

void setErrorStatus(Errors error)
{

  status_process = Status::error;
  status_error = error;

  switch (error)
  {
  case Errors::cannot_open_file: {
    notifyEvent(PIN_RED, PIN_RED);
  }
  break;
  case Errors::cannot_create_file: {
    notifyEvent(PIN_GREEN, PIN_RED);
  }
  break;
  case Errors::file_size_too_long: {
    notifyEvent(PIN_BLUE, PIN_RED);
  }
  break;
  case Errors::file_exceed_limit_size: {
    notifyEvent(PIN_WHITE, PIN_RED);
  }
  break;
  case Errors::cannot_connect: {
    notifyEvent(PIN_BLUE, PIN_BLUE);
  }
  break;
  case Errors::nothing: {
    resetColor();
  }
  break;
  }

  SerialData.write(SOH);
  SerialData.write(NAK);
  SerialData.write(status_error);
  SerialData.write(EOT);
  delay(100);
}

// >> IMU LOGIC

void initIMU()
{

  SerialData.print("Init IMU system: ");

  imu_device.begin();

  if (imu_core.beginCore() != 0)
  {
    SerialData.println("Error at beginCore().\n");
  }
  else
  {
    // Setup the accelerometer
    dataToWrite = 0;
    dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;

    // Now, write the patched together data
    errorAccumulator += imu_core.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);

    // Set the ODR bit
    errorAccumulator += imu_core.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
    dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);

    // Enable tap detection on X, Y, Z axis, but do not latch output
    errorAccumulator += imu_core.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x0E);

    // Set tap threshold
    // Write 0Ch into TAP_THS_6D
    errorAccumulator += imu_core.writeRegister(LSM6DS3_ACC_GYRO_TAP_THS_6D, 0x03);

    // Set Duration, Quiet and Shock time windows
    // Write 7Fh into INT_DUR2
    errorAccumulator += imu_core.writeRegister(LSM6DS3_ACC_GYRO_INT_DUR2, 0x7F);

    // Single & Double tap enabled (SINGLE_DOUBLE_TAP = 1)
    // Write 80h into WAKE_UP_THS
    errorAccumulator += imu_core.writeRegister(LSM6DS3_ACC_GYRO_WAKE_UP_THS, 0x80);

    // Single tap interrupt driven to INT1 pin -- enable latch
    // Write 08h into MD1_CFG
    errorAccumulator += imu_core.writeRegister(LSM6DS3_ACC_GYRO_MD1_CFG, 0x48);

    if (errorAccumulator)
    {
      SerialData.println("Problem configuring the device.");
    }
    else
    {
      SerialData.println("OK");
    }
  }

  // Configure the atmega interrupt pin
  pinMode(IMU_INT1_PIN, INPUT);
  attachInterrupt(0, int1ISR, RISING);
}

void int1ISR()
{
  int1Status++;
}

// << IMU LOGIC

void setup()
{

  SPI.setCS(CS_PIN);
  SPI.setMOSI(SI_PIN);
  SPI.setMISO(SO_PIN);
  SPI.setSCK(SCK_PIN);

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_WHITE, OUTPUT);

  resetColor();

  SerialData.begin(DATA_SERIAL_BAUD); // Trasmission data

  if (DEBUG)
  {
    SerialDebug.begin(DEBUG_SERIAL_BAUD); // Monitor
    while (!SerialDebug)
      ;
  }

  message("Create file from serial (", false);
  message(BUILD, false);
  message(")", true);

  if (!SerialFlash.begin(CS_PIN))
  {
    setErrorStatus(Errors::cannot_connect);
  }
  else
  {
    message("Flash connected.", true);
  }

  AudioMemory(16);
  mixer1.gain(0, 0);
  mixer1.gain(1, 0);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0);

  initIMU();

  // Test audio
  if (TEST_RAW)
  {
    status_flow = SerialStatusFlow::demo;
  }
  else
  {
    message("Waiting incoming message...", true);
  }
}

void message(String message, bool ln)
{
  if (DEBUG)
  {
    if (ln)
    {
      SerialDebug.println(message);
    }
    else
    {
      SerialDebug.print(message);
    }
  }
}

void playFile(const char* filename)
{
  // mixer1.gain(0,1);
  //  SerialDebug.print("Playing file: ");
  //  SerialDebug.println(filename);

  playFlashRaw1.play(filename);

  delay(5);

  while (playFlashRaw1.isPlaying())
    ;
  // mixer1.gain(0,0);
}

void eraseFlash()
{
  message("Erasing flash memory, wait... ", false);
  uint8_t id[5];
  SerialFlash.readID(id);
  SerialFlash.eraseAll();

  while (!SerialFlash.ready())
  {
    delay(150);
    digitalWrite(PIN_BLUE, LOW);
    delay(150);
    digitalWrite(PIN_BLUE, HIGH);
  }
  digitalWrite(PIN_BLUE, HIGH);

  // Send OK to PC
  SerialData.write(SOH);
  SerialData.write(ACK);
  SerialData.write(EOT);
  SerialData.flush();

  message("OK", true);
}

void convertUInt32ToBytes(uint32_t value, byte* bytes)
{
  bytes[0] = static_cast<unsigned char>(value & 0x000000FF);
  bytes[1] = static_cast<unsigned char>((value & 0x0000FF00) >> 8);
  bytes[2] = static_cast<unsigned char>((value & 0x00FF0000) >> 16);
  bytes[3] = static_cast<unsigned char>((value & 0xFF000000) >> 24);
}

uint32_t convertBytesToUInt32(byte* bytes)
{
  ArrayToInteger converter; // Create a converter

  converter.array[0] = bytes[0]; // save something to each byte in the array
  converter.array[1] = bytes[1]; // save something to each byte in the array
  converter.array[2] = bytes[2]; // save something to each byte in the array
  converter.array[3] = bytes[3]; // save something to each byte in the array

  return converter.integer;
}

void resetColor()
{
  analogWrite(PIN_RED, 0);
  digitalWrite(PIN_GREEN, LOW);
  digitalWrite(PIN_BLUE, LOW);
  digitalWrite(PIN_WHITE, LOW);
}

void notifyEvent(int pin1, int pin2)
{
  if (pin1 == PIN_RED)
  {
    analogWrite(pin1, 0);
    delay(500);
    analogWrite(pin1, 55);
    delay(500);
  }
  else
  {
    digitalWrite(pin1, LOW);
    delay(500);
    digitalWrite(pin1, HIGH);
    delay(500);
  }
  if (pin2 == PIN_RED)
  {
    analogWrite(pin2, 0);
    delay(500);
    analogWrite(pin2, 55);
    delay(500);
  }
  else
  {
    digitalWrite(pin2, LOW);
    delay(500);
    digitalWrite(pin2, HIGH);
    delay(500);
  }
}

Status receiving_status = Status::wait_start;

void createFile(String pFileName, uint32_t pFileSize)
{

  // byte data[pFileSize];
  char fileName[pFileName.length() + 1];

  pFileName.toCharArray(fileName, pFileName.length() + 1);
  // pData.getBytes(data, pFileSize);

  message("Creating file: ", false);
  message(fileName, true);

  // Remove if it already exists
  SerialFlash.opendir();

  //  if (SerialFlash.exists(fileName)) {
  //    message(pFileName, false);
  //    message(" already exists: ", false);
  //    SerialFlash.remove(fileName);
  //    message("deleted", true);
  //  }

  // Create new file
  if (SerialFlash.create(fileName, pFileSize))
  {

    // Opening file to write it
    file = SerialFlash.open(fileName);
    if (file)
    {
      message("Created file.", true);
    }
    else
    {
      SerialData.write(SOH);
      SerialData.write(NAK);
      SerialData.write(Errors::cannot_open_file);
      SerialData.write(EOT);
      delay(100);
      status_process = Status::error;
    }
  }
  else
  {
    SerialData.write(SOH);
    SerialData.write(NAK);
    SerialData.write(Errors::cannot_create_file);
    SerialData.write(EOT);
    delay(100);
    status_process = Status::error;
  }
}

bool need_arm = true;
bool armed = false;
double average_gryo = 0;
String fileRaw;
char* fileArrayRaw;
long randomValue;

void loop()
{
  if (status_flow == SerialStatusFlow::demo)
  {

    if (need_arm)
    {
      playFlashRaw1.play("POWERON_0.RAW");
      delay(5);
      while (playFlashRaw1.isPlaying())
        ;
      need_arm = false;
      currentMillis = millis();
      randomSeed(currentMillis);
    }

    if (armed)
    {
      if (!playFlashRaw1.isPlaying())
      {
        playFlashRaw1.play("HUM_0.RAW");
      }

      // CLASH
      if (int1Status > 0)
      {
        delay(300);
        if (int1Status == 1)
        {
          fileRaw = "CLASH_";
          fileRaw += String(random(1, 10));
          fileRaw += "_0.RAW";
          fileRaw.toCharArray(fileArrayRaw, fileRaw.length());

          playFlashRaw1.stop();
          playFlashRaw1.play(fileArrayRaw);
        }
        if (int1Status > 1)
        {
          playFlashRaw1.stop();
          playFlashRaw1.play("CLASH_9_0.RAW");
        }
        currentMillis = millis();
        int1Status = 0;
      }

      // SWING
      average_gryo = (imu_device.readFloatGyroX() + imu_device.readFloatGyroY() + imu_device.readFloatGyroZ()) / 3.0;

      if (average_gryo > 1000)
      {
        playFlashRaw1.stop();
        fileRaw = "SWING_";
        fileRaw += String(random(1, 8));
        fileRaw += "_0.RAW";
        fileRaw.toCharArray(fileArrayRaw, fileRaw.length());
        currentMillis = millis();
      }
    }

    if (millis() - currentMillis > 3000)
    {
      playFlashRaw1.stop();
      playFlashRaw1.play("POWEROFF_0.RAW");
    }
  }
  else
  {
    if (status_error == Errors::nothing)
    {
      byte_count = 0;

      while (SerialData.available() && byte_count < BUFFER)
      {
        currentMillis = millis();
        incoming_byte = SerialData.read();

        // SerialDebug2.print((char)incoming_byte);
        // SerialDebug2.print("|");

        if ((incoming_byte == ENQ) && (status_flow == SerialStatusFlow::wait))
        {
          eraseFlash();
        }
        else if ((incoming_byte == SOH) && (status_flow == SerialStatusFlow::wait))
        {
          message("Start message", true);
          // incoming_message = "";
          status_flow = SerialStatusFlow::started;
        }
        else
        {
          int currentValue = incoming_byte;
          // Selection status
          if (receiving_status < Status::start_writing)
          {
            switch (currentValue)
            {
            case STX: {
              message("Start new file", true);
            }
            break;
            case DC1: {
              message("Start receiving file name", true);
              file_name = "";
              receiving_status = Status::receive_file_name;
            }
            break;
            case DC2: {
              message("File name received: ", false);
              message(file_name, true);
              message("Start receiving file size", true);
              file_size = 0;
              index_bytes = 0;
              file_size_bytes[0] = 0;
              file_size_bytes[1] = 0;
              file_size_bytes[2] = 0;
              file_size_bytes[3] = 0;
              receiving_status = Status::receive_file_size;
            }
            break;
            }
          }

          // Aggregation
          switch (receiving_status)
          {
          case Status::receive_file_name: {
            if (currentValue != DC1)
            {
              file_name += (char)currentValue;
            }
          }
          break;
          case Status::receive_file_size: {
            if (currentValue != DC2)
            {
              file_size_bytes[index_bytes] = currentValue;
              //                message(index_bytes, false);
              //                message(") ", false);
              //                message(file_size_bytes[index_bytes], true);
              index_bytes++;

              if (index_bytes == 4)
              {
                // Calculate size file from byte to bit
                file_size = convertBytesToUInt32(file_size_bytes);
                message("File size received: ", false);
                message(String(file_size), true);
                message("Start receiving data file", true);
                receiving_status = Status::start_writing;
                createFile(file_name, file_size);
              }
            }
          }
          break;
          case Status::start_writing: {
            // data_file += currentValue;
            byte data[] = {currentValue};

            // Write buffer
            file.write(data, 1);

            //              if (first_data <= 14) {
            //                SerialDebug2.print(data[0], DEC);
            //                SerialDebug2.print("|");
            //                SerialDebug2.println(data[0], HEX);
            //              }
            //              first_data++;
          }
          }
        }

        byte_count++;
      }

      if ((status_flow == SerialStatusFlow::started) && (millis() - currentMillis > 5000))
      {

        message("End message", true);
        status_flow = SerialStatusFlow::wait;
        listFiles();
        status_flow = SerialStatusFlow::demo;
      }
      else if ((receiving_status == Status::start_writing) && (millis() - currentMillis > 2000))
      {

        receiving_status = Status::wait_start;
        file.close();
        message("Close file", true);

        digitalWrite(PIN_GREEN, LOW);
        delay(250);
        digitalWrite(PIN_GREEN, HIGH);

        SerialData.write(SOH);
        SerialData.write(ACK);
        SerialData.write(EOT);
        delay(100);
      }
    }
    else
    {
      // Repeat forever current error
      setErrorStatus(status_error);
      delay(2000);
    }
  }
}

void process_message(String value)
{
  long message_size = value.length();

  message("Received message (", false);
  message(message_size, false);
  message("): ", false);
  // message(value, true);

  if (message_size == 1)
  {

    int real_value = value.toInt();

    switch (real_value)
    {
    case ENQ: {
      eraseFlash();
    }
    break;
    }
  }
  else
  {
    Status receiving_status = Status::wait_start;
    for (int l = 0; l < message_size; l++)
    {

      int currentValue = String(value[1]).toInt();
    }
  }
}

void listFiles()
{

  mixer1.gain(0, 0.2);

  SerialFlash.opendir();

  while (1)
  {
    // List all files to PC
    char filename[64];
    uint32_t filesize;

    if (SerialFlash.readdir(filename, sizeof(filename), filesize))
    {
      SerialData.write(SOH);
      SerialData.write(STX);
      SerialData.print(filename);
      SerialData.print(" ");
      spaces(20 - strlen(filename));
      SerialData.print(" (");
      SerialData.print(filesize);
      SerialData.print(")\n");
      SerialData.write(EOT);
      playFile(filename);
    }
    else
    {
      break; // no more files
    }
  }
}

void spaces(int num)
{
  for (int i = 0; i < num; i++)
  {
    SerialData.print(" ");
  }
}
