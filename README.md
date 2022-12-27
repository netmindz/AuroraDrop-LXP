# AuroraDrop LXP v1.0.0

THIS HAS JUST BEEN FORKED. CURRENTLY NO MODIFIED CODE HAS BEEN UPLOADED YET. (It's done, it's just not committed!)

RGB Matrix Audio Visualiser for ESP32

Forked from AuaoraDrop by uklooney, based on the FastLED Auroa Demo by Jason Coon, and using the ESP32-HUB75-MatrixPanel-I2S-DMA library by mrfaptastic.

## Introduction

AuroraDrop LXP currently works *only* with HUB75 matrix panels and an INMP441 microphone. The goal of this fork is to make a functional audio visualization display with zero interaction on the end-user's part. *This means no web server, no network audio, no TFT displays, no LED strips, etc.*

Major goals of this fork are:

* Using the INMP441 microphone for audio input (other I2S inputs may work with minor modifications)
* Automatic gain control
* Basic BPM detection
* Code cleanup for future modification
* Removal of all functions that aren't used with the specific target hardware
* ...and some sort of physical integration with the panel that isn't a jumble of wires.

I suggest using the original AuroraDrop project for details on wiring and if you have other needs past HUB75+INMP441 integrations.

## Hardware Recommendations

This code has been tested with up to two 64x64 pixel HUB74 "E" panels in horizontal mode. Also works fine with up to two 64x32 panels. More than two 64x64 panels is currently unstable, but two 64x64 panels sits nicely on a small shelf and looks impressive. 

The recommended ESP32 target for the project is specificially the ESP32-S3-DevKitM-1 board. 

https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/hw-reference/esp32s3/user-guide-devkitm-1.html

The board was selected because the HUB75 driver has optimizations for the ESP32-S3 which makes it faster, and the DevKitM1 (`not` C1) board has a VERY advantageous pin arrangement that can be used to directly attach to the HUB75 connector (with some pin header bending!)

The audio input is accomplished with the INMP441 microphone. Commonly available circular breakouts have the needed resistor and capacitor on the PCB.

## Latest Updates

1.0.0
* Code cleanup
* Automatic gain control
* I2S code modernizarion for new ESP32 APIs
* Basic BPM detection
* Removal of other hardware targets
* Deactivation of some very CPU intensive effects
* RGB LED on ESP32-S3-DevKitM-1 to do basic frames-per-second monitoring
* Removal of webserver and networking code

### Suggested Simple Hardware Solution
   
 * ESP32 I2S Matrix Shield (tested with v2.0.6)
 * https://github.com/witnessmenow/ESP32-i2s-Matrix-Shield
 * https://www.youtube.com/watch?v=ZiR93TmSyE0

### Libraries

 * FastLED
 * https://github.com/FastLED/FastLED

 * ESP32 HUB75 LED MATRIX PANEL DMA Display
 * https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA

 * Adafruit GFX Library
 * https://github.com/adafruit/Adafruit-GFX-Library

 * ArduinoFFT
 *  https://github.com/kosme/arduinoFFT

 * Adafruit NeoPixel
 * https://github.com/adafruit/Adafruit_NeoPixel
 * (This is ONLY for the ESP onboard RGB LED because I'm super lazy.)
 
### Based on

 Lots, including...

* AuroraDrop, obviously: https://github.com/uklooney/AuroraDrop/

 * Aurora: https://github.com/pixelmatix/aurora
   * Copyright (c) 2014 Jason Coon

 * ESP32-HUB75-MatrixPanel-I2S-DMA and Examples: https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA
   * Copyright (c) 2021 mrfaptastic
 
 * Portions of this code are adapted from "Flocking" in "The Nature of Code" by Daniel Shiffman: http://natureofcode.com/
   * Copyright (c) 2014 Daniel Shiffman
   * http://www.shiffman.net

 * Portions of this code are adapted from "Funky Clouds" by Stefan Petrick: https://gist.github.com/anonymous/876f908333cd95315c35
 * Portions of this code are adapted from "NoiseSmearing" by Stefan Petrick: https://gist.github.com/StefanPetrick/9ee2f677dbff64e3ba7a
   * Copyright (c) 2014 Stefan Petrick
   * http://www.stefan-petrick.de/wordpress_beta

 * Portions of this code are adapted from Noel Bundy's work: https://github.com/TwystNeko/Object3d
   * Copyright (c) 2014 Noel Bundy
 
 * Portions of this code are adapted from the Petty library: https://code.google.com/p/peggy/
   * Copyright (c) 2008 Windell H Oskay.  All right reserved.

 * Portions of this code are adapted from SmartMatrixSwirl by Mark Kriegsman: https://gist.github.com/kriegsman/5adca44e14ad025e6d3b
   * https://www.youtube.com/watch?v=bsGBT-50cts
   * Copyright (c) 2014 Mark Kriegsman

 * Portions of this code are adapted from LedEffects Plasma by Robert Atkins:
   * https://bitbucket.org/ratkins/ledeffects/src/26ed3c51912af6fac5f1304629c7b4ab7ac8ca4b/Plasma.cpp?at=default
   * Copyright (c) 2013 Robert Atkins

 * Portions of this code are adapted from Random Nerd Tutorials:
   * https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/
