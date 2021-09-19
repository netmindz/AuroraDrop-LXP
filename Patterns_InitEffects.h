#ifndef Patterns_InitEffects_H
#define Patterns_InitEffects_H

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


//#include "Vector.h"

#include "PatternEffectTest1.h"
#include "PatternEffectTestBlur2d.h"
#include "PatternEffectTest3.h"


class Patterns_InitEffects : public Playlist {
  private:

    PatternEffectTest1 effectTest1;
    PatternEffectTestBlur2d effectTestBlur2d;
    PatternEffectTest3 effectTest3;

    int currentIndex = 0;
    Drawable* currentItem;

    int getCurrentIndex() {
      return currentIndex;
    }

    const static int PATTERN_COUNT = 3;       // always use static on esp32 !!!!

    Drawable* shuffledItems[PATTERN_COUNT];

    Drawable* items[PATTERN_COUNT] = {
      &effectTest1,
      &effectTestBlur2d,
      &effectTest3,

    };

  public:
    Patterns_InitEffects() {
      // add the items to the shuffledItems array
      for (int a = 0; a < PATTERN_COUNT; a++) {
        shuffledItems[a] = items[a];
      }

      shuffleItems();

      this->currentItem = items[0];

      for (int i=0; i < MAX_PATTERNS_EFFECT; i++) 
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
