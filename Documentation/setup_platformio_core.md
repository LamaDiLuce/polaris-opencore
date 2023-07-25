# How to use PlatformIO core for polaris

This method is bloat-free (does not require using VSCode in particular), but explains how to build the firmware from source and upload it using platformIO, ths easiest way for non-Windows users form the command prompt.
For instructions on Windows, refer to the [PlatformIO IDE setup](https://github.com/LamaDiLuce/polaris-opencore/wiki/VSCode-PlatformIO)

## Install PlatformIO core
A detailed guide on how to setup is described [on the official website](https://platformio.org/install/cli).

### With pip
```bash
pip install -U platformio
```
### Debian based
```sh
apt install platformio
```

### Arch based
The `platformio` package is available on the AUR. It is possible to get it with the AUR helper of your choice, e.g with yay
```sh
yay -Sy platformio
```

### With homebrew
```sh
brew install platformio
```
## Project setup
Now, make sure to clone the repository and hook the ANIMA EVO.
```sh
git clone https://github.com/LamaDiLuce/polaris-opencore
cd polaris-opencore
```
### Build and upload
The build command `platformio run` has to be executed under the *project root* to make sure `platformio` reads the configuration file.
The following command allows building the whole project and upload it to the board (thanks the the `--target` option). It will open a *Teensy Loader* instance to tell you the upload status
```sh
platformio run --target upload
```
