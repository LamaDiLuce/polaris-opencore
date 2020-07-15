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
{ liveSettings.version = CURRENTVERSION ;

  liveSettings.colorSet[RED]    = {255, 0, 0, 0};      //0 RED
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
  
  if(!SerialFlash.exists(configFilename))
  { Serial.println(String(configFilename)+" does not exist.");
	  loadDefaults();
    return;
  }

  SerialFlashFile file = SerialFlash.open(configFilename);
  StaticJsonDocument<16384> doc;
  char buffer[16384];

  file.read(buffer, 16384);
  file.close();

  DeserializationError error = deserializeJson(doc, buffer);
  if (error)
  { CoreLogging::writeLine("Failed to read file, using default configuration");
    loadDefaults();
    return;
  } else
  { JsonObject obj = doc.as<JsonObject>();

    liveSettings.version = obj["version"].as<String>();
    if(!liveSettings.version.equals(CURRENTVERSION))
    { //wrong format filetype so load defaults
      loadDefaults();
      return;
    }
    
    liveSettings.activeBank = doc["activeBank"] | BLUE;
    
    for(int i=0; i<=7; i++)
    { liveSettings.colorSet[i].red   = doc["bank"][i]["color"]["red"] | 0;
      liveSettings.colorSet[i].green = doc["bank"][i]["color"]["green"] | 0;
      liveSettings.colorSet[i].blue  = doc["bank"][i]["color"]["blue"] | 0;
      liveSettings.colorSet[i].white = doc["bank"][i]["color"]["white"] | 0;
   
      liveSettings.clashSet[i].red   = doc["bank"][i]["clash"]["red"] | 0;
      liveSettings.clashSet[i].green = doc["bank"][i]["clash"]["green"] | 0;
      liveSettings.clashSet[i].blue  = doc["bank"][i]["clash"]["blue"] | 0;
      liveSettings.clashSet[i].white = doc["bank"][i]["clash"]["white"] | 0;
    }
  }
  
}
void CoreSettings::saveToStore()
{ if(!SerialFlash.ready())
  { CoreLogging::writeLine("ERROR, saveToStore, SeriaFlash Not Ready");
    return;
  }
  
  if(!SerialFlash.exists(configFilename))
  { if(!SerialFlash.createErasable(configFilename, 65535))
    { CoreLogging::writeLine("ERROR, savetoStore, unable to create " + String(configFilename));
	    return;
	  }
  } 

  SerialFlashFile file = SerialFlash.open(configFilename);
  file.erase();
  StaticJsonDocument<16384> doc;
  // Set the values in the document
  doc["version"] = liveSettings.version;
  doc["activeBank"] = liveSettings.activeBank;

  for(int i=0; i<=7; i++)
  { doc["bank"][i]["color"]["red"] = liveSettings.colorSet[i].red;
    doc["bank"][i]["color"]["green"] = liveSettings.colorSet[i].green;
    doc["bank"][i]["color"]["blue"] = liveSettings.colorSet[i].blue;
    doc["bank"][i]["color"]["white"] = liveSettings.colorSet[i].white;
  
    doc["bank"][i]["clash"]["red"] = liveSettings.clashSet[i].red;
    doc["bank"][i]["clash"]["green"] = liveSettings.clashSet[i].green;
    doc["bank"][i]["clash"]["blue"] = liveSettings.clashSet[i].blue;
    doc["bank"][i]["clash"]["white"] = liveSettings.clashSet[i].white;
  }

  // Serialize JSON to file
  char buffer[16384];
  uint32_t sz = serializeJsonPretty(doc, buffer);//<-- writes json in a more human readable format
  //uint32_t sz = serializeJson(doc, buffer);    //<-- writes a smaller file, but the block size is 64K anyway
  if (sz == 0)
  { CoreLogging::writeLine("Error, saveToStore, Failed to write to file");
  }
  file.write(buffer, sz);

  // Close the file
  file.close();
  
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
