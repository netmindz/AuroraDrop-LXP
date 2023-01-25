# AuroraDrop LXP v1.0.0

RGB Matrix Audio Visualiser for ESP32

Forked from AuaoraDrop by uklooney, based on the FastLED Auroa Demo by Jason Coon, and using the ESP32-HUB75-MatrixPanel-I2S-DMA library by mrfaptastic. Audio reactive and I2S code from the WLED usermod "AudioReactive" - and some love from MoonModules WLED fork as well. 

## Introduction

AuroraDrop LXP currently works *only* with HUB75 matrix panels and an INMP441 microphone. The goal of this fork is to make a functional audio visualization display with zero interaction on the end-user's part. *This means no web server, no network audio, no TFT displays, no LED strips, etc.*

Major goals of this fork are:

* Using the INMP441 microphone for audio input (other I2S inputs will often work with minor modifications)
* Automatic gain control
* Basic BPM detection
* Code cleanup to ease future modification
* Removal of all functions that aren't used with the specific target hardware
* ...and some sort of physical integration with the panel that isn't a jumble of wires.

I suggest using the original AuroraDrop project for details on wiring and if you have other needs past HUB75+INMP441 integrations.

## Hardware Recommendations

This code has been tested with up to **FOUR** 64x64 pixel HUB74 "E" panels in a horizontal layout on an ESP32-S3 with 2MB of PSRAM. Also works fine with 64x32 panels (tested with two, expect more will work). A few effects need some love for widths > 128 pixels, and some are disabled based on this logic.

At 256 pixels wide the framereate does sometimes suffer - but if you play with the playlist sizes and pick effects that are very fast, 30fps is attainable on 64x256 (four panels) - which is 16,384 LEDs!

The recommended ESP32 target for the project is specificially the ESP32-S3-DevKitM-1 board. 

https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/hw-reference/esp32s3/user-guide-devkitm-1.html

The board was selected because the HUB75 driver has optimizations for the ESP32-S3 which makes it faster, and the ESP32-S3-DevKitM1 (**not** DevKitC1) board has a VERY advantageous pin arrangement that can be used to directly attach to the HUB75 connector (with some pin header bending!). It doesn't have PSRAM, but that doesn't seem to be as needed at the moment.

Larger matrix widths (over 128 wide) have been tested with a non-Espressif 3rd-parth ESP32-S3-DevKitC1 "type" board - the pinout is better than the Espressif devkit-C1 so we can get 16 pins in an even 2x8 continuous spacing. 

At the moment I'm not sure if the PSRAM is helping large widths - or it's other fixes I've done. Either way, the PSRAM is indeed in use in the HUB75 panel driver if you enable it and your board supports it.

### Audio Input - INMP441 I2S ###

The audio input is accomplished with the INMP441 microphone. Commonly available circular breakout boards have the needed resistor and capacitor on the breakout.

**If you happen to wire this up on a breadboard, make sure to run a ground between the panel (or the panel PSU) and the ESP32 if you don't have one already.**

The panels will (mostly) work fine without a ground, but the I2S mic will become hilariously unstable and lead you down a rabbit hole for a week trying to figure out why the mic never seems like it's capturing real audio data. 

I've had 100 issues with I2S in general - there's code in there to try and work around bugs in particular ESP IDF versions - but sometimes the "L/R" pin just needs to be moved from GND to VCC or vice-versa to make it work, even with the compile-time fixes.

Ideally the "L/R" (or "LR") pin should be to ground, but I've had times where it's to VCC even with the in-line fixes, on different boards with the same ESP32-S3 chip and the same ESP IDF. I have no clue why. It does seem to be the board/chip and not the mic.

## Latest Updates

1.0.0 (WIP)
* Imported much more robust audio reactive code from the WLED AudioReactive usermod
  * ...with some nice finishing touches from MoonModules' version of the same usermod ( I :heart: [MoonModules](https://github.com/MoonModules/WLED) )
* Code cleanup (lots)
** I've expanded a lot of the more dense syntax to make the code easier to understand. 
* Better scaling of all the FFT bins based on AudioReactive code  
* Automatic gain control (now courtesy of AudioReactive)
* I2S code modernizarion for new ESP32 APIs (now courtesy of AudioReactive hints)
* Basic BPM detection
* Removal of other hardware targets
* Deactivation or optimization of some very CPU intensive effects
* RGB LED on ESP32-S3 boards to do basic frames-per-second monitoring (now via FastLED)
** You could also add a single WS2811 or similar if your board doesn't have one
* Removal of webserver and networking code
* Added a generic "NOOP" effect so we can do nothing, intentionally, in a playlist.
* Optional pot for global brightness control (use a 10k pot)
* Optional use of a button (like "boot") to control debug display
* Fixes and/or disable logic for some effects when MATRIX_WIDTH > 128 pixels
* A few new effects and some zjuzhing and bugfixes on existing ones
* Organization of background, foreground, sound, and static patterns into their own specific layers

### Libraries

 * FastLED
    * https://github.com/FastLED/FastLED

 * ESP32 HUB75 LED MATRIX PANEL DMA Display
    * https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA

 * Adafruit GFX Library
    * https://github.com/adafruit/Adafruit-GFX-Library

 * ArduinoFFT - "develop" branch
    * https://github.com/kosme/arduinoFFT/tree/develop
    * You'll need to download this as a zip or check it out directly with git.

### Based on

 Lots, including...

 * AuroraDrop, obviously: https://github.com/uklooney/AuroraDrop/

 * WLED usermod "AudioReactive": https://github.com/MoonModules/WLED/tree/mdev/usermods/audioreactive
 
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

### Why "LXP" ???

This started off as just grabbing some different LED panels off Amazon to see what they could do.

Once I discovered AuroraDrop I decided to take this on as a Christmas present. I discuss lots of my ongoing projects with the intended recipient, so I just told them it was my secret project "LXP"... 

...which stood for "Leigh's Xmas Present"
