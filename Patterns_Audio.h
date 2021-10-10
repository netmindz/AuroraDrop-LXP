#ifndef Patterns_Audio_H
#define Patterns_Audio_H

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include "PatternTest.h"

#include "PatternAudio_A_RotatingWave.h"
#include "PatternAudio_B_CircularWave.h"
#include "PatternAudio_C_DotsSingle.h"
#include "PatternAudio_D_RotatingSpectrum.h"

#include "PatternAudio_N_SpectrumPeakBars.h"
#include "PatternAudio_O_Spectrum2.h"

#include "PatternAudio_XR_Torus.h"
#include "PatternAudio_XS_8x8Squares.h"
#include "PatternAudio_XT_BigSpark.h"
#include "PatternAudio_XX_Aurora.h"




// test patterns to integrate
#include "PatternAudio_Z_Lines.h"
#include "PatternAudio_Z_Circles.h"
#include "PatternAudio_Z_Triangles.h"
#include "PatternAudio_Z_WaveSingle.h"
#include "PatternAudio_P_DiagonalSpectrum.h"
#include "PatternAudio_Z_SpectrumCircle.h"


#include "PatternTestCanvas.h"
#include "PatternTestSpectrum.h"



class Patterns_Audio : public Playlist {
  private:
    PatternTest patternTest;
    PatternTestSpectrum patternTestSpectrum;
    PatternAudio8x8Squares audio8x8Squares;
    PatternAudioBigSpark audioBigSpark;
    PatternAudioTorus audioTorus;
    PatternAudioRotatingSpectrum audioRotatingSpectrum;
    PatternAudioRotatingWave audioRotatingWave;
    PatternAudioCircularWave audioCircularWave;

    // test
    PatternAudioLines audioLines;
    PatternAudioCircles audioCircles;
    PatternAudioTriangles audioTriangles;
    PatternTestSpectrum testSpectrum;
    PatternAudioSpectrumPeakBars audioSpectrumPeakBars;
    PatternAudioSpectrum2 audioSpectrum2;
    PatternAudioWaveSingle audioWaveSingle;
    PatternAudioDotsSingle audioDotsSingle;
    PatternAudioDiagonalSpectrum audioDiagonalSpectrum;
    PatternAudioSpectrumCircle audioSpectrumCircle;
    PatternAudioAurora audioAurora;
    PatternCanvasTest canvasTest;

    int currentIndex = 0;
    Drawable* currentItem;

    int getCurrentIndex() {
      return currentIndex;
    }

    const static int PATTERN_COUNT = 12;

    Drawable* shuffledItems[PATTERN_COUNT];

    Drawable* items[PATTERN_COUNT] = {
      //&patternTest,
      //&patternTestSpectrum,
      
      &audioRotatingWave,
      &audioCircularWave,
      &audioDotsSingle,                   // dancing/rolling dots
      &audioRotatingSpectrum,

      &audio8x8Squares,
      &audioBigSpark,
      &audioTorus,

      // working initial proof of concept
      &audioSpectrumPeakBars,                    // 16 lines
      &audioSpectrum2,      // to sort

      &audioDiagonalSpectrum,   // to tidy
      &audioTriangles,                    // triangles
      &audioSpectrumCircle,     // spectrum lines from centre out, rotating around 360 degrees



      //&audioAurora,

      //&canvasTest,
      //&audioCircles,      // messing
      //&audioWaveSingle,   // doing
      //&audioLines,
      //&testCircles,
      //&testSpectrum,

    };

  public:
    Patterns_Audio() {
      // add the items to the shuffledItems array
      for (int a = 0; a < PATTERN_COUNT; a++) {
        shuffledItems[a] = items[a];
      }

      shuffleItems();

      this->currentItem = items[0];

      for (int i=0; i < maxPatternAudio; i++) 
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

    uint8_t getCurrentPatternId2()
    {
      return currentItem->id2;      
    }

    char * getCurrentPatternId()
    {
      return currentItem->id;      
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
