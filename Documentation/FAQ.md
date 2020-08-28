# FAQ and troubleshooting

- [I'm using PlatformIO inside VSCode and I'm getting the error "CoreEntryPoint.h: No such file or directory"](#)
- [I'm getting an error message after connection with Scintilla "An OpenCore Polaris was detected, but Scintilla failed to communicate with it"](#)


## I'm using PlatformIO inside VSCode and I'm getting the error *"CoreEntryPoint.h: No such file or directory"*
Open the *platformio.ini* file and change or add the line: *lib_extra_dirs = ~/Documents/GitHub/polaris-opencore/libraries*

## I'm getting an error message after connection with Scintilla *"An OpenCore Polaris was detected, but Scintilla failed to communicate with it"*
It could be normal for older firmware. Just go ahead and upload the latest FW (*Firmware Tools > Flash latest OpenCore firmware*)


