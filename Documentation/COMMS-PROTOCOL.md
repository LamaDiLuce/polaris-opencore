# Polaris-opencore Serial Communication

## Specification v.1.5

### History
- v1.6   202106-14  Nuntis, added sound list commands
- v1.5   202012-16  Nuntis, 
  Converted to markdown & added to repository, moced all 'proposed' not implemented commands to end of document
- v1.4   2020-08-30  Nuntis,
  added LIST? to get a listing of files, ERASE to erase flash memory, changed r read to RD and WR for writing a file,  space management functions FREE?, SIZE?, USED?
  Split Table/Commands into Categories/Pages
- v1.3   2020-08-13  Nuntis, added W & L for swing power clash, and DEBUG mode
- v1.2   2020-07-03  Nuntis, added B?
- v1.1   2020-07-02  AnFive / Nuntis
- v1.0   2020-06-28  AnFive

### Background
The Arduino Serial communication allows transmitting characters. The communication is stateless.

### Requirements
Definition of the serial communication protocol implemented in firmware.
- The protocol should be future proof: new functionality added later should not require substantial changes to the protocol.
- The protocol should be designed to work well with the desktop application.
- The protocol should be device agnostic, how the firmware stores the  information is not defined by the protocol.

### Protocol definition
The firmware will periodically poll the serial communication during operation.

Upon receiving a serial communication it will begin a "configuration mode".

The configuration mode will be active until it is terminated with a specific serial command, or until the firmware detects a USB disconnection (by querying the charging status of the battery).

When the configuration session is terminated, the new settings are automatically written to non-volatile memory.

All lines/replies from the saber beginning with a # (hash) are comments/logs from the logging module when the saber has firmware in debug mode.

Commands that are not recognised do not solicit a response allowing for feature detection without direct knowledge of build number features.

---
## Firmware Query Commands

### Name: Legacy
**Command:** Any string starting with ```'#' (0x23)``` and ending with a line feed ```LF (0x0a, \n)```.

**Effect:**  Replies with a single character ```'#' (0x23)```

**Notes:**  This is the format used by the commands in the legacy Polaris/Anima.
This ensures that those commands are ignored and also communicates that this is an OpenCore saber.

Saber firmware uses STX character to do file transmission strings starting with a # ignore STX characters in order for Scintilla to detect OpenCore vs Legacy sabers.

Build 1.9.13+, Scintilla/Gilthoniel

### Name: Read Firmware Version
**Command:** ```V?\n```

**Effect:**  Replies with firmware version as defined in Arduino header file
```V=x.x.x.x\n```

**Notes:**   Extended reply to indicate exact nature of message

All replies indicate the nature/content of the reply incmoing.

Build 1.9.13+, Scintilla/Gilthoniel

### Name: Developer Report
**Command:** ```V\n```

**Effect:**  V on it's own returns a test report, containing serial numbers, build number and a list of files on the serial flash for use by developers.

May be used to detect early firmware that does not have any other communication protocols implemented.

Build 1.9.11+, Scintilla/Gilthoniel

### Name: Read Serial Number
**Command:** ```S?\n```

**Effect:**  Replies with serial number (128-bit UUID) followed by \n
```S=e1234567-00119016-50224e45\n```

**Notes:** Build 1.9.12+, Scintilla/Gilthoniel

---
## System Control Commands

### Name: Factory Reset
**Command:** RESET\n
**Notes:**  Clears the configuration file and restores it to default value.
**Notes:**   Build 1.9.12+, Gilthoniel

### Name: End Configuration Session
**Command:** SAVE\n
**Effect:**  Saves the data to non-volatile storage. Replies with
> OK SAVE\n

Does not exit configuration mode - this is handled by the firmware detecting the end of charging. Detecting the end of charging when in configuration mode also does a save.
**Notes:** Build 1.9.13+, Gilthoniel

##Main Colour Commands
### Name:    Write main (guard) colour to bank
**Command:** Cx=rrggbbww\n

Where x=0..7 bank number rrggbbww = hexadecimal colour
or
Cx=r,g,b,w
Where x=0..7 bank number ```r,g,b,w``` are decimal comma separated values for the colour
**Effect:**  Store the colour bank
Replies with
>  OK Cx=rrggbbww\n

**Notes:**   Build 1.9.12+,  Scintilla, Gilthoniel

### Name: Read Main colour from bank as hex
**Command:** ````Cx?\n````
         where ```x``` = 0...7 bank number

**Effect:**  Replies with the the color bank data
```Cx=rrggbbww\n```

where ```x``` is bank number and rrggbbww is hexadecimal colour

**Notes:** Build 1.9.12+, Scintilla

### Name: Read Main colour from bank as decimal  CSV
**Command:** ```cx?\n```
 where ```x``` = 0...7 bank number

**Effect:**  Replies with the the color bank data
```cx=r,g,b,w\n```

where ```x``` is bank number and ```r,g,b,w``` is the colour as a decimal CSV string

**Notes:** Build 1.9.12+, Gilthoniel

### Name:    Fetch all Main colours as hex
**Command:** ```C?\n```

**Notes:**  Replies with all bank main colours 0..7 as a series of lines
```
 C0=rrggbbww\n
 C1=rrggbbww\n
 C2=rrggbbww\n
 C3=rrggbbww\n
 C4=rrggbbww\n
 C5=rrggbbww\n
 C6=rrggbbww\n
 C7=rrggbbww\n
```
**Notes:**   Build 1.9.12+

### Name:    Fetch all Main colours as decimal CSV
**Command:** ```c?\n```
**Effect:**  Replies with all bank main colours 0..7 as a series of lines with colors specified as a decimal CSV string
```
 c0=r,g,b,w\n
 c1=r,g,b,w\n
 c2=r,g,b,w\n
 c3=r,g,b,w\n
 c4=r,g,b,w\n
 c5=r,g,b,w\n
 c6=r,g,b,w\n
 c7=r,g,b,w\n
```
**Notes:**   Build 1.9.12+
         Gilthoniel

---
## Flash/Clash Colour Commands

### Name: Write Flash (Clash) colour to bank
**Command:** ```Fx=rrggbbww\n```
 where ```x``` = 0...7 bank number ```rrggbbww = hexadecimal colour```
 or
 ```Fx=r,g,b,w```
 where ```x``` = 0..7 bank number ```r,g,b,w``` is decimal comma separated values for colour

**Effect:**  Stores flash color for that bank
         replies with
         OK Fx=rrggbbww\n

**Notes:** Build 1.9.12+, Scintilla, Gilthoniel

### Name: Read Flash colour from bank as hex
**Command:** ```Fx?\n```
 where ```x``` = 0...7 bank number

**Effect:**  Replies with the the colour bank data
 ```Fx=rrggbbww\n```
 where ```x``` is bank number and rrggbbww is hexadecimal colour

**Notes:** Build 1.9.12+, Scintilla

### Name: Read Flash colour from bank as decimal CSV
**Command:** ```fx?\n```
 where ```x``` = 0...7 bank number

**Effect:**  Replies with the the colour bank data
 ```Fx=r,g,b,w\n```
 where ```x``` is bank number and ```r,g,b,w``` is the colour as a decimal CSV string

**Notes:**  Build 1.9.12+, Gilthoniel

### Name: Fetch all flash (clash) colours as hex
**Command:** ```F?\n```
**Effect:**  Replies with all bank main colours 0..7 as a series of lines
```
 F0=rrggbbww\n
 F1=rrggbbww\n
 F2=rrggbbww\n
 F3=rrggbbww\n
 F4=rrggbbww\n
 F5=rrggbbww\n
 F6=rrggbbww\n
 F7=rrggbbww\n
```
**Notes:**   Build 1.9.12+

### Name:    Fetch all flash (clash) colours as decimal CSV
**Command:** ```f?\n```
**Effect:**  Replies with all bank main colours 0..7 as a series of lines with colors specified as a decimal CSV string
```
  f0=r,g,b,w\n
  f1=r,g,b,w\n
  f2=r,g,b,w\n
  f3=r,g,b,w\n
  f4=r,g,b,w\n
  f5=r,g,b,w\n
  f6=r,g,b,w\n
  f7=r,g,b,w\n
```
**Notes:**   Build 1.9.12+, Gilthoniel

---
##Swing Colour Commands

### Name:    Write Swing colour to bank
**Command:** ```Wx=rrggbbww\n```
where ```x``` = 0...7 bank number
and ```rrggbbww``` = hexadecimal colour
or
```Wx=r,g,b,w```
where ```x``` = 0..7 bank number
and ```r,g,b,w``` is decimal comma separated values for colour
**Effect:** Stores swingcolor for that bank
replies with
```
  OK Wx=rrggbbww\n
```
**Notes:**   Build 1.9.15+, Gilthoniel

### Name:    Read Swing colour from bank as hex
**Command:** Wx?\n
         where ```x``` = 0...7 bank number
**Notes:**  Replies with the the swing colour bank data
         Wx=rrggbbww\n
         where ```x``` is bank number and rrggbbww is hexadecimal colour
**Notes:**   Build 1.9.15+

### Name:    Read Swing colour from bank as decimal CSV
**Command:** ```wx?\n```
where ```x``` = 0...7 bank number
**Effect:** Replies with the swing colour bank data
```
  Wx=r,g,b,w\n
```
where ```x``` is bank number and ```r,g,b,w``` is the colour as a decimal CSV string
**Notes:**   Build 1.9.15+, Gilthoniel

### Name:    Fetch all swing  colours as hex
**Command:** W?\n
**Effect:**  Replies with all bank main colours 0..7 as a series of lines
```
  W0=rrggbbww\n
  W1=rrggbbww\n
  W2=rrggbbww\n
  W3=rrggbbww\n
  W4=rrggbbww\n
  W5=rrggbbww\n
  W6=rrggbbww\n
  W7=rrggbbww\n
```
**Notes:**   Build 1.9.15+, Gilthoniel

### Name:    Fetch all swing colours as decimal CSV
**Command:** ```w?\n```
**Effect:**  Replies with all bank main colours 0..7 as a series of lines with colors specified as a decimal CSV string
```
 w0=r,g,b,w\n
 w1=r,g,b,w\n
 w2=r,g,b,w\n
 w3=r,g,b,w\n
 w4=r,g,b,w\n
 w5=r,g,b,w\n
 w6=r,g,b,w\n
 w7=r,g,b,w\n
```
**Notes:**   Build 1.9.15+, Gilthoniel

---
##Bank and Colour Preview Commands

### Name:    Set active colour bank
**Command:** B=x\n
         where ```x``` is the selected bank 0...7
**Notes:**  Sets the active colour bank b
         Replies with
         OK B=x\n
**Notes:**   Build 1.9.12+
         Scintilla
         Gilthoniel

### Name:    Query Active Bank
**Command:** ```B?\n```
Request which bank is currently active
**Effect:** Replies with
```
  B=x\n
```
Where ```x``` is active bank 0..7
**Notes:** Build 1.9.12+, Scintilla, Gilthoniel

### Name:    Preview color
**Command:** ```P=rrggbbww\n``` or ```P=r,g,b,w\n```
where ```rrggbbww``` is hexdecimal colour value or ```r,g,b,w``` is decimal CSV colour.
or ```P=0\n``` to turn it off. (Note it will turn off automatically anyway.)
**Effect:** Specify a color to preview on the blade or turn the blade off.
Replies with
```
  OK P=rrggbbww\n
``` 
**Notes:** As the blade is recharging when connected the blink indicator will turn this preview colour off.
Disconnecting the power (USB cable) also turns off the preview colour.
Build 1.9.15+, Gilthoniel

---
##Sound Lists -- change filenames for sounds

### Name: List Assigned On/Activation Sound Filenames
**Comamnd:** sON?\n
**Effect:**  Lists (as a CSV string) the files assigned as on/activation sounds
Replies with
```
  sON=POWERON_1_0.RAW,POWERON_2_0.RAW
```

### Name: set On/Activation Sound Filenames
**Comamnd:** sON= ...CSV list of filenames... \n
**Effect:**  Sets (from a CSV string) the files assigned as power on/activation sounds, maximum 25
Replies with
```
  OK sON=POWERON_1_0.RAW,POWERON_2_0.RAW
```

### Name: List Assigned Off/Deactivate Sound Filenames
**Comamnd:** sOFF?\n
**Effect:**  Lists (as a CSV string) the files assigned as off/deactivate sounds
Replies with
```
  sOFF=POWEROFF_1_0.RAW,POWEROFF_2_0.RAW
```

### Name: set Off/Deactivate Sound Filenames
**Comamnd:** sOFF= ...CSV list of filenames... \n
**Effect:**  Sets (from a CSV string) the files assigned as power off/deactivate sounds, maximum 25
Replies with
```
  OK sOFF=POWEROFF_1_0.RAW,POWEROFF_2_0.RAW
```


### Name: List Assigned Hum Sound Filenames
**Comamnd:** sHUM?\n
**Effect:**  Lists (as a CSV string) the files assigned as hum sounds
Replies with
```
  sHUM=HUM_0.RAW,HUM_1.RAW
```

### Name: set Hum Sound Filenames
**Comamnd:** sHUM= ...CSV list of filenames... \n
**Effect:**  Sets (from a CSV string) the files assigned as power hum sounds, maximum 25
Replies with
```
  OK sHUM=POWEROFF_1_0.RAW,POWEROFF_2_0.RAW
```

### Name: List Assigned Swing Sound Filenames
**Comamnd:** sSW?\n
**Effect:**  Lists (as a CSV string) the files assigned as swing sounds
Replies with
```
  sSW=SWING_1_0.RAW,SWING_2_0.RAW,SWING_3_0.RAW
```

### Name: set Swing Sound Filenames
**Comamnd:** sSW= ...CSV list of filenames... \n
**Effect:**  Sets (from a CSV string) the files assigned as swing sounds, maximum 25
Replies with
```
  OK sSW= SWING_1_0.RAW,SWING_2_0.RAW,SWING_3_0.RAW
```

### Name: List Assigned Clash Sound Filenames
**Comamnd:** sCL?\n
**Effect:**  Lists (as a CSV string) the files assigned as clash sounds
Replies with
```
  sCL=CLASH_1_0.RAW,CLASH_2_0.RAW,CLASH_3_0.RAW
```

### Name: set Clash Sound Filenames
**Comamnd:** sCL= ...CSV list of filenames... \n
**Effect:**  Sets (from a CSV string) the files assigned as clash sounds, maximum 25
Replies with
```
  OK sCL=CLASH_1_0.RAW,CLASH_2_0.RAW,CLASH_3_0.RAW
```

---
##Storage Management Commands

### Name:    List files in flash memory
**Command:** LIST?\n
**Effect:**  Lists files in the flash memory in this format:
```
  Files on memory:
  CLASH_10_0.RAW        88656
  CLASH_1_0.RAW         88200
  CLASH_2_0.RAW         88200
  CLASH_3_0.RAW         88200
  CLASH_4_0.RAW         88200
  CLASH_5_0.RAW         88200
  CLASH_6_0.RAW         88200
  CLASH_7_0.RAW         88200
  CLASH_8_0.RAW         88200
  CLASH_9_0.RAW         88200
  HUM_0.RAW             82592
  POWEROFF_0.RAW        88200
  POWERON_0.RAW         88200
  SWING_1_0.RAW         88200
  SWING_2_0.RAW         88200
  SWING_3_0.RAW         88200
  SWING_4_0.RAW         88200
  SWING_5_0.RAW         88200
  SWING_6_0.RAW         88200
  SWING_7_0.RAW         88200
  SWING_8_0.RAW         88200
  config.ini            65536

  Serial Flash Chip
  JEDEC ID: 1 60 18
  Memory Size: 16777216 bytes
  Memory Free: 14745600 bytes
  Memory Used: 2031616 bytes
  Block Size: 65536 bytes
```
**Notes:**   --

### Name:    Report Serial Flash (Storage) used space
**Command:** ```USED?\n```
**Notes:**  Returns number of bytes in serial flash used for all files, including deleted blocks that are no longer available.
Replies with
```
  USED=number-of-bytes\n
```
**Notes:**   --

### Name:    Report Serial Flash Storage space free / available
**Command:** ```FREE?\n```
**Notes:**  Returns number of bytes in serial flash free for writing new files.
Replies with
```
  FREE=number-of-bytes\n
```
**Notes:**  To allow the user to see if they have space to delete and  replace a single file instead of wiping and re-loading all the files.

### Name:    Report Serial Flash Storage space size
**Command:** ```SIZE?\n```
**Effect:** Returns the size of the Serial Flash storage in number of bytes.
Replies with
```
 SIZE=number-of-bytes\n
```
**Notes:**   --

### Name:    Erase/Format all the flash memory
**Command:** ```ERASE=ALL```
**Effect:** Erases the flash memory ready to upload new files
Replies with
```
 Erasing Serial Flash, this may take 20s to 2 minutes\n
```
then during the erase it replies with a stream of ```#``` and ```LF``` characters (every couple of seconds) to show the process has not hung, as this can take between 30s-2 minutes.

Completion replies with two lines
```
 OK, Now re-load your sound files.\n
 OK, Serial Flash Erased.\n
```
**Notes:**   sendtosaber.exe -erase-all

---
##File Read Commands

### Name:    Read file (text or binary)
**Command:** ```RD?config.ini\n``` or ```RD?<filename>\n```
**Notes:**  For ASCII files (any file that does not end in RAW) replies with STX followed by the contents of the file, followed by EOF or replies with
 ```ERROR, <filename> NOT FOUND```
 
 For binary files (any ending with RAW) just sends the binary bytes, use timeout to detect end of file -- though downloading binary files is not a normal function.

---
##File Write Commands

### Name:    Detect Write is Read / implemented / available
**Command:** ```WR?\n```
**Efect:**  Replies
 ```OK, Write Ready\n```
indicating saber can write to memory - enabling feature detection and that memory is ready
**Notes:** sendtosaber.exe to verify firmware has capability and is ready.

### Name:    Write file (text or binary)
**Command:** ```WR=filename,size\n```
followed by binary filestream with exactly ‘size’ number of bytes.

**Effect:**  Replies with
 ```OK, Write filename, size\n```
 immediately after the initial command before receiving the byte stream, if the serial byte stream is interrupted for more than 10s the file write will abort and timeout -- an incomplete (coreupted) file will of been written to the flash.
 
 Replies with
  ```OK, Write Complete\n```
 upon completion
         
 Possible errors
```
 ERROR, Write Invalid format\n
 ERROR, Write Invalid filename\n
 ERROR, Write Invalid file length\n
 ERROR, Serial Flash Not Ready\n
 ERROR, Unable to create file (1)\n
 ERROR, Unable to create file (2)\n
 ERROR, Write Timed-out\n
```
As standard files cannot be overwritten, but an existing  file is marked as deleted and that space is not regained unless a ERASE=ALL is sent - the replacement file uses new space on the Serial Flash.

Except the config.ini file which is marked as erasable and as long as it remains under 64Kbytes in size it will re-use the same space on the serial flash. (If size becomes an issue minify the JSON file before sending it.)

**Notes:**   sendtosaber.exe/sendtosaber <filename>

---
##Proposed/Suggested Functionality for future.

### Name: Set Volumew / Turn off sound
**Command:** ```VOL=x\n```
 where x=0..255
 or
 ```VOLb=x```
 Could be a setting for each bank
 where
 b=0...7 -- bank number
 x=0..255 - volume
**Effect:**  Allowing for a bank to be set to a specific volume or zero=silent

**Notes:** Also ```VOL?```
 or
 ```VOLb?```
 Replies with only
 ```VOL=0\n```
 or
 ```VOL=255\n```

 Until actual volume control can or is implemented 0 = off all other values mean on and is set to 255
 This could be a setting for each bank
 Per Daniele’s original request, now silent 'tap' mode obsoletes
 this primary functionality.

### Name:    Read battery
**Command:** ```BAT?\n```
**Effect:**  Replies with the battery information (format TBD)

Battery will always be On  charge because it’s plugged in, so the only replies could be CHARGING or FULLY CHARGED.

For forward compatibility with hardware that might have  battery level knowledge reply with a %age
```
 B=50\n
 B=100\n
```
**Notes:**   --

### Name: Turn Debug/Logging mode on or off
**Command:** DEBUG=1\n -- turn logging mode on
DEBUG=0\n -- turn logging mode off
**Effect:** Replies with
> OK, DEBUG=1\n
  OK, DEBUG=0\n

**Notes:**   This should be saved in the JSON file so that a reboot/power off-on keeps logging for debug purposes.
**NOT YET IMPLEMENTED**

---
## Power Clash Colour Commands
- Saber does not Currently have any Power Clash
- **Not Yet Implemented**

### Name: Write Power Clash colour to bank
**Command:** ```Lx=rrggbbww\n```
where ```x``` = 0...7 bank number
and ```rrggbbww``` = hexadecimal colour
or
```Lx=r,g,b,w```
where ```x``` = 0..7 bank number
and ```r,g,b,w``` is decimal comma separated values for colour

**Effect:**  Stores Power Clash color for that bank
replies with
```
  OK Lx=rrggbbww\n
```
**Notes:**   **not yet implemented**

### Name:    Read Power Clash colour from bank as hex
**Command:** ```Lx?\n```
where ```x``` = 0...7 bank number
**Effect:** Replies with the the colour bank data
```
  Lx=rrggbbww\n
```
where ```x``` is bank number and ```rrggbbww``` is hexadecimal colour

**Notes:**   **not yet implemented**

### Name:    Read Power Clash colour from bank as decimal CSV
**Command:** ```lx?\n``` (lower-case L ASCII 0x6c)
where ```x``` = 0...7 bank number

**Effect:** Replies with the the colour bank data
```
  lx=r,g,b,w\n
```
where ```x``` is bank number and ```r,g,b,w``` is the colour as a decimal CSV string

**Notes:**   **not yet implemented**

### Name:    Fetch all Power Clash colours as hex
**Command:**  ```L?\n```

**Effect:** Replies with all bank main colours 0..7 as a series of lines.
```
  L0=rrggbbww\n
  L1=rrggbbww\n
  L2=rrggbbww\n
  L3=rrggbbww\n
  L4=rrggbbww\n
  L5=rrggbbww\n
  L6=rrggbbww\n
  L7=rrggbbww\n
  ```
**Notes:**   **not yet implemented**

### Name:    Fetch all Power Clash colours as decimal CSV
**Command:**  ```l?\n``` (lower-case L ASCII 0x6c)

**Efect:** Replies with all bank main colours 0..7 as a series of lines with colors specified as a decimal CSV string.
```
  l0=r,g,b,w\n
  l1=r,g,b,w\n
  l2=r,g,b,w\n
  l3=r,g,b,w\n
  l4=r,g,b,w\n
  l5=r,g,b,w\n
  l6=r,g,b,w\n
  l7=r,g,b,w\n
```
**Notes:**   **not yet implemented**
