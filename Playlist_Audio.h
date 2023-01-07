#ifndef Playlist_Audio_H
#define Playlist_Audio_H

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include "PatternsOther\PatternTest.h"

#include "PatternsAudio\PatternAudio_A_RotatingWave.h"
#include "PatternsAudio\PatternAudio_B_CircularWave.h"
#include "PatternsAudio\PatternAudio_C_DotsSingle.h"
#include "PatternsAudio\PatternAudio_D_RotatingSpectrum.h"
#include "PatternsAudio\PatternAudio_E_ClassicSpectrum128.h"
#include "PatternsAudio\PatternAudio_F_Cubes.h"
#include "PatternsAudio\PatternAudio_N_SpectrumPeakBars.h"
#include "PatternsAudio\PatternAudio_O_Spectrum2.h"
#include "PatternsAudio\PatternAudio_R_AuroraDrop.h"
#include "PatternsAudio\PatternAudio_XR_Torus.h"
#include "PatternsAudio\PatternAudio_XS_8x8Squares.h"
#include "PatternsAudio\PatternAudio_XT_BigSpark.h"
#include "PatternsAudio\PatternAudio_XX_Aurora.h"
#include "PatternsEffects\PatternEffect_XX_NOOP.h"

// test patterns to integrate
//
#include "PatternsAudio\PatternAudio_Z_Lines.h"
#include "PatternsAudio\PatternAudio_Z_Circles.h"
#include "PatternsAudio\PatternAudio_Z_Triangles.h"
#include "PatternsAudio\PatternAudio_Z_WaveSingle.h"
#include "PatternsAudio\PatternAudio_P_DiagonalSpectrum.h"
#include "PatternsAudio\PatternAudio_Z_SpectrumCircle.h"
#include "PatternsAudio\PatternAudio_XY_2dWaves.h"
#include "PatternsAudio\PatternAudio_XY_2dGrid.h"
#include "PatternsAudio\PatternAudio_XY_3dGrid.h"

#include "PatternsOther\PatternTestCanvas.h"
#include "PatternsOther\PatternTestSpectrum.h"

#include "PatternsAudio\PatternAudio_X1_Angles.h"

class Playlist_Audio : public Playlist {

    private:

    PatternAudioAngles audioAngles;
    PatternAudioRotatingWave audioRotatingWave;
    PatternAudioCircularWave audioCircularWave;
    PatternAudioDotsSingle audioDotsSingle;
    PatternAudioRotatingSpectrum audioRotatingSpectrum;
    PatternAudioClassicSpectrum128 audioClassicSpectrum128;
    PatternAudioCubes audioCubes;
    PatternAudio8x8Squares audio8x8Squares;
    PatternAudioBigSpark audioBigSpark;
    PatternAudioTorus audioTorus;
    PatternEffectNOOP effectNOOP;

    // test
    //
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

    PatternAudio2dWaves audio2dWaves;

    PatternAudio2dGrid audio2dGrid;
    PatternAudio3dGrid audio3dGrid;

    PatternTest patternTest;
    PatternTestSpectrum patternTestSpectrum;

    PatternAudioAuroraDrop patternAuroraDrop;

    int currentIndex = 0;
    Drawable* currentItem;

    int getCurrentIndex() {

        return currentIndex;
        
    }

    const static int PATTERN_COUNT = 20;

    Drawable* shuffledItems[PATTERN_COUNT];

    Drawable* items[PATTERN_COUNT] = {

        &effectNOOP,
        &audioCircularWave,
        &audioDotsSingle,            
        &audioRotatingSpectrum,
        &audioClassicSpectrum128,
        &audioCubes,
        &audio2dWaves,
        &audio2dGrid,
        &audio3dGrid,
        &audio8x8Squares,
        &audioBigSpark,
        &audioTorus,
        &audioSpectrumPeakBars,
        &audioSpectrum2,
        &audioDiagonalSpectrum,
        &audioTriangles,          
        &audioSpectrumCircle,
        &patternAuroraDrop,
        &patternTestSpectrum,
        &audioCircles,                 
        // &audioAurora,               // not overly interesting
        // &audioWaveSingle,           // not overly interesting
        // &canvasTest,
        // &audioLines,                // not overly interesting
        // &audioAngles,               // I just don't like it
        // &audioRotatingWave,         // crashes
        // &testCircles,
        // &testSpectrum,
        // &patternTest,

    };

    public:

    Playlist_Audio() {

        // add the items to the shuffledItems array
        //
        for (int a = 0; a < PATTERN_COUNT; a++) {
        
            shuffledItems[a] = items[a];

        }

        shuffleItems();
        this->currentItem = items[0];

        for (int i=0; i < CountPlaylistsAudio; i++) {

            this->currentItem->start(i); 

        }

    }

    //char* Drawable::name = (char *)"Patterns";

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

        if (getItemEnabled(_id) != value) {

            Serial.printf("Pattern '%s' changed from %d to %d\n",getItemName(_id),getItemEnabled(_id),value);

        }

        items[_id]->setEnabled((bool)value);

    }

    bool getCurrentItemEnabled() {

        return this->currentItem->isEnabled();      
      
    }

    // Auroradrop: 

    void stop() {

        if (currentItem) {

            currentItem->stop();

        }

    }

    void start(uint8_t _pattern) {
        
        if (currentItem) {
        
            currentItem->start(_pattern);
        
        }

    }

    void move(int step, uint8_t _pattern) {

        currentIndex += step;

        if (currentIndex >= PATTERN_COUNT) {
            
            currentIndex = 0;

        } else if (currentIndex < 0) {
            
            currentIndex = PATTERN_COUNT - 1;

        }

        if (effects.paletteIndex == effects.RandomPaletteIndex) {
            
            effects.RandomPalette();
        
        }

        moveTo(currentIndex, _pattern);

    }

    void moveRandom(int step, uint8_t _pattern) {

        currentIndex += step;

        if (currentIndex >= PATTERN_COUNT) {
            
            currentIndex = 0;

        } else if (currentIndex < 0) {
            
            currentIndex = PATTERN_COUNT - 1;

        }

        if (effects.paletteIndex == effects.RandomPaletteIndex) {

            effects.RandomPalette();
        
        }

        if (currentItem) {
        
            currentItem->stop();

        }

        currentItem = shuffledItems[currentIndex];

        if (currentItem) {
        
            currentItem->start(_pattern);

        }

    }

    void shuffleItems() {

        for (int a = 0; a < PATTERN_COUNT; a++) {

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
            
            if (i == PATTERN_COUNT - 1) {
            
                Serial.println(F("\""));
            
            } else {
                
                Serial.println(F("\","));
            
            }
            
        }

        Serial.println("  ]");
        Serial.println("}");

    }

    char * getCurrentPatternName() {

        return currentItem->name;      
    
    }

    uint8_t getCurrentPatternId2() {

        return currentItem->id2;   

    }

    char * getCurrentPatternId() {

        return currentItem->id;      
      
    }


    void moveTo(int index, uint8_t _pattern) {

        if (currentItem) {

            currentItem->stop();
        
        }

        currentIndex = index;
        currentItem = items[currentIndex];

        if (currentItem) {

            currentItem->start(_pattern);

        }

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

      if (index >= PATTERN_COUNT || index < 0) {
      
        return false;
      
      }

      moveTo(index, _pattern);

      return true;
    
    }
    
};

#endif
