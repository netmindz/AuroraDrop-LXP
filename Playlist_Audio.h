#ifndef Platlist_Audio_H
#define Platlist_Audio_H

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include "PatternTest.h"

#include "PatternAudio_A_RotatingWave.h"
#include "PatternAudio_B_CircularWave.h"
#include "PatternAudio_C_DotsSingle.h"
#include "PatternAudio_D_RotatingSpectrum.h"
#include "PatternAudio_E_ClassicSpectrum128.h"
#include "PatternAudio_F_Cubes.h"

#include "PatternAudio_N_SpectrumPeakBars.h"
#include "PatternAudio_O_Spectrum2.h"

#include "PatternAudio_R_AuroraDrop.h"

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



class Playlist_Audio : public Playlist {
  private:

    PatternAudioRotatingWave audioRotatingWave;
    PatternAudioCircularWave audioCircularWave;
    PatternAudioDotsSingle audioDotsSingle;
    PatternAudioRotatingSpectrum audioRotatingSpectrum;
    PatternAudioClassicSpectrum128 audioClassicSpectrum128;
    //PatternAudioCubes audioCubes;

    PatternAudio8x8Squares audio8x8Squares;
    PatternAudioBigSpark audioBigSpark;
    PatternAudioTorus audioTorus;

    // test
    PatternAudioSpectrumPeakBars audioSpectrumPeakBars;
    PatternAudioSpectrum2 audioSpectrum2;
    PatternAudioDiagonalSpectrum audioDiagonalSpectrum;
    PatternAudioTriangles audioTriangles;
    PatternAudioSpectrumCircle audioSpectrumCircle;

    PatternAudioAurora audioAurora;
    PatternAudioWaveSingle audioWaveSingle;
    PatternCanvasTest canvasTest;
    PatternAudioCircles audioCircles;
    PatternAudioLines audioLines;

    PatternTest patternTest;
    PatternTestSpectrum patternTestSpectrum;

    PatternAudioAuroraDrop patternAuroraDrop;

    int currentIndex = 0;
    Drawable* currentItem;

    int getCurrentIndex() {
      return currentIndex;
    }

    const static int PATTERN_COUNT = 16;

    Drawable* shuffledItems[PATTERN_COUNT];

    Drawable* items[PATTERN_COUNT] = {

      
      &audioRotatingWave,
      &audioCircularWave,
      &audioDotsSingle,                   // dancing/rolling dots
      &audioRotatingSpectrum,
      &audioClassicSpectrum128,
      //&audioCubes,

      &audio8x8Squares,
      &audioBigSpark,
      &audioTorus,

      // working initial proof of concept
      &audioSpectrumPeakBars,                    // 16 lines
      &audioSpectrum2,      // to sort
      &audioDiagonalSpectrum,   // to tidy
      &audioTriangles,                    // triangles
      &audioSpectrumCircle,     // spectrum lines from centre out, rotating around 360 degrees
      &patternAuroraDrop,

      &patternTest,
      &patternTestSpectrum,

      //&audioAurora,
      //&audioWaveSingle,   // doing
      //&canvasTest,
      //&audioCircles,      // messing
      //&audioLines,
      //&testCircles,
      //&testSpectrum,

    };

  public:
    Playlist_Audio() {
      // add the items to the shuffledItems array
      for (int a = 0; a < PATTERN_COUNT; a++) {
        shuffledItems[a] = items[a];
      }
      shuffleItems();
      this->currentItem = items[0];
      for (int i=0; i < maxPlaylistsAudio; i++) 
      {
      this->currentItem->start(i); 
      }
    }

    //char* Drawable::name = (char *)"Patterns";

    // Auroradrop: 
    // Auroradrop: 
    char* getItemName(int _id) {
      return items[_id]->name;      
    }

    int getPatternCount() {
      return PATTERN_COUNT;
    }

    bool getItemEnabled(int _id) {
      return items[_id]->isEnabled();      
    }

    void setItemEnabled(int _id, int value) {
        //items[_id]->enabled == (bool)value;
        items[_id]->setEnabled((bool)value);
        Serial.print("pattern id "); Serial.print(id); Serial.print(" changed to "); Serial.println(value);
    }

    bool getCurrentItemEnabled() {
      return this->currentItem->isEnabled();      
    }

    // Auroradrop: 
    // Auroradrop: 



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
