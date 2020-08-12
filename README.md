# LamaDiLuce - Anima EVO
[![Build Status](https://img.shields.io/github/forks/LamaDiLuce/polaris-opencore.svg)](https://github.com/LamaDiLuce/polaris-opencore)
[![Build Status](https://img.shields.io/github/stars/LamaDiLuce/polaris-opencore.svg)](https://github.com/LamaDiLuce/polaris-opencore)
[![License](https://img.shields.io/github/license/LamaDiLuce/polaris-opencore.svg)](https://github.com/LamaDiLuce/polaris-opencore)
[![GitHub issues](https://img.shields.io/github/issues/LamaDiLuce/polaris-opencore)](https://github.com/LamaDiLuce/polaris-opencore/issues)
[![Donate](https://img.shields.io/badge/Donate-PayPal-blue.svg)](https://www.paypal.me/ludosport/)

Polaris, the combat ready modular hilt born in 2015 becomes an open source project in 2020 powered with the new Anima EVO electronic module.

To read the entire story you can follow [this link](https://www.lamadiluce.it/polaris-evo/).

[![Polaris Anima EVO](https://www.lamadiluce.it/wp-content/uploads/2020/06/FBcover-01-e1596115323936.png)](https://www.lamadiluce.it/polaris-evo/)

---

## Table of contents
- [Introduction](#introduction)
- [Features](#features)
- [Setup](#setup)
- [Contributing](#contributing)
- [Code Team](#core-team)
- [FAQ](#faq)
- [Donations](#donations)
- [License](#license)

---

## Introduction
[LudoSport](https://www.ludosport.net/) masters created in 2006 a combat discipline with the lightsaber that spread a common language among the people practicing it, however the athletes didn’t have a reference for their equipment yet.

In 2015 [LamaDiLuce](https://www.lamadiluce.it/) gave birth to Polaris, a new saber that would become the reference for hundreds of lightsaber enthusiasts who wanted to compete with an elegant weapon.

After 5 years, and after thousands of feedbacks from worldwide users, the new generation of saber is ready and you can contribute to the evolution of Polaris too.

---

## Features

- RGBW costomization for LED
- No pushbutton required
  - Gestures to arm / disarm
  - Blade color can be selected with a gesture among pre-sets during arming
- Electronic powered by a Cortex-M4
- Dedicated Scintilla PC software for advanced customization and firmware upgrade

---

## Setup
In order to be able to contributre to this project, compile and upload the FW to your lightsaber, you need to:
- undestand C++
- be familiar with Arduino
- download and installa bunch of software

The following guides contain a step by step instructions to be up and running:


> [Windows (or MAC) guide step by step](Documentation/SETUP.md#windows-setup)


> [Linux guide step by step](Documentation/SETUP.md#linux-setup)

---

## Contributing




---

## Core Team




---

## FAQ

- **I'm using PlatformIO inside VSCode and I'm getting the error *"CoreEntryPoint.h: No such file or directory"***
    - Open the *platformio.ini* file and change or add the line: *lib_extra_dirs = ~/Documents/GitHub/polaris-opencore/libraries*
- **I'm getting an error message after connection with Scintilla *"An OpenCore Polaris was detected, but Scintilla failed to communicate with it."***
    - It could be normal for older firmware. Just go ahead and upload the latest FW (*Firmware Tools > Flash latest OpenCore firmware*)


---

## Donations

You can still support the project with [development suggestions](https://bit.ly/Polaris-OpenCore-suggestion) or with a [free donation](https://www.paypal.me/ludosport/) that will be used for new hardware evolutions.

[![Donate](https://img.shields.io/badge/Donate-PayPal-blue.svg)](https://www.paypal.me/ludosport/)

---

## License
[![License](https://img.shields.io/github/license/LamaDiLuce/polaris-opencore.svg)](https://github.com/LamaDiLuce/polaris-opencore)

---