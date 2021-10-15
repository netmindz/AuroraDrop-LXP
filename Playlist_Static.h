#ifndef Platlist_Static_H
#define Platlist_Static_H

//#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


//#include "Vector.h"
//#include "Boid.h"
//#include "Attractor.h"

// these are work in progress
#include "PatternStatic_A_Worms.h"
#include "PatternStatic_M_SpiralLines.h"
#include "PatternStatic_M_Flock.h"
#include "PatternStatic_M_FlowField.h"
#include "PatternStatic_M_Attract.h"
#include "PatternStatic_M_Bounce.h"
#include "PatternStatic_X_Atom.h"
#include "PatternStatic_X_SimpleStars.h"

// theses are all just proof of concept from aurora demo,
#include "PatternTest.h"
#include "PatternXIncrementalDrift.h"
#include "PatternXSpiro.h"
#include "PatternXElectricMandala.h"
#include "PatternXRadar.h"
#include "PatternXSpin.h"



class Platlist_Static : public Playlist {
  private:

    PatternStaticWorms staticWorms;
    PatternStaticSimpleStars staticSimpleStars;
    PatternStaticAtom staticAtom;

    PatternTest patternTest;
    PatternSpiralLines spiralLines;
    PatternIncrementalDrift incrementalDrift;
    PatternFlock flock;
    PatternFlowField flowField;
    PatternSpiro spiro;
    PatternAttract attract;
    PatternElectricMandala electricMandella;
    PatternRadar radar;
    PatternSpin spin;
    PatternStaticBounce staticBounce;

    int currentIndex = 0;
    Drawable* currentItem;

    int getCurrentIndex() {
      return currentIndex;
    }

    const static int PATTERN_COUNT = 6;

    Drawable* shuffledItems[PATTERN_COUNT];

    Drawable* items[PATTERN_COUNT] = {

      &staticWorms,

      &spiralLines,                  // spiraling lines
      &flock,
      &attract,
      &flowField,
      &staticBounce,
      
      //&staticSimpleStars,       // don't work
      //&staticAtom,              // don't work


      //&spin,                  // BAD freezes randomly


      //&radar,

      //&patternTest,
      //&incrementalDrift,
      //&spiro,
      //&electricMandella,   // plasma effect

    };

  public:
    Platlist_Static() {
      // add the items to the shuffledItems array
      for (int a = 0; a < PATTERN_COUNT; a++) {
        shuffledItems[a] = items[a];
      }

      shuffleItems();

      this->currentItem = items[0];

      for (int i=0; i < maxPlaylistsStatic; i++) 
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
