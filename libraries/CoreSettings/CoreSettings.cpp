#include "CoreSettings.h"

CoreSettings::CoreSettings()
{
}
void CoreSettings::init()
{ loadDefaults();
  readFromStore(); 
}
void CoreSettings::loadDefaults()
{ liveSettings.activeBank=BLUE;
  loadDefaultColors();
}
void CoreSettings::loadDefaultColors()
{ liveSettings.colorSet[RED]    = {255, 0, 0, 0};      //0 RED
  liveSettings.colorSet[GREEN]  = {0, 255, 0, 0};      //1 GREEN
  liveSettings.colorSet[BLUE]   = {0, 0, 255, 0};      //2 BLUE
  liveSettings.colorSet[YELLOW] = {100, 255, 0, 60};   //3 YELLOW
  liveSettings.colorSet[ACQUA]  = {0, 255, 240, 80};   //4 ACQUA
  liveSettings.colorSet[PURPLE] = {35, 10, 255, 10};   //5 PURPLE
  liveSettings.colorSet[ORANGE] = {150, 255, 0, 20};   //6 ORANGE
  liveSettings.colorSet[WHITE]  = {25, 170, 150, 255}; //7 WHITE
  liveSettings.colorSet[OFF]    = {0, 0, 0, 0};        //8 OFF

  liveSettings.clashSet[RED]    = {25, 170, 150, 255}; 
  liveSettings.clashSet[GREEN]  = {25, 170, 150, 255}; 
  liveSettings.clashSet[BLUE]   = {25, 170, 150, 255}; 
  liveSettings.clashSet[YELLOW] = {25, 170, 150, 255}; 
  liveSettings.clashSet[ACQUA]  = {25, 170, 150, 255}; 
  liveSettings.clashSet[PURPLE] = {25, 170, 150, 255}; 
  liveSettings.clashSet[ORANGE] = {25, 170, 150, 255}; 
  liveSettings.clashSet[WHITE]  = {0, 255, 240, 80};   //WHITE flashes ACQUA 
  liveSettings.clashSet[OFF]    = {0, 0, 0, 0};
}
void CoreSettings::readFromStore()
{ //@TODO: read json to settinsg
  //else read from EEPROM

  //currently active bank is saved in EEPROM
  if (EEPROM.read(REG_CHECK) == CHECK_VALUE)
  { liveSettings.activeBank= EEPROM.read(REG_COLORSET);
  }
  else
  { liveSettings.activeBank=BLUE;
  }
}
void CoreSettings::saveToStore()
{ //@TODO: colours to json to serial file
  //if not possible use EEPROM


  //currently bank is in EEPROM
  EEPROM.write(REG_COLORSET, liveSettings.activeBank);
  EEPROM.write(REG_CHECK, CHECK_VALUE);
}

void CoreSettings::setActiveBank(int iBank)
{ liveSettings.activeBank=iBank;
  saveToStore();
}
int CoreSettings::getActiveBank()
{ return liveSettings.activeBank;
}

ColorLed CoreSettings::getMainColor(int bank)
{ return liveSettings.colorSet[bank];
}
ColorLed CoreSettings::getClashColor(int bank)
{ return liveSettings.clashSet[bank];
}
void CoreSettings::setMainColor(int bank, ColorLed cc)
{ liveSettings.colorSet[bank]=cc;
}
void CoreSettings::setClashColor(int bank, ColorLed cc)
{ liveSettings.clashSet[bank]=cc;
}
