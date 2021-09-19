/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef PatternsOLD_H
#define PatternsOLD_H

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


//#include "Vector.h"
//#include "Boid.h"
//#include "Attractor.h"



/* 
 *  Note from mrfaptastic:
 *  
 *  Commented out patterns are due to the fact they either didn't work properly with a non-square display,
 *  or from my personal opinion, are crap. 
 */

#include "PatternAudioLines.h"
#include "PatternAudioCircles.h"
//#include "PatternAudioShapes.h"

//#include "PatternTest.h"
#include "PatternTestSpectrum.h"
//#include "PatternSpiral.h"
//#include "PatternIncrementalDrift.h"
//#include "PatternFlock.h"
//#include "PatternFlowField.h"
//#include "PatternSpiro.h"
#include "PatternTestCircles.h"
//#include "PatternAttract.h"

/*
#include "PatternPendulumWave.h"
#include "PatternIncrementalDrift.h"
#include "PatternMunch.h"
#include "PatternElectricMandala.h"
#include "PatternSwirl.h"
#include "PatternSimplexNoise.h"
#include "PatternWave.h"
#include "PatternInfinity.h"
#include "PatternPlasma.h"
#include "PatternSnake.h"
#include "PatternInvaders.h"
#include "PatternLife.h"
#include "PatternMaze.h"
*/

//#include "PatternRadar.h"
//#include "PatternNoiseSmearing.h" // Doesn't seem to work, omitting.
//#include "PatternIncrementalDrift2.h" // Doesn't seem to work, omitting.
//#include "PatternSpin.h" // Doesn't seem to work, omitting.
//#include "PatternBounce.h" // Doesn't seem to work, omitting.
//#include "PatternCube.h" // Doesn't seem to work, omitting.
//#include "PatternFire.h" // Doesn't seem to work, omitting.
//#include "PatternPulse.h" // Doesn't seem to work, omitting.
//#include "PatternSpark.h" // Doesn't seem to work, omitting.

class PatternsOLD : public Playlist {
  private:
    //PatternAudioShapes audioShapes;
    PatternAudioLines audioLines;
    PatternAudioCircles audioCircles;

    //PatternTest patternTest;
    PatternTestSpectrum testSpectrum;
    //PatternIncrementalDrift incrementalDrift;
    //PatternSpiral spiral;
    //PatternFlock flock;
    //PatternFlowField flowField;
    //PatternSpiro spiro;
    PatternTestCircles testCircles;
    //PatternAttract attract;
/*
    PatternSwirl swirl;
    PatternPendulumWave pendulumWave;
    PatternFlowField flowField;
    PatternMunch munch;
    PatternElectricMandala electricMandala;
    PatternSimplexNoise simplexNoise;
    PatternWave wave;
    PatternFlock flock;
    PatternInfinity infinity;
    PatternPlasma plasma;
    PatternInvadersSmall invadersSmall;
    PatternSnake snake;
    PatternLife life;
    PatternMaze maze;
*/

 //   PatternRainbowFlag rainbowFlag; // doesn't work
 //   PatternPaletteSmear paletteSmear;
 //   PatternMultipleStream multipleStream;   // doesn't work
 //   PatternMultipleStream2 multipleStream2; // doesn't work
 //   PatternMultipleStream3 multipleStream3; // doesn't work
 //   PatternMultipleStream4 multipleStream4; // doesn't work
 //   PatternMultipleStream5 multipleStream5; // doesn't work
 //   PatternMultipleStream8 multipleStream8; // doesn't work

 //   PatternRadar radar;
 //   PatternIncrementalDrift2 incrementalDrift2;
 //   PatternSpin spin;
 //   PatternBounce bounce;
 //   PatternInvadersMedium invadersMedium;
 //   PatternInvadersLarge invadersLarge;
 //   PatternCube cube;
 //   PatternFire fire;
 //   PatternPulse pulse;
 //  PatternSpark spark;

    int currentIndex = 0;
    Drawable* currentItem;

    int getCurrentIndex() {
      return currentIndex;
    }

    //const static int PATTERN_COUNT = 37;   

    const static int PATTERN_COUNT = 1;       // always use static on esp32 !!!!

    Drawable* shuffledItems[PATTERN_COUNT];

    Drawable* items[PATTERN_COUNT] = {
      //&audioCircles,
      &audioLines,
      //&audioShapes,
      //&testCircles,
      //&incrementalDrift,
      //&spiral,
      //&testSpectrum,
      //&flowField,
      //&flock,
      //&spiro,
      //&attract, // 21 ok

/*      
      &spiro,           // cool
      &life, // ok
      &pendulumWave, //11 ok
      &munch, // 14 ok
      &electricMandala, // 15 ok
      &simplexNoise, // 17 - cool!
      &swirl, // 22
      &infinity, // works
      &plasma, // works
      &invadersSmall, // works ish
      &snake, // ok
      &maze, // ok
*/


   //   &patternTest,     // ok 
   //   &paletteSmear,  // fail
   //   &multipleStream, // fail
   //   &multipleStream8,// fail
   //   &multipleStream5,// fail
   //   &multipleStream3,// fail
   //   &radar, // fail
   //   &multipleStream4, // fail
   //   &multipleStream2, // fail
   //   &pulse,// fail
   //   &incrementalDrift2, // 13 fail
   //   &spin, // 16 ok but repeditivev
   //   &wave, // 18 ok (can't work with 256+ matrix due to uint8_t vars)
   //   &rainbowFlag, //20 // fail
   //   &bounce, // boncing line crap
   //   &invadersMedium, // fail
   //   &invadersLarge, // fail
   //   &cube, // works ish
   //   &fire, // ok ish
   //   &spark, // same as fire


    };

  public:
    PatternsOLD() {
      // add the items to the shuffledItems array
      for (int a = 0; a < PATTERN_COUNT; a++) {
        shuffledItems[a] = items[a];
      }

      shuffleItems();

      this->currentItem = items[0];

      for (int i=0; i < MAX_PATTERNS_OLD; i++) 
      {
      this->currentItem->start(i); 
      }



    }

    //char* Drawable::name = (char *)"Patterns";

    void stop() {
      if (currentItem)
        currentItem->stop();
    }

    void start(uint8_t _pattern) {
      if (currentItem)
        currentItem->start(_pattern);
    }

    void move(int step, uint8_t _pattern) {
      currentIndex += step;

      if (currentIndex >= PATTERN_COUNT) currentIndex = 0;
      else if (currentIndex < 0) currentIndex = PATTERN_COUNT - 1;

      if (effects.paletteIndex == effects.RandomPaletteIndex)
        effects.RandomPalette();

      moveTo(currentIndex, _pattern);

      //if (!isTimeAvailable && currentItem == &analogClock)
     //   move(step);
    }

    void moveRandom(int step, uint8_t _pattern) {
      currentIndex += step;

      if (currentIndex >= PATTERN_COUNT) currentIndex = 0;
      else if (currentIndex < 0) currentIndex = PATTERN_COUNT - 1;

      if (effects.paletteIndex == effects.RandomPaletteIndex)
        effects.RandomPalette();

      if (currentItem)
        currentItem->stop();

      currentItem = shuffledItems[currentIndex];

      if (currentItem)
        currentItem->start(_pattern);

     // if (!isTimeAvailable && currentItem == &analogClock)
     //   moveRandom(step);
    }

    void shuffleItems() {

      //return;  // TEMP

      for (int a = 0; a < PATTERN_COUNT; a++)
      {
        int r = random(a, PATTERN_COUNT);
        Drawable* temp = shuffledItems[a];
        shuffledItems[a] = shuffledItems[r];
        shuffledItems[r] = temp;
      }
    }


    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
      return currentItem->drawFrame(_pattern, _total);
    }

    void listPatterns() {
      Serial.println(F("{"));
      Serial.print(F("  \"count\": "));
      Serial.print(PATTERN_COUNT);
      Serial.println(",");
      Serial.println(F("  \"results\": ["));

      for (int i = 0; i < PATTERN_COUNT; i++) {
        Serial.print(F("    \""));
        Serial.print(i, DEC);
        Serial.print(F(": "));
        Serial.print(items[i]->name);
        if (i == PATTERN_COUNT - 1)
          Serial.println(F("\""));
        else
          Serial.println(F("\","));
      }

      Serial.println("  ]");
      Serial.println("}");
    }

    char * getCurrentPatternName()
    {
      return currentItem->name;      
    }

    void moveTo(int index, uint8_t _pattern) {
      if (currentItem)
        currentItem->stop();

      currentIndex = index;

      currentItem = items[currentIndex];

      if (currentItem)
        currentItem->start(_pattern);
    }    

    bool setPattern(String name, uint8_t _pattern) {
      for (int i = 0; i < PATTERN_COUNT; i++) {
        if (name.compareTo(items[i]->name) == 0) {
          moveTo(i, _pattern);
          return true;
        }
      }

      return false;
    }


    bool setPattern(int index, uint8_t _pattern) {
      if (index >= PATTERN_COUNT || index < 0)
        return false;

      moveTo(index, _pattern);

      return true;
    }
};

#endif
