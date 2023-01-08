#ifndef Playlist_InitialEffects_H
#define Playlist_InitialEffects_H

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//#include "Vector.h"

#include "PatternsEffects\PatternEffect_A_TestBlur2d.h"
#include "PatternsEffects\PatternEffect_B_SpiralStream1.h"
#include "PatternsEffects\PatternEffect_C_Stream1.h"
#include "PatternsEffects\PatternEffect_D_Move.h"
// #include "PatternsEffects\PatternEffect_E_Minimal.h"
#include "PatternsEffects\PatternEffect_XX_NOOP.h"
#include "PatternsEffects\PatternEffect_X_Munch.h"

class Playlist_Foreground : public Playlist {

    private:

    PatternEffectSpiralStream1 effectSpiralStream1;
    PatternEffectTestBlur2d effectTestBlur2d;
    PatternEffectStream1 effectStream1;
    PatternEffectMove effectMove;
    PatternEffectMunch effectMunch;
    // PatternEffectMinimal effectMinimal;
    PatternEffectNOOP effectNOOP;

    int currentIndex = 0;
    Drawable* currentItem;

    int getCurrentIndex() {

      return currentIndex;

    }

    const static int PATTERN_COUNT = 6;

    Drawable* shuffledItems[PATTERN_COUNT];

    Drawable* items[PATTERN_COUNT] = {
        
        &effectTestBlur2d,
        &effectSpiralStream1,
        &effectStream1,
        &effectMove,
        &effectMunch,              // moved to foreground as it doesn't do anything to background
        &effectNOOP,
        // &effectMinimal,         // janky

    };

    public:

    Playlist_Foreground() {

        // add the items to the shuffledItems array
        //
        for (int a = 0; a < PATTERN_COUNT; a++) {

            shuffledItems[a] = items[a];
        }

        shuffleItems();

        this->currentItem = items[0];

        for (int i=0; i < CountPlaylistsForeground; i++) {

            this->currentItem->start(i); 

        }

    }

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
