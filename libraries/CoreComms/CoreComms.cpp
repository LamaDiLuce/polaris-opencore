#include "CoreComms.h"
#include "Arduino.h"

// Constructor
CoreComms::CoreComms()
{
}
// Init
void CoreComms::init(String pBuild)
{
  build = pBuild;
  serial = getKinetisUID();

  Serial.begin(BAUD_RATE); // It needs for PC communication services
  CoreLogging::writeLine("CoreComms: Build: %s", build);
  CoreLogging::writeLine("CoreComms: Serial Number: %s", serial);
}
void CoreComms::setModule(CoreLed* cled, CoreSettings* cSet)
{
  ledmodule = cled;
  setmodule = cSet;
}

// Process loop
void CoreComms::loop()
{
  if (Serial.available() > 0)
  {
    int incomingByte;
    incomingByte = Serial.read();

    // Any incoming string beginning with a # has a single character # response,
    // and can be used to detect the saber model, so ignore STX if string starts with "#"
    if ((incomingByte == STX) && (incomingMessage.charAt(0) != '#'))
    {
      // for future file transfer
      incomingMessage = "";
    }
    else if ((incomingByte == ETX) || (incomingByte == LF))
    {
      processIncomingMessage(incomingMessage);
      incomingMessage = "";
    }
    else
    {
      incomingMessage += (char)incomingByte;
    }
  }
}

String CoreComms::getKinetisUID()
{
  char uidString[32];
  snprintf(uidString, 32, "%08lx-%08lx-%08lx", SIM_UIDMH, SIM_UIDML, SIM_UIDL);
  return String(uidString);
}

void CoreComms::processIncomingMessage(const String& pIncomingMessage)
{
  String out;
  out = "";

  if (pIncomingMessage == "V")
  {
    printDevInfo();
  }
  else if (pIncomingMessage.startsWith("#"))
  {
    Serial.print("#");
  }
  else if (pIncomingMessage == "V?")
  {
    out = "V=" + build;
  }
  else if (pIncomingMessage == "S?")
  {
    out = "S=" + serial;
  }
  else if (pIncomingMessage.charAt(2) == '=')
  {
    // a set somethign command
    // C0=  color 1 = r,g,b,w    or = #0011223344
    // F0=  clash(flash) color 1 = r,g,b,w
    char i = pIncomingMessage.charAt(1);
    if ((i >= '0') && (i <= '7'))
    {
      if ((pIncomingMessage.charAt(0) == 'C') || (pIncomingMessage.charAt(0) == 'c'))
      {
        setmodule->setMainColor(i - 48, stringToColorLed(pIncomingMessage.substring(3)));
        out = "OK " + pIncomingMessage;
      }
      else if ((pIncomingMessage.charAt(0) == 'F') || (pIncomingMessage.charAt(0) == 'f'))
      {
        setmodule->setClashColor(i - 48, stringToColorLed(pIncomingMessage.substring(3)));
        out = "OK " + pIncomingMessage;
      }
      else if ((pIncomingMessage.charAt(0) == 'W') || (pIncomingMessage.charAt(0) == 'w'))
      {
        setmodule->setSwingColor(i - 48, stringToColorLed(pIncomingMessage.substring(3)));
        out = "OK " + pIncomingMessage;
      }
    }
  }
  else if (pIncomingMessage.charAt(2) == '?')
  {
    char i = pIncomingMessage.charAt(1);
    if ((i >= '0') && (i <= '7'))
    {
      if (pIncomingMessage.charAt(0) == 'C')
      {
        out = "C" + String(i - 48) + "=" + colorLedToString(setmodule->getMainColor(i - 48), true); // ascii 48 = 0
      }
      else if (pIncomingMessage.charAt(0) == 'c')
      {
        out = "c" + String(i - 48) + "=" + colorLedToString(setmodule->getMainColor(i - 48), false);
      }
      else if (pIncomingMessage.charAt(0) == 'F')
      {
        out = "F" + String(i - 48) + "=" + colorLedToString(setmodule->getClashColor(i - 48), true);
      }
      else if (pIncomingMessage.charAt(0) == 'f')
      {
        out = "f" + String(i - 48) + "=" + colorLedToString(setmodule->getClashColor(i - 48), false);
      }
      else if (pIncomingMessage.charAt(0) == 'W')
      {
        out = "W" + String(i - 48) + "=" + colorLedToString(setmodule->getSwingColor(i - 48), true);
      }
      else if (pIncomingMessage.charAt(0) == 'w')
      {
        out = "w" + String(i - 48) + "=" + colorLedToString(setmodule->getSwingColor(i - 48), false);
      }
    }
  }
  else if (pIncomingMessage == "C?")
  {
    for (int i = 0; i <= 7; i++)
    {
      out += "C" + String(i) + "=" + colorLedToString(setmodule->getMainColor(i), true);
      if (i != 7)
      {
        out += "\n";
      }
    }
  }
  else if (pIncomingMessage == "c?")
  {
    for (int i = 0; i <= 7; i++)
    {
      out += "c" + String(i) + "=" + colorLedToString(setmodule->getMainColor(i), false);
      if (i != 7)
      {
        out += "\n";
      }
    }
  }
  else if (pIncomingMessage == "F?")
  {
    for (int i = 0; i <= 7; i++)
    {
      out += "F" + String(i) + "=" + colorLedToString(setmodule->getClashColor(i), true);
      if (i != 7)
      {
        out += "\n";
      }
    }
  }
  else if (pIncomingMessage == "f?")
  {
    for (int i = 0; i <= 7; i++)
    {
      out += "f" + String(i) + "=" + colorLedToString(setmodule->getClashColor(i), false);
      if (i != 7)
      {
        out += "\n";
      }
    }
  }
  else if (pIncomingMessage == "W?")
  {
    for (int i = 0; i <= 7; i++)
    {
      out += "W" + String(i) + "=" + colorLedToString(setmodule->getSwingColor(i), true);
      if (i != 7)
      {
        out += "\n";
      }
    }
  }
  else if (pIncomingMessage == "w?")
  {
    for (int i = 0; i <= 7; i++)
    {
      out += "w" + String(i) + "=" + colorLedToString(setmodule->getSwingColor(i), false);
      if (i != 7)
      {
        out += "\n";
      }
    }
  }
  else if (pIncomingMessage.equalsIgnoreCase("P=0"))
  {
    ledmodule->changeColor({0,0,0,0});
    out="OK, P=0";    
  }
  else if (pIncomingMessage.startsWith("P=") || pIncomingMessage.startsWith("p="))
  {
    ledmodule->changeColor(stringToColorLed(pIncomingMessage.substring(2)));
    out="OK, "+pIncomingMessage;    
  }
  else if (pIncomingMessage.startsWith("B="))
  {
    char i = pIncomingMessage.charAt(2);
    if ((i >= '0') && (i <= '7'))
    {
      setmodule->setActiveBank(i - 48);
      out = "OK " + pIncomingMessage;
    }
  }
  else if (pIncomingMessage == "B?")
  {
    out = "B=" + String(setmodule->getActiveBank());
  }
  else if (pIncomingMessage.equalsIgnoreCase("ERASE="+serial) 
           || pIncomingMessage.equalsIgnoreCase("ERASE=ALL"))
  { out ="ERROR, ERASE Not Yet Implemented.";
    SerialFlash.eraseAll();
    int i=0;
    Serial.println("Erasing Serial Flash, this may take 20s to 2 minutes");
    while (SerialFlash.ready() == false)
    {
      // wait, 30 seconds to 2 minutes for most chips
      if(i==0)
      {
        Serial.print("#");
      }
      i++;
      if(i>10)
      {
        i=0;
      }
    }
    Serial.print("OK, Now re-load your sound files.\n");
    Serial.print("OK, Serial Flash Erased.\n");
  }
  else if (pIncomingMessage.startsWith("ERASE") || pIncomingMessage.startsWith("erase") )
  { out="ERROR, ERASE aborted, no security serial no.";
  }
  else if (pIncomingMessage.startsWith("r?") || pIncomingMessage.startsWith("R?") )
  {
    //read file
    if(!pIncomingMessage.endsWith(".wav") && !pIncomingMessage.endsWith(".WAV"))
    { 
      //if not a binary WAV file send STX and then ETX
      Serial.print(STX);
    }
    char filename[64];
    pIncomingMessage.substring(2).toCharArray(filename,64);
    setmodule->printFile(filename, true);
    if(!pIncomingMessage.endsWith(".wav") && !pIncomingMessage.endsWith(".WAV"))
    { 
      //if not a binary WAV file send STX and then ETX
      Serial.print(ETX);
    }
  }
  else if (pIncomingMessage.equalsIgnoreCase("LIST?"))
  { 
    out = listFiles();
  }
  else if (pIncomingMessage == "RESET")
  { // ledmodule->loadDefaultColors();
    setmodule->loadDefaults();
    out = "OK RESET";
  }
  else if (pIncomingMessage == "SAVE")
  { //@TODO: call non-volatile settings module to save all settings to non-volatile storage
    commsMode = MODE_NORMAL;
    setmodule->saveToStore();
    out = "OK SAVE";
  }

  if (out.length() != 0)
  {
    if (commsMode == MODE_NORMAL)
    {
      commsMode = MODE_CONFIG;
      //@TODO: switch saber to off standby????
    }
    // future if in serial transfer mode do not interrupt with normal comms
    // only reply when in CONFIG mode, or having just switched out of NORMAL mode
    if (commsMode == MODE_CONFIG)
    {
      Serial.print(out + char(LF));
    }
  }
  
}

ColorLed CoreComms::stringToColorLed(String sColor)
{
  ColorLed cc = {0, 0, 0, 0};
  String strip = sColor.trim();

  if ((strip.indexOf(",") < 0) && (strip.indexOf(";") < 0) && (strip.indexOf(" ") < 0))
  { // no punctuation means hexadecimal
    // hex number - remove any hexadecimal prefixes
    if (strip.startsWith("#"))
    {
      strip = strip.substring(1);
    }
    else if (strip.startsWith("0x"))
    {
      strip = strip.substring(2);
    }
    u_int32_t rgbw = 0;
    for (unsigned int i = 0; i < strip.length(); i++)
    {
      char c = strip.charAt(i);
      rgbw = (rgbw * 16);
      if (c <= '9')
      {
        rgbw += (c - '0');
      }
      else if (c <= 'F')
      {
        rgbw += 10 + c - 'A';
      }
      else if (c <= 'f')
      {
        rgbw += 10 + c - 'a';
      }
    }
    cc.red = (rgbw >> 24) & 255;
    cc.green = (rgbw >> 16) & 255;
    cc.blue = (rgbw >> 8) & 255;
    cc.white = rgbw & 255;
  }
  else
  { // csv list "r,g,b,w"
    char sep = ',';
    int pos = strip.indexOf(sep);
    if (pos < 0)
    {
      sep = ';';
      pos = strip.indexOf(sep);
    }
    if (pos < 0)
    {
      sep = ' ';
      pos = strip.indexOf(sep);
    }
    if (pos > 0)
    { // logger.writeLine("strtoColorLed sep="+sep);
      cc.red = strip.substring(0, pos).toInt();
      strip = strip.substring(pos + 1);

      pos = strip.indexOf(sep);
      if (pos > 0)
      {
        cc.green = strip.substring(0, pos).toInt();
        strip = strip.substring(pos + 1);

        pos = strip.indexOf(sep);
        if (pos > 0)
        {
          cc.blue = strip.substring(0, pos).toInt();
          strip = strip.substring(pos + 1);

          cc.white = strip.toInt();
        }
      }
    }
  }
  return cc;
}

char CoreComms::hexDigit(int n)
{                                  // fast binary int to ascii hex
  return n + ((n < 10) ? 48 : 55); // use 87 for lowercase a, 55 for uppercase A
}

String CoreComms::colorLedToString(ColorLed cColor, boolean bHex)
{
  if (bHex)
  {
    String hx = "";
    hx = hx + hexDigit((cColor.red & 0xF0) >> 4) + hexDigit(cColor.red & 0x0f) + hexDigit((cColor.green & 0xF0) >> 4) +
         hexDigit(cColor.green & 0x0f) + hexDigit((cColor.blue & 0xF0) >> 4) + hexDigit(cColor.blue & 0x0f) +
         hexDigit((cColor.white & 0xF0) >> 4) + hexDigit(cColor.white & 0x0f);
    return hx;
  }
  else
  {
    return String(cColor.red) + "," + String(cColor.green) + "," + String(cColor.blue) + "," + String(cColor.white);
  }
}

byte CoreComms::getMode()
{
  return commsMode;
}
void CoreComms::setMode(byte cmode)
{
  commsMode = cmode;
}
void CoreComms::printDevInfo()
{
  Serial.write(STX);
  Serial.print("Build: ");
  Serial.println(build);
  Serial.print("Serial Number: ");
  Serial.println(serial);
  Serial.print("SerialFlash connecting...\n");
  SerialFlash.opendir();
  unsigned char buf[256];
  Serial.println("Files on memory:");

  while (1)
  {
    // List all files to PC
    char filename[64];
    uint32_t filesize;

    if (SerialFlash.readdir(filename, sizeof(filename), filesize))
    {
      Serial.print(filename);
      Serial.print(" ");
      Serial.print(spaces(20 - strlen(filename)));
      Serial.print(" (");
      Serial.print(filesize);
      Serial.print(")\n");

      Serial.print("settings getfilesize: ");
      Serial.print(setmodule->getFileSize(filename));
      Serial.print(" bytes\n");
    }
    else
    {
      break; // no more files
    }
  }
  
  Serial.println("Read Chip Identification:");
  SerialFlash.readID(buf);
  Serial.print("  JEDEC ID:     ");
  Serial.print(buf[0], HEX);
  Serial.print(" ");
  Serial.print(buf[1], HEX);
  Serial.print(" ");
  Serial.println(buf[2], HEX);
  Serial.print("  Memory Size:  ");
  uint32_t chipsize = SerialFlash.capacity(buf);
  Serial.print(chipsize);
  Serial.println(" bytes");
  Serial.print("  Block Size:   ");
  Serial.print(SerialFlash.blockSize());
  Serial.println(" bytes");

  Serial.write(ETX);
}
String CoreComms::listFiles()
{ String lst="";

  lst=char(STX);
  //Serial.print("SerialFlash connecting...\n");
  SerialFlash.opendir();
  unsigned char buf[256];
  lst += "Files on memory:\n";
  boolean moreFiles=true;
  while (moreFiles)
  {
    // List all files to PC
    char filename[64];
    uint32_t filesize;

    if (SerialFlash.readdir(filename, sizeof(filename), filesize))
    {
      lst += String(filename)+spaces(26 - strlen(filename))+" ";
      lst += String(filesize);
      lst += "\n";
    }
    else
    {
      moreFiles=false;
    }
  }

  lst += "Serial Flash Chip JEDEC ID: ";
  SerialFlash.readID(buf);
  lst += String(buf[0], HEX)+" ";
  lst += String(buf[1], HEX)+" ";
  lst += String(buf[2], HEX)+"\n";

  lst += "Memory Size: ";
  uint32_t chipsize = SerialFlash.capacity(buf);
  lst += String(chipsize);
  lst += " bytes\n";
  lst += "Block Size: ";
  lst += String(SerialFlash.blockSize());
  lst += " bytes\n";

  lst += char(ETX);
  return lst;

      /*
    SerialFlash.opendir();
    unsigned char buf[256];
    boolean morefiles=true; 
    while (morefiles)
    {
      // List all files to Serial
      char filename[64];
      uint32_t filesize;

      out = "";
      if (SerialFlash.readdir(filename, sizeof(filename), filesize))
      {
        out += filename + spaces(22 - strlen(filename));
        out += String(filesize)+"\n";
        //out += "settings getfilesize: " + setmodule->getFileSize(filename) + " bytes\n");
      } else
      {
        morefiles=false;
      }
    }
    out += "Serial Flash Chip JEDEC ID:  " ;
    SerialFlash.readID(buf);
    out += String(buf[0], HEX)+" "+String(buf[1], HEX)+" "+String(buf[2], HEX)+"\n";

    //SerialFlash.readSerialNumber(buf);
    //out += "SF SN: "+String(buf)+"\n";

    uint32_t chipsize = SerialFlash.capacity(buf);
    out += "Memory Size: "+String(chipsize)+" bytes\n";
    out += "Block Size: "+String(SerialFlash.blockSize())+" bytes\n";
    */
}
String CoreComms::spaces(int num)
{ String out="";
  for (int i = 0; i < num; i++)
  { out += " ";
  }
  return out;
}
