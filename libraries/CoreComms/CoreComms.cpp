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
void CoreComms::setModule(CoreSettings* cSet)
{
  //ledmodule = cled;
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

  if (pIncomingMessage.equalsIgnoreCase("V"))
  {
    printDevInfo();
  }
  else if (pIncomingMessage.substring(0,1).equalsIgnoreCase("#"))
  {
    Serial.print("#");
  }
  else if (pIncomingMessage.equalsIgnoreCase("V?"))
  {
    out = "V=" + build;
  }
  else if (pIncomingMessage == "S?")
  {
    out = "S=" + serial;
  }
  else if (pIncomingMessage.equalsIgnoreCase("ERASE=ALL"))
  { SerialFlash.eraseAll();
    int i=0;
    Serial.println("Erasing Serial Flash, this may take 20s to 2 minutes");
    while (SerialFlash.ready() == false)
    {
      delay(500);
      i++;
      Serial.print("#");
      // wait, 30 seconds to 2 minutes for most chips
      if(i>=20)
      {
        Serial.print("\n");
        i=0;
      }
    }
    out="\nOK, Now re-load your sound files.\nOK, Serial Flash Erased.\n";
  }
  else if (pIncomingMessage.substring(0,5).equalsIgnoreCase("ERASE"))
  {
    out="ERROR, ERASE aborted, incomplete erase command.";
  }
  else if (pIncomingMessage.equalsIgnoreCase ("WR?") )
  {
    out="OK, Write Ready";
  }
  else if (pIncomingMessage.substring(0,3).equalsIgnoreCase("WR="))
  { 
    // W=filename,size\n   --binary-file-stream--
    int pos = pIncomingMessage.indexOf(',');
    if(pos<0)
    {
      out="ERROR, Write Invalid format";
    }
    else
    { 
      String fname=pIncomingMessage.substring(3,pIncomingMessage.indexOf(',')).trim();
      long fsize=pIncomingMessage.substring(pIncomingMessage.indexOf(',')+1).trim().toInt();

      Serial.print("OK, Write "+fname+", "+String(fsize)+".\n");
      
      if(fname.length()==0)
      {
        out="ERROR, Write Invalid filename";
      }
      else if(fsize<=0)
      {
        out="ERROR, Write Invalid file length";
      }
      else
      {
        if(!SerialFlash.ready())
        {
          out="ERROR, Serial Flash Not Ready";
        }
        else
        { 
          char filename[fname.length() + 1];
          fname.toCharArray(filename,64);
          // Remove if it already exists
          SerialFlash.opendir();

          if( SerialFlash.exists(filename) )
          {
            SerialFlash.remove(filename);
          }
          //now read incoming stream to flash
          boolean success=false;
          if(fname.toLowerCase().endsWith(".raw"))
          { 
            //raw files are binary -- so not erasable and exact size
            success=SerialFlash.create(filename, fsize);
          }
          else
          { 
            //all other files are ascii, so set as eraseable -- 65535 is the blocksize, so files mightas well be blocksize
            success=SerialFlash.createErasable(filename, max(65535,fsize));
          }
          
          if(success)
          {
            // Opening file to write it
            SerialFlashFile file = SerialFlash.open(filename);
            if (file)
            {
              long counter=0;
              unsigned long time = millis();

              while (counter<fsize)
              { 
                while(!Serial.available())
                { delay(5);
                  if((millis()-time)>10000)
                  {
                    //if no comms for 10 seconds then timeout the transfer
                    time=-1;
                    break;
                  }
                }
                byte data[] = {Serial.read()};
                file.write(data, 1);
                
                counter++;
                time = millis();
              }
              if(time<0)
              {
                file.erase();
                out="ERROR, Write Timed-out";
              }
              else
              {
                out="OK, Write Complete";
              }              
              file.close();
            }
            else
            {
            out="ERROR, Unable to create file (2)";
            }
          }
          else
          {
            out="ERROR, Unable to create file (1)";
          }
          

        }

      }
      
      
    }
        
  }
  else if (pIncomingMessage.substring(0,3).equalsIgnoreCase("RD?") )
  {
    //read file
    String lc=pIncomingMessage;
    lc=lc.toLowerCase(); 
    if(!lc.endsWith(".raw"))
    { 
      //if not a binary sound file send STX and then ETX
      Serial.print(STX);
    }
    char filename[64];
    pIncomingMessage.substring(3).toCharArray(filename,64);
    setmodule->printFile(filename, true);
    if(!lc.endsWith(".raw"))
    { 
      //if not a binary WAV file send STX and then ETX
      Serial.print(ETX);
    }
  }
  else if (pIncomingMessage.equalsIgnoreCase("FREE?"))
  {
    out = "FREE="+String(getStorageFree());
  }
  else if (pIncomingMessage.equalsIgnoreCase("USED?"))
  {
    out = "USED="+String(getStorageUsed());
  }
  else if (pIncomingMessage.equalsIgnoreCase("SIZE?"))
  {
    out = "SIZE="+String(getStorageCapacity());
  }
  else if (pIncomingMessage.equalsIgnoreCase("LIST?"))
  { 
    out = listFiles();
  }
  else if (pIncomingMessage.equalsIgnoreCase("sON?"))
  { 
    out = "sON="+setmodule->getOnSounds();
  }
  else if (pIncomingMessage.startsWith("sON="))
  { 
    setmodule->setOnSounds(pIncomingMessage.substring(4));
    out = "OK " + pIncomingMessage;
  }
  else if (pIncomingMessage.equalsIgnoreCase("sOFF?"))
  { 
    out = "sOFF="+setmodule->getOffSounds();
  }
    else if (pIncomingMessage.startsWith("sOFF="))
  { 
    setmodule->setOffSounds(pIncomingMessage.substring(5));
    out = "OK " + pIncomingMessage;
  }
  else if (pIncomingMessage.equalsIgnoreCase("sHUM?"))
  { 
    out = "sHUM="+setmodule->getHumSounds();
  }
  else if (pIncomingMessage.startsWith("sHUM="))
  { 
    setmodule->setHumSounds(pIncomingMessage.substring(5));
    out = "OK " + pIncomingMessage;
  }
  else if (pIncomingMessage.equalsIgnoreCase("sSW?"))
  { 
    out = "sSW=" + setmodule->getSwingSounds();
  }
  else if (pIncomingMessage.startsWith("sSW="))
  { 
    setmodule->setSwingSounds(pIncomingMessage.substring(4));
    out = "OK " + pIncomingMessage;
  }
  else if (pIncomingMessage.equalsIgnoreCase("sCL?"))
  { 
    out = "sCL=" + setmodule->getClashSounds();
  }
  else if (pIncomingMessage.startsWith("sCL="))
  { 
    setmodule->setClashSounds(pIncomingMessage.substring(4));
    out = "OK " + pIncomingMessage;
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
    changeColor({0,0,0,0});
    out="OK, P=0";    
  }
  else if (pIncomingMessage.startsWith("P=") || pIncomingMessage.startsWith("p="))
  {
    changeColor(stringToColorLed(pIncomingMessage.substring(2)));
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

  Serial.println(listFiles());

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
  uint32_t maxAddr=0;
  while (moreFiles)
  {
    // List all files to PC
    char filename[64];
    uint32_t filesize;
    

    if (SerialFlash.readdir(filename, sizeof(filename), filesize))
    {
      lst += String(filename)+spaces(max(1,22 - strlen(filename)));
      lst += String(filesize);
      lst += "\n";

      SerialFlashFile ff = SerialFlash.open(filename);
      maxAddr= max(maxAddr, ff.getFlashAddress()+ff.size());
    }
    else
    {
      moreFiles=false;
    }
  }

  lst += "Serial Flash Chip\nJEDEC ID: ";
  SerialFlash.readID(buf);
  lst += String(buf[0], HEX)+" ";
  lst += String(buf[1], HEX)+" ";
  lst += String(buf[2], HEX)+"\n";

  uint32_t cap = SerialFlash.capacity(buf);
  lst += "Memory Size: "+String(cap);
  lst += " bytes\n";
  lst += "Memory Free: "+String(cap-maxAddr);
  lst += " bytes\n";
  lst += "Memory Used: "+String(maxAddr);
  lst += " bytes\n";
  
  lst += "Block Size: ";
  lst += String(SerialFlash.blockSize());
  lst += " bytes\n";

  lst += char(ETX);
  return lst;
}
String CoreComms::spaces(int num)
{ String out="";
  for (int i = 0; i < num; i++)
  { out += " ";
  }
  return out;
}
uint32_t CoreComms::getStorageCapacity()
{ 
  SerialFlash.opendir();
  unsigned char buf[256];
  SerialFlash.readID(buf);
  return SerialFlash.capacity(buf);
}
uint32_t CoreComms::getStorageUsed()
{
  //must traverse files find last one and add it's size
  SerialFlash.opendir();
  boolean moreFiles=true;
  uint32_t maxAddr=0;
  while (moreFiles)
  {
    char filename[64];
    uint32_t filesize;
    if (SerialFlash.readdir(filename, sizeof(filename), filesize))
    {
      SerialFlashFile ff = SerialFlash.open(filename);
      maxAddr= max(maxAddr, ff.getFlashAddress()+ff.size());
    }
    else
    {
      moreFiles=false;
    }
  }
  return maxAddr;
}
uint32_t CoreComms::getStorageFree()
{
  return getStorageCapacity()-getStorageUsed();
}

void CoreComms::changeColor(const ColorLed& cLed)
{
  analogWrite(PIN_RED, !COMMON_GND ? cLed.red : 255 - cLed.red);
  analogWrite(PIN_GREEN, !COMMON_GND ? cLed.green : 255 - cLed.green);
  analogWrite(PIN_BLUE, !COMMON_GND ? cLed.blue : 255 - cLed.blue);
  analogWrite(PIN_WHITE, !COMMON_GND ? cLed.white : 255 - cLed.white);
}