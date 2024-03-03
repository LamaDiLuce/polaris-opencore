#include "CoreSettings.h"

static constexpr const char* configFilename = "config.ini";

CoreSettings::CoreSettings()
{
}

void CoreSettings::init()
{
  pinMode(HW_VERSION_1, INPUT_PULLUP);   // Pin 40
  pinMode(HW_VERSION_2, INPUT_PULLUP);   // Pin 41
  pinMode(HW_VERSION_3, INPUT_PULLUP);   // Pin 42

  int hwVersion1 = digitalRead(HW_VERSION_1);
  int hwVersion2 = digitalRead(HW_VERSION_2);
  int hwVersion3 = digitalRead(HW_VERSION_3);

  liveSettings.hwVersion = 8 - hwVersion1 * 1 - hwVersion2 * 2 - hwVersion3 * 4;
    
  loadDefaults();
  readFromStore();
}

void CoreSettings::loadDefaults()
{
  liveSettings.activeBank = ACQUA;
  loadDefaultColors();
  loadDefaultSounds();
}

void CoreSettings::loadDefaultSounds()
{
  liveSettings.soundEngine=0;

  liveSettings.on.count=1;
  liveSettings.on.sounds[0] ="POWERON_0.RAW";

  liveSettings.off.count=1;
  liveSettings.off.sounds[0] ="POWEROFF_0.RAW";

  liveSettings.hum.count=1;
  liveSettings.hum.sounds[0] ="HUM_0.RAW";

  liveSettings.clash.count = 10;
  for(int i=0; i<liveSettings.clash.count; i++)
  {
    liveSettings.clash.sounds[i] = "CLASH_"+String(i+1)+"_0.RAW";
  }

  liveSettings.swing.count = 0;

  liveSettings.smoothSwingA.count=8;
  for(int i=0; i<liveSettings.smoothSwingA.count; i++)
  {
    liveSettings.smoothSwingA.sounds[i] = "SMOOTHSWINGH_"+String(i+1)+"_0.RAW";
  }

  liveSettings.smoothSwingB.count=8;
  for(int i=0; i<liveSettings.smoothSwingB.count; i++)
  {
    liveSettings.smoothSwingB.sounds[i] = "SMOOTHSWINGL_"+String(i+1)+"_0.RAW";
  }
}

void CoreSettings::loadDefaultColors()
{
  liveSettings.version = CURRENTVERSION;
  
  if (liveSettings.hwVersion == 1)
  {  
    liveSettings.colorSet[RED] = {255, 0, 0, 0};        // 0 RED
    liveSettings.colorSet[ORANGE] = {150, 255, 0, 20};  // 1 ORANGE
    liveSettings.colorSet[YELLOW] = {100, 255, 0, 60};  // 2 YELLOW
    liveSettings.colorSet[GREEN] = {0, 255, 0, 0};      // 3 GREEN
    liveSettings.colorSet[WHITE] = {25, 170, 150, 255}; // 4 WHITE
    liveSettings.colorSet[ACQUA] = {0, 255, 240, 80};   // 5 ACQUA
    liveSettings.colorSet[BLUE] = {0, 0, 255, 0};       // 6 BLUE
    liveSettings.colorSet[PURPLE] = {35, 10, 255, 10};  // 7 PURPLE
    liveSettings.colorSet[OFF] = {0, 0, 0, 0};          // 8 OFF

    liveSettings.clashSet[RED] = {25, 170, 150, 255};
    liveSettings.clashSet[ORANGE] = {25, 170, 150, 255};
    liveSettings.clashSet[YELLOW] = {25, 170, 150, 255};
    liveSettings.clashSet[GREEN] = {25, 170, 150, 255};
    liveSettings.clashSet[WHITE] = {0, 255, 240, 80}; // WHITE flashes ACQUA
    liveSettings.clashSet[ACQUA] = {25, 170, 150, 255};
    liveSettings.clashSet[BLUE] = {25, 170, 150, 255};
    liveSettings.clashSet[PURPLE] = {25, 170, 150, 255};
    liveSettings.clashSet[OFF] = {0, 0, 0, 0};

    liveSettings.swingSet[RED] = {128, 0, 0, 255};
    liveSettings.swingSet[ORANGE] = {100, 255, 0, 90};
    liveSettings.swingSet[YELLOW] = {64, 196, 0, 255};
    liveSettings.swingSet[GREEN] = {0, 196, 0, 255};
    liveSettings.swingSet[WHITE] = {0, 0, 128, 255};
    liveSettings.swingSet[ACQUA] = {0, 196, 128, 255};
    liveSettings.swingSet[BLUE] = {0, 0, 196, 255};
    liveSettings.swingSet[PURPLE] = {35, 10, 196, 255};
    liveSettings.swingSet[OFF] = {0, 0, 0, 0};
  }
  else
  {
    liveSettings.colorSet[RED] = {255, 0, 0, 0};        // 0 RED
    liveSettings.colorSet[ORANGE] = {255, 128, 0, 0};  // 1 ORANGE
    liveSettings.colorSet[YELLOW] = {180, 255, 0, 0};  // 2 YELLOW
    liveSettings.colorSet[GREEN] = {0, 255, 0, 0};      // 3 GREEN
    liveSettings.colorSet[WHITE] = {20, 62, 44, 255}; // 4 WHITE
    liveSettings.colorSet[ACQUA] = {0, 255, 240, 80};   // 5 ACQUA
    liveSettings.colorSet[BLUE] = {0, 0, 255, 0};       // 6 BLUE
    liveSettings.colorSet[PURPLE] = {59, 44, 255, 0};  // 7 PURPLE
    liveSettings.colorSet[OFF] = {0, 0, 0, 0};          // 8 OFF

    liveSettings.clashSet[RED] = {25, 170, 150, 255};
    liveSettings.clashSet[ORANGE] = {25, 170, 150, 255};
    liveSettings.clashSet[YELLOW] = {25, 170, 150, 255};
    liveSettings.clashSet[GREEN] = {25, 170, 150, 255};
    liveSettings.clashSet[WHITE] = {0, 255, 240, 80}; // WHITE flashes ACQUA
    liveSettings.clashSet[ACQUA] = {25, 170, 150, 255};
    liveSettings.clashSet[BLUE] = {25, 170, 150, 255};
    liveSettings.clashSet[PURPLE] = {25, 170, 150, 255};
    liveSettings.clashSet[OFF] = {0, 0, 0, 0};

    liveSettings.swingSet[RED] = {255, 0, 0, 30};
    liveSettings.swingSet[ORANGE] = {255, 160, 0, 0};
    liveSettings.swingSet[YELLOW] = {180, 255, 0, 40};
    liveSettings.swingSet[GREEN] = {40, 255, 0, 40};
    liveSettings.swingSet[WHITE] = {0, 64, 108, 255};
    liveSettings.swingSet[ACQUA] = {0, 196, 128, 255};
    liveSettings.swingSet[BLUE] = {0, 64, 255, 26};
    liveSettings.swingSet[PURPLE] = {35, 10, 196, 125};
    liveSettings.swingSet[OFF] = {0, 0, 0, 0};
  }
  
}

void CoreSettings::readFromStore()
{
  if (!SerialFlash.ready())
  {
    CoreLogging::writeLine("CoreSettings: ERROR, readFromStore, SeriaFlash Not Ready");
    return;
  }

  if (!SerialFlash.exists(configFilename))
  {
    CoreLogging::writeLine("CoreSettings: Config file %s does not exist.", configFilename);
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
  {
    CoreLogging::writeLine("CoreSettings: Failed to read file, using default configuration");
    loadDefaults();
    return;
  }
  else
  {
    JsonObject obj = doc.as<JsonObject>();

    liveSettings.version = obj["version"].as<String>();
    if (!liveSettings.version.equals(CURRENTVERSION))
    { // wrong format filetype so load defaults
      loadDefaults();
      return;
    }

    liveSettings.activeBank =  max(0, min(doc["activeBank"] | BLUE, COLORS));

    for (int i = 0; i <= 7; i++)
    {
      liveSettings.colorSet[i].red = doc["bank"][i]["color"]["red"] | 0;
      liveSettings.colorSet[i].green = doc["bank"][i]["color"]["green"] | 0;
      liveSettings.colorSet[i].blue = doc["bank"][i]["color"]["blue"] | 0;
      liveSettings.colorSet[i].white = doc["bank"][i]["color"]["white"] | 0;

      liveSettings.clashSet[i].red = doc["bank"][i]["clash"]["red"] | 0;
      liveSettings.clashSet[i].green = doc["bank"][i]["clash"]["green"] | 0;
      liveSettings.clashSet[i].blue = doc["bank"][i]["clash"]["blue"] | 0;
      liveSettings.clashSet[i].white = doc["bank"][i]["clash"]["white"] | 0;

      liveSettings.swingSet[i].red = doc["bank"][i]["swing"]["red"] | 0;
      liveSettings.swingSet[i].green = doc["bank"][i]["swing"]["green"] | 0;
      liveSettings.swingSet[i].blue = doc["bank"][i]["swing"]["blue"] | 0;
      liveSettings.swingSet[i].white = doc["bank"][i]["swing"]["white"] | 0;
    }

    loadDefaultSounds();
    
    //on off hum clash swing smoothSwingA smoothSwingB
    if(doc["sounds"]["on"])
    { liveSettings.on.count = doc["sounds"]["on"].size();
      for(unsigned int i=0; i<liveSettings.on.count; i++)
      { String s = doc["sounds"]["on"][i];
        liveSettings.on.sounds[i] = s;
      }
    }
    if(doc["sounds"]["off"])
    { liveSettings.off.count = doc["sounds"]["off"].size();
      for(unsigned int i=0; i<liveSettings.off.count; i++)
      { String s = doc["sounds"]["off"][i];
        liveSettings.off.sounds[i] = s;
      }
    }
    if(doc["sounds"]["hum"])
    { liveSettings.hum.count = doc["sounds"]["hum"].size();
      for(unsigned int i=0; i<liveSettings.hum.count; i++)
      { String s = doc["sounds"]["hum"][i];
        liveSettings.hum.sounds[i] = s;
      }
    }
    if(doc["sounds"]["clash"])
    { liveSettings.clash.count = doc["sounds"]["clash"].size();
      for(unsigned int i=0; i<liveSettings.clash.count; i++)
      { String s = doc["sounds"]["clash"][i];
        liveSettings.clash.sounds[i] = s;
      }
    }
    if(doc["sounds"]["swing"])
    { liveSettings.swing.count = doc["sounds"]["swing"].size();
      for(unsigned int i=0; i<liveSettings.swing.count; i++)
      { String s = doc["sounds"]["swing"][i];
        liveSettings.swing.sounds[i] = s;
      }
    }
    if(doc["sounds"]["smoothSwingA"])
    { liveSettings.smoothSwingA.count = doc["sounds"]["smoothSwingA"].size();
      for(unsigned int i=0; i<liveSettings.smoothSwingA.count; i++)
      { String s = doc["sounds"]["smoothSwingA"][i];
        liveSettings.smoothSwingA.sounds[i] = s;
      }
    }
    if(doc["sounds"]["smoothSwingB"])
    { liveSettings.smoothSwingB.count = doc["sounds"]["smoothSwingB"].size();
      for(unsigned int i=0; i<liveSettings.smoothSwingB.count; i++)
      { String s = doc["sounds"]["smoothSwingB"][i];
        liveSettings.smoothSwingB.sounds[i] = s;
      }
    }

  }
}
void CoreSettings::saveToStore()
{
  if (!SerialFlash.ready())
  {
    CoreLogging::writeLine("CoreSettings: ERROR, saveToStore, SeriaFlash Not Ready");
    return;
  }

  if (!SerialFlash.exists(configFilename))
  {
    if (!SerialFlash.createErasable(configFilename, 65535))
    {
      CoreLogging::writeLine("CoreSettings: ERROR, savetoStore, unable to create file %s", configFilename);
      return;
    }
  }

  SerialFlashFile file = SerialFlash.open(configFilename);
  file.erase();
  StaticJsonDocument<16384> doc;
  // Set the values in the document
  doc["version"] = liveSettings.version;
  doc["activeBank"] = liveSettings.activeBank;

  for (int i = 0; i <= 7; i++)
  {
    doc["bank"][i]["color"]["red"] = liveSettings.colorSet[i].red;
    doc["bank"][i]["color"]["green"] = liveSettings.colorSet[i].green;
    doc["bank"][i]["color"]["blue"] = liveSettings.colorSet[i].blue;
    doc["bank"][i]["color"]["white"] = liveSettings.colorSet[i].white;

    doc["bank"][i]["clash"]["red"] = liveSettings.clashSet[i].red;
    doc["bank"][i]["clash"]["green"] = liveSettings.clashSet[i].green;
    doc["bank"][i]["clash"]["blue"] = liveSettings.clashSet[i].blue;
    doc["bank"][i]["clash"]["white"] = liveSettings.clashSet[i].white;

    doc["bank"][i]["swing"]["red"] = liveSettings.swingSet[i].red;
    doc["bank"][i]["swing"]["green"] = liveSettings.swingSet[i].green;
    doc["bank"][i]["swing"]["blue"] = liveSettings.swingSet[i].blue;
    doc["bank"][i]["swing"]["white"] = liveSettings.swingSet[i].white;
  }

  doc["sounds"]["soundengine"] = liveSettings.soundEngine;    

  //on off hum clash swing smoothSwingA smoothSwingB
  for(int i=0; i<liveSettings.on.count; i++)
  { doc["sounds"]["on"][i] = liveSettings.on.sounds[i];
  }
  for(int i=0; i<liveSettings.off.count; i++)
  { doc["sounds"]["off"][i] = liveSettings.off.sounds[i];
  }
  for(int i=0; i<liveSettings.hum.count; i++)
  { doc["sounds"]["hum"][i] = liveSettings.hum.sounds[i];
  }
  for(int i=0; i<liveSettings.clash.count; i++)
  { doc["sounds"]["clash"][i] = liveSettings.clash.sounds[i];
  }
  for(int i=0; i<liveSettings.swing.count; i++)
  { doc["sounds"]["swing"][i] = liveSettings.swing.sounds[i];
  }
for(int i=0; i<liveSettings.smoothSwingA.count; i++)
  { doc["sounds"]["smoothSwingA"][i] = liveSettings.smoothSwingA.sounds[i];
  }
for(int i=0; i<liveSettings.smoothSwingB.count; i++)
  { doc["sounds"]["smoothSwingB"][i] = liveSettings.smoothSwingB.sounds[i];
  }

  // Serialize JSON to file
  char buffer[16384];
  uint32_t sz = serializeJsonPretty(doc, buffer); //<-- writes json in a more human readable format
  // uint32_t sz = serializeJson(doc, buffer);    //<-- writes a smaller file, but the block size is 64K anyway
  if (sz == 0)
  {
    CoreLogging::writeLine("CoreSettings: Error, saveToStore, Failed to write to file");
  }
  file.write(buffer, sz);

  // Close the file
  file.close();
}

String CoreSettings::getOnSounds()
{ return liveSettings.on.getCSV();
}
void CoreSettings::setOnSounds(String csv)
{ return liveSettings.on.setCSV(csv);
}
String CoreSettings::getRandomOnSound()
{ return liveSettings.on.getRandom();
}

String CoreSettings::getOffSounds()
{ return liveSettings.off.getCSV();
}
void CoreSettings::setOffSounds(String csv)
{ return liveSettings.off.setCSV(csv);
}
String CoreSettings::getRandomOffSound()
{ return liveSettings.off.getRandom();
}

String CoreSettings::getHumSounds()
{ return liveSettings.hum.getCSV();
}
void CoreSettings::setHumSounds(String csv)
{ return liveSettings.hum.setCSV(csv);
}
String CoreSettings::getRandomHumSound()
{ return liveSettings.hum.getRandom();
}

String CoreSettings::getSwingSounds()
{ return liveSettings.swing.getCSV();
}
void CoreSettings::setSwingSounds(String csv)
{ return liveSettings.swing.setCSV(csv);
}
String CoreSettings::getRandomSwingSound()
{ return liveSettings.swing.getRandom();
}

String CoreSettings::getSmoothSwingSoundsA()
{ return liveSettings.smoothSwingA.getCSV();
}
void CoreSettings::setSmoothSwingSoundsA(String csv)
{ return liveSettings.smoothSwingA.setCSV(csv);
}
String CoreSettings::getSmoothSwingSoundsB()
{ return liveSettings.smoothSwingB.getCSV();
}
void CoreSettings::setSmoothSwingSoundsB(String csv)
{ return liveSettings.smoothSwingB.setCSV(csv);
}
String CoreSettings::getRandomSmoothSwingSoundA()
{ return liveSettings.smoothSwingA.getRandom();
}
String CoreSettings::getMatchingSmoothSwingSoundB()
{ return liveSettings.smoothSwingB.sounds[liveSettings.smoothSwingA.lastRandom];
}
int CoreSettings::getSmoothSwingSize()
{
  return min(liveSettings.smoothSwingA.count, liveSettings.smoothSwingB.count);
}

String CoreSettings::getClashSounds()
{ return liveSettings.clash.getCSV();
}
void CoreSettings::setClashSounds(String csv)
{ return liveSettings.clash.setCSV(csv);
}
String CoreSettings::getRandomClashSound()
{ return liveSettings.clash.getRandom();
}

void CoreSettings::printFile(const char* filen, boolean ignore)
{
  if (!SerialFlash.ready())
  {
    CoreLogging::writeLine("CoreSettings: ERROR, printFile, SeriaFlash Not Ready");
    return;
  }
  if (!SerialFlash.exists(filen))
  {
    CoreLogging::writeLine("CoreSettings: ERROR, printFile, %s does not exist", filen);
    return;
  }
  char buf[1];
  SerialFlashFile file = SerialFlash.open(filen);
  while (file.available())
  {
    noInterrupts();
    WDOG_REFRESH = 0xA602;
    WDOG_REFRESH = 0xB480;
    interrupts();
    file.read(buf, 1);
    if (ignore && ((buf[0] == (char)255) || (buf[0] < 9)))
      break;
    else
      Serial.print(buf[0]);
  }
  file.close();
}

int32_t CoreSettings::getFileSize(const char* filen)
{
  int32_t sz = -88;

  if (!SerialFlash.ready())
  {
    CoreLogging::writeLine("CoreSettings: ERROR, SeriaFlash Not Ready");
    return -99;
  }
  if (!SerialFlash.exists(filen))
  {
    CoreLogging::writeLine("CoreSettings: ERROR, file %s does not exist", filen);
    return -1;
  }
  SerialFlashFile file = SerialFlash.open(filen);
  sz = file.size();
  file.close();

  return sz;
}

void CoreSettings::setActiveBank(int iBank)
{ 
  liveSettings.activeBank = max(0, min(iBank, COLORS));
  saveToStore();
}

int CoreSettings::getActiveBank()
{
  return liveSettings.activeBank;
}

ColorLed CoreSettings::getMainColor(int bank)
{
  return liveSettings.colorSet[bank];
}

ColorLed CoreSettings::getClashColor(int bank)
{
  return liveSettings.clashSet[bank];
}

ColorLed CoreSettings::getSwingColor(int bank)
{
  return liveSettings.swingSet[bank];
}

void CoreSettings::setMainColor(int bank, ColorLed cc)
{
  liveSettings.colorSet[bank] = cc;
}

void CoreSettings::setClashColor(int bank, ColorLed cc)
{
  liveSettings.clashSet[bank] = cc;
}

void CoreSettings::setSwingColor(int bank, ColorLed cc)
{
  liveSettings.swingSet[bank] = cc;
}
