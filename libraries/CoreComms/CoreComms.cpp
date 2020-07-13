#include "Arduino.h"
#include "CoreComms.h"

//Constructor
CoreComms::CoreComms()
{
}
//Init
void CoreComms::init(String pBuild)
{
  build = pBuild;
  serial = kinetisUID();

  Serial.begin(BAUD_RATE); //It needs for PC communication services
  CoreLogging::writeParamString("Build", build);
  CoreLogging::writeParamString("Serial Number", serial);

}
void CoreComms::setModule(CoreLed *cled, CoreSettings *cSet)
{ ledmodule = cled;
  setmodule = cSet;
}

//Process loop
void CoreComms::loop()
{   
  if (Serial.available() > 0)
  {
    int incomingByte;
    incomingByte = Serial.read();
    
    //Any incoming string beginning with a # has a single character # response,
    //and can be used to detect the saber model, so ignore STX if string starts with "#"
    if ((incomingByte == STX) && (incomingMessage.charAt(0)!='#'))
    { //for future file transfer
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

void CoreComms::kinetisUID(uint32_t *uid)
{
  uid[0] = SIM_UIDMH;
  uid[1] = SIM_UIDML;
  uid[2] = SIM_UIDL;
}
const char *CoreComms::kinetisUID(void)
{
  uint32_t uid[3];
  static char uidString[27];
  kinetisUID(uid);
  sprintf(uidString, "%08lx-%08lx-%08lx", uid[0], uid[1], uid[2]);
  return uidString;
}

void CoreComms::processIncomingMessage(const String& pIncomingMessage)
{ 
  String out;
  out="";

  if (pIncomingMessage == "V")
  { printDevInfo();
  }
  else if(pIncomingMessage.startsWith("#"))
  { Serial.print("#");
  }
  else if (pIncomingMessage == "V?")
  { out="V="+build;
  }
  else if (pIncomingMessage == "S?")
  { out="S="+serial;
  }
  else if(pIncomingMessage.charAt(2)=='=')
  { //a set somethign command
    // C0=  color 1 = r,g,b,w    or = #0011223344
    // F0=  clash(flash) color 1 = r,g,b,w
    char i=pIncomingMessage.charAt(1);
    if((i>='0') && (i<='7'))
    { if((pIncomingMessage.charAt(0)=='C') || (pIncomingMessage.charAt(0)=='c'))
      { setMainColor(i-48, pIncomingMessage.substring(3) );
        out="OK "+pIncomingMessage;
      } else if((pIncomingMessage.charAt(0)=='F') || (pIncomingMessage.charAt(0)=='f'))
      { setClashColor(i-48, pIncomingMessage.substring(3) );
        out="OK "+pIncomingMessage;
      }
    }
  }
  else if(pIncomingMessage.charAt(2)=='?')
  { char i=pIncomingMessage.charAt(1);
    if((i>='0') && (i<='7'))
    { if(pIncomingMessage.charAt(0)=='C')
      { out="C"+String(i-48)+"="+colorLedToString(getMainColor(i-48),true);  //ascii 48 = 0
      } else if(pIncomingMessage.charAt(0)=='c')
      { out="c"+String(i-48)+"="+colorLedToString(getMainColor(i-48),false);
      } else if(pIncomingMessage.charAt(0)=='F')
      { out="F"+String(i-48)+"="+colorLedToString(getClashColor(i-48),true);
      } else if(pIncomingMessage.charAt(0)=='f')
      { out="f"+String(i-48)+"="+colorLedToString(getClashColor(i-48),false);
      } 
    }
  }
  else if(pIncomingMessage=="C?")
  { for(int i=0; i<=7; i++)
    { out+="C"+String(i)+"="+colorLedToString(getMainColor(i),true);
      if(i!=7)
      { out+="\n";
      }
    }
  }  
  else if(pIncomingMessage=="c?")
  { for(int i=0; i<=7; i++)
    { out+="c"+String(i)+"="+colorLedToString(getMainColor(i),false);
      if(i!=7)
      { out+="\n";
      }
    }
  }
  else if(pIncomingMessage=="F?")
  { for(int i=0; i<=7; i++)
    { out+="F"+String(i)+"="+colorLedToString(getClashColor(i),true);
      if(i!=7)
      { out+="\n";
      }
    }
  }  
  else if(pIncomingMessage=="f?")
  { for(int i=0; i<=7; i++)
    { out+="f"+String(i)+"="+colorLedToString(getClashColor(i),false);
      if(i!=7)
      { out+="\n";
      }
    }
  }
  else if (pIncomingMessage.startsWith("B="))
  { char i=pIncomingMessage.charAt(2);
    if((i>='0') && (i<='7'))
    { setmodule->setActiveBank(i-48);
      out="OK "+pIncomingMessage;
    }
  }
  else if (pIncomingMessage=="B?")
  { out="B="+String(setmodule->getActiveBank());
  }
  else if(pIncomingMessage=="RESET")
  { //ledmodule->loadDefaultColors();
    setmodule->loadDefaults();
    out="OK RESET";
  }
  else if(pIncomingMessage=="SAVE")
  { //@TODO: call non-volatile settings module to save all settings to non-volatile storage
    commsMode=MODE_NORMAL;
    setmodule->saveToStore();
    out="OK SAVE";
  }
  
  if(out.length()!=0)
  { if(commsMode==MODE_NORMAL)
    { commsMode=MODE_CONFIG;
      //@TODO: switch saber to off standby????
    }
    //future if in serial transfer mode do not interrupt with normal comms
    //only reply when in CONFIG mode, or having just switched out of NORMAL mode
    if(commsMode==MODE_CONFIG)
    { Serial.print(out+char(LF));
    }
  }
}

ColorLed CoreComms::getMainColor(int bank) const
{ //return ledmodule->getMainColor(bank);
  return setmodule->getMainColor(bank);
}
ColorLed CoreComms::getClashColor(int bank) const
{ //return ledmodule->getClashColor(bank);
  return setmodule->getClashColor(bank);
}
void CoreComms::setMainColor(int bank, String colorString)
{ setMainColor(bank, stringToColorLed(colorString));
}
void CoreComms::setMainColor(int bank, const ColorLed& ledColor)
{ //call settings module
  //ledmodule->setMainColor(bank, ledColor);
  setmodule->setMainColor(bank, ledColor);
}
void CoreComms::setClashColor(int bank, String colorString)
{ setClashColor(bank, stringToColorLed(colorString));
}
void CoreComms::setClashColor(int bank, const ColorLed& ledColor)
{ //call settings module
  //ledmodule->setClashColor(bank, ledColor);
  setmodule->setClashColor(bank, ledColor);
}
ColorLed CoreComms::stringToColorLed(String sColor)
{ ColorLed cc = {0,0,0,0};
  String strip=sColor.trim();

  if((strip.indexOf(",")<0) && (strip.indexOf(";")<0) && (strip.indexOf(" ")<0)) 
  { //no punctuation means hexadecimal
    //hex number - remove any hexadecimal prefixes
    if(strip.startsWith("#"))
    { strip = strip.substring(1);
    } else if (strip.startsWith("0x"))
    { strip = strip.substring(2);
    }
    u_int32_t rgbw= 0;
    for(unsigned int i=0; i<strip.length(); i++)
    { char c = strip.charAt(i);
      rgbw=(rgbw*16);
      if(c <= '9')
      { rgbw+= (c-'0');
      } else if (c<='F')
      { rgbw+= 10 + c - 'A';
      } else if (c<='f')
      { rgbw+= 10 + c - 'a';
      }
    }
    cc.red = (rgbw>>24) & 255;
    cc.green = (rgbw>>16) & 255;
    cc.blue = (rgbw>>8) & 255;
    cc.white = rgbw & 255;
  } else
  { //csv list "r,g,b,w"
    char sep = ',';
    int pos = strip.indexOf(sep);
    if(pos<0)
    { sep=';';
      pos = strip.indexOf(sep);
    }
    if(pos<0)
    { sep=' ';
      pos = strip.indexOf(sep);
    }
    if(pos>0)
    { //logger.writeLine("strtoColorLed sep="+sep);
      cc.red= strip.substring(0,pos).toInt();
      strip=strip.substring(pos+1);
      
      pos = strip.indexOf(sep);
      if(pos>0)
      { cc.green= strip.substring(0,pos).toInt();
        strip=strip.substring(pos+1);
        
        pos = strip.indexOf(sep);
        if(pos>0)
        { cc.blue= strip.substring(0,pos).toInt();
          strip=strip.substring(pos+1);
        
          cc.white= strip.toInt();
        }
      }
    }
  }
  return cc;
}

char CoreComms::hexDigit(int n)
{ //fast binary int to ascii hex
  return n + ((n<10) ? 48 : 55); //use 87 for lowercase a, 55 for uppercase A
}

String CoreComms::colorLedToString(ColorLed cColor, boolean bHex)
{ if(bHex)
  { String hx="";
    hx=hx+hexDigit( (cColor.red & 0xF0)>>4 ) + hexDigit(cColor.red & 0x0f)
      +hexDigit( (cColor.green & 0xF0)>>4 ) + hexDigit(cColor.green & 0x0f)
      +hexDigit( (cColor.blue & 0xF0)>>4 ) + hexDigit(cColor.blue & 0x0f)
      +hexDigit( (cColor.white & 0xF0)>>4 ) + hexDigit(cColor.white & 0x0f);
    return hx;
  } else
  { return String(cColor.red)+","+ String(cColor.green)+","
         + String(cColor.blue)+","+ String(cColor.white);
  }
}

byte CoreComms::getMode()
{ return commsMode;
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
        //List all files to PC
        char filename[64];
        uint32_t filesize;

        if (SerialFlash.readdir(filename, sizeof(filename), filesize))
        {
          Serial.print(filename);
          Serial.print(" ");
          spaces(20 - strlen(filename));
          Serial.print(" (");
          Serial.print(filesize);
          Serial.print(")\n");
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

void CoreComms::spaces(int num)
{
  for (int i = 0; i < num; i++)
  {
    Serial.print(" ");
  }
}
