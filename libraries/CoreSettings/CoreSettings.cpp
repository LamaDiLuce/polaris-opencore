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
{ if(!SerialFlash.ready())
  { CoreLogging::writeLine("ERROR, readFromStore, SeriaFlash Not Ready");
    return;
  }
  //@TODO: read json to settinsg
  //-------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  if(!SerialFlash.exists("/config.ini"))
  { SerialFlash.remove("/config.ini");
  }
  
  if(!SerialFlash.exists(configFilename))
  { Serial.println(String(configFilename)+" does not exist.");
	loadDefaults();
    return;
  }



  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  //-------------------------------------------------------------------------

  //currently active bank is saved in EEPROM
  if (EEPROM.read(REG_CHECK) == CHECK_VALUE)
  { liveSettings.activeBank= EEPROM.read(REG_COLORSET);
  }
  else
  { liveSettings.activeBank=BLUE;
  }

  //for test only
  saveToStore();
}
void CoreSettings::saveToStore()
{ if(!SerialFlash.ready())
  { CoreLogging::writeLine("ERROR, saveToStore, SeriaFlash Not Ready");
    return;
  }
  //@TODO: colours to json to serial file
  //--------- ----------------------------------------------------------------
  //--------- ----------------------------------------------------------------
  //--------- ----------------------------------------------------------------
  if(!SerialFlash.exists(configFilename))
  { if(!SerialFlash.createErasable(configFilename, 65535))
    { CoreLogging::writeLine("ERROR, savetoStore, unable to create " + String(configFilename));
	  return;
	}
  } 

  SerialFlashFile file = SerialFlash.open(configFilename);
  file.erase();
  StaticJsonDocument<8192> doc;
  // Set the values in the document
  doc["version"] = liveSettings.version;
  doc["activeBank"] = liveSettings.activeBank;

  // I know this is wrong, but I wanted proof of concept, Nuntis
  for(int i=0; i<=7; i++)
  { doc["colorSet["+String(i)+"].red"] = liveSettings.colorSet[i].red;
    doc["colorSet["+String(i)+"].green"] = liveSettings.colorSet[i].green;
    doc["colorSet["+String(i)+"].blue"] = liveSettings.colorSet[i].blue;
    doc["colorSet["+String(i)+"].white"] = liveSettings.colorSet[i].white;
  
    doc["clashSet["+String(i)+"].red"] = liveSettings.clashSet[i].red;
    doc["clashSet["+String(i)+"].green"] = liveSettings.clashSet[i].green;
    doc["clashSet["+String(i)+"].blue"] = liveSettings.clashSet[i].blue;
    doc["clashSet["+String(i)+"].white"] = liveSettings.clashSet[i].white;
  }

  // Serialize JSON to file
  char buffer[8192];
  uint32_t sz =serializeJson(doc, buffer);
  if (sz == 0)
  { CoreLogging::writeLine("Error, saveToStore, Failed to write to file");
  }
  file.write(buffer, sz);

  buffer[0]=EOF;
  file.write(buffer,1);

  // Close the file
  file.close();
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  //currently bank is in EEPROM
  EEPROM.write(REG_COLORSET, liveSettings.activeBank);
  EEPROM.write(REG_CHECK, CHECK_VALUE);
}
void CoreSettings::printFile(const char *filen, boolean ignore)
{ if(!SerialFlash.ready())
  { Serial.print("ERROR, printFile, SeriaFlash Not Ready\n");
    return;
  }
  if(!SerialFlash.exists(filen))
  { Serial.print("ERROR, printFile, "+String(filen)+" does not exist\n");
    return;
  }
  char buf[1];
  SerialFlashFile file = SerialFlash.open(filen);
  while(file.available())
  { file.read(buf,1);
    if(ignore && ((buf[0]==(char) 255) || (buf[0]<9)) )
	  break;
	else
      Serial.print(buf[0]);
  }
  file.close();
}
int32_t CoreSettings::getFileSize(const char *filen)
{ int32_t sz =-88;
  
  if(!SerialFlash.ready())
  { CoreLogging::writeLine("ERROR, SeriaFlash Not Ready");
    return -99;
  }
  if(!SerialFlash.exists(filen))
  { CoreLogging::writeLine("ERROR, file " + String(filen) + " does not exist");
    return -1;
  }
  SerialFlashFile file = SerialFlash.open(filen);
  sz =  file.size();
  file.close();
  
  return sz;
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


