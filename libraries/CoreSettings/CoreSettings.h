#pragma once

#include <Arduino.h>
#include <EEPROM.h>

#include "CoreCommon.h"

#define REG_CHECK 0
#define CHECK_VALUE 33
#define REG_COLORSET 1

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
	private:
        saberSettings liveSettings;
};
