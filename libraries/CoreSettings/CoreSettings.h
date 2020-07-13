#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <SerialFlash.h>

#include "ArduinoJson.h"
#include "CoreLogging.h"

static constexpr int REG_CHECK = 0;
static constexpr int CHECK_VALUE = 33;
static constexpr int REG_COLORSET = 1;
static constexpr char configFilename[16] = "config.ini";

class CoreSettings
{
	public:
		//Constructor
		CoreSettings();
		void init();

		void loadDefaults();
		void loadDefaultColors();
        void readFromStore();
		void saveToStore();

		void setActiveBank(int iBank);
		int getActiveBank();

		ColorLed getMainColor(int bank);
		ColorLed getClashColor(int bank);
		void setMainColor(int bank, ColorLed cc);
		void setClashColor(int bank, ColorLed cc);

		int32_t getFileSize(const char *filen);
		void printFile(const char *filen, boolean ignore);

	private:
        saberSettings liveSettings;
	
};
