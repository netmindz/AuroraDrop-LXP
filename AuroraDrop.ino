#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>

/*--------------------- MATRIX GPIO CONFIG  -------------------------*/
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13
#define A_PIN 23
#define B_PIN 19 // Changed from library default
#define C_PIN 5
#define D_PIN 17
#define E_PIN 18 // 32
#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 16

#define SERIAL_MSG_AUDIO_SPECTRUM 65  // A

/*--------------------- MATRIX PANEL CONFIG -------------------------*/
#define MATRIX_HEIGHT 64
#define MATRIX_WIDTH 64
#define MATRIX_CENTRE_X (MATRIX_WIDTH / 2)
#define MATRIX_CENTRE_Y (MATRIX_HEIGHT / 2)
#define PANELS_NUMBER 1                         // Number of chained panels, if just a single panel, obviously set to 1
#define PIN_E 18  //32


#define MAX_PATTERNS_EFFECT 2
//#define MAX_PATTERNS_AMBIENT_BACKGROUND 1    // useful?
#define MAX_PATTERNS_AUDIO 3
#define MAX_PATTERNS_STATIC 2


MatrixPanel_I2S_DMA *dma_display = nullptr;

#include "SerialData.h"
SerialData serialData;

#include "Effects.h"
Effects effects;
#include "Drawable.h"
#include "Playlist.h"
//#include "Geometry.h"

#include "Vector.h"
#include "Boid.h"
#include "Attractor.h"

#include "Patterns_InitEffects.h"
#include "Patterns_Audio.h"
#include "Patterns_Static.h"

Patterns_InitEffects patternsInitEffects[MAX_PATTERNS_EFFECT];
Patterns_Audio patternsAudio[MAX_PATTERNS_AUDIO];
Patterns_Static patternsStatic[MAX_PATTERNS_STATIC];
Patterns_InitEffects patternsFinalEffects[MAX_PATTERNS_EFFECT];


/* -------------------------- Setup -------------------------- */
void setup()
{
  HUB75_I2S_CFG mxconfig;
  mxconfig.mx_height = MATRIX_HEIGHT;               // we have 64 pix heigh panels
  mxconfig.mx_width = MATRIX_WIDTH;                 // we have 64 pix heigh panels
  mxconfig.chain_length = PANELS_NUMBER;            // we have 2 panels chained
  mxconfig.gpio.e = PIN_E;                          // we MUST assign pin e to some free pin on a board to drive 64 pix height panels with 1/32 scan
  mxconfig.clkphase = false;                        // change this if you have issues with ghosting.
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;         // in case that we use panels based on FM6126A chip, we can change that

  
 /************** SERIAL **************/
  Serial.begin(115200);
  delay(1000);     // all comms to initilialise
  
 /************** DISPLAY **************/
  Serial.println("...Starting Display");
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(150); //0-255

  dma_display->fillScreenRGB888(128,128,128);
  delay(1000);
  dma_display->clearScreen();  
  Serial.println("**************** Starting Aurora Effects Demo ****************");


  // setup the effects and NOISE generator
  effects.Setup();
  
  Serial.println("Effects being loaded: ");
  listPatterns();

  // init all the initial effects patterns
  for (uint8_t i=0; i < MAX_PATTERNS_EFFECT; i++)
  {
    patternsInitEffects[i].default_fps = 90;
    patternsInitEffects[i].pattern_fps = 90;
    patternsInitEffects[i].ms_animation_max_duration = 5000;
    patternsInitEffects[i].moveRandom(1, i); // start from a random pattern
    Serial.print("Starting with intitial effects pattern: ");
    Serial.println(patternsInitEffects[i].getCurrentPatternName());
    patternsInitEffects[i].start(i);
    patternsInitEffects[i].ms_previous = millis();
    patternsInitEffects[i].fps_timer = millis();
  }

  // init all the audio effects patterns
  for (uint8_t i=0; i < MAX_PATTERNS_AUDIO; i++)
  {
    patternsAudio[i].default_fps = 90;
    patternsAudio[i].pattern_fps = 90;
    patternsAudio[i].ms_animation_max_duration = 5000;
    patternsAudio[i].moveRandom(1, i); // start from a random pattern
    Serial.print("Starting with static effects pattern: ");
    Serial.println(patternsAudio[i].getCurrentPatternName());
    patternsAudio[i].start(i);
    patternsAudio[i].ms_previous = millis();
    patternsAudio[i].fps_timer = millis();
  }

  // init all the static/non-audio effects patterns
  for (uint8_t i=0; i < MAX_PATTERNS_STATIC; i++)
  {
    patternsStatic[i].default_fps = 90;
    patternsStatic[i].pattern_fps = 90;
    patternsStatic[i].ms_animation_max_duration = 60000;
    patternsStatic[i].moveRandom(1, i); // start from a random pattern
    Serial.print("Starting with static effects pattern: ");
    Serial.println(patternsStatic[i].getCurrentPatternName());
    patternsStatic[i].start(i);
    patternsStatic[i].ms_previous = millis();
    patternsStatic[i].fps_timer = millis();
  }

  // init all the final effects patterns
  for (uint8_t i=0; i < MAX_PATTERNS_EFFECT; i++)
  {
    patternsFinalEffects[i].default_fps = 90;
    patternsFinalEffects[i].pattern_fps = 90;
    patternsFinalEffects[i].ms_animation_max_duration = 5000;
    patternsFinalEffects[i].moveRandom(1, i); // start from a random pattern
    Serial.print("Starting with intitial effects pattern: ");
    Serial.println(patternsFinalEffects[i].getCurrentPatternName());
    patternsFinalEffects[i].start(i);
    patternsFinalEffects[i].ms_previous = millis();
    patternsFinalEffects[i].fps_timer = millis();
  }



}


long restartCount = 0;

void loop()
{
  serialData.processSerialData();
  // menu.run(mainMenuItems, mainMenuItemCount);  


  // ###### --------------- 1. NEW  ----------------- ########


  for (uint8_t i=0; i < MAX_PATTERNS_EFFECT; i++)
  {
    // #-------- start next animation if maxduration reached --------#
    if ( (millis() - patternsInitEffects[i].ms_previous) > patternsInitEffects[i].ms_animation_max_duration) 
    {
      patternsInitEffects[i].stop();      
      patternsInitEffects[i].moveRandom(1, i);
      //patterns.move(1);
      patternsInitEffects[i].start(i);  
      //Serial.print("Changing pattern to:  ");
      Serial.println(patternsInitEffects[i].getCurrentPatternName());

      patternsInitEffects[i].ms_previous = millis();
      patternsInitEffects[i].fps_timer = millis();
      //ms_previous[i] = millis();
      //fps_timer[i] = millis();

      // Select a random palette as well
      //effects.RandomPalette();

    }
    if ( 1000 / patternsInitEffects[i].pattern_fps + patternsInitEffects[i].last_frame < millis())
    {
      patternsInitEffects[i].last_frame = millis();
      patternsInitEffects[i].pattern_fps = patternsInitEffects[i].drawFrame(i, MAX_PATTERNS_EFFECT);
      if (!patternsInitEffects[i].pattern_fps)
        patternsInitEffects[i].pattern_fps = patternsInitEffects[i].default_fps;
      ++patternsInitEffects[i].fps;
    }

    if (patternsInitEffects[i].fps_timer + 1000 < millis()){
      //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);
      patternsInitEffects[i].fps_timer = millis();
      patternsInitEffects[i].fps = 0;
    }

  }


  for (uint8_t i=0; i < MAX_PATTERNS_AUDIO; i++)
  {
    // #-------- start next animation if maxduration reached --------#
    if ( (millis() - patternsAudio[i].ms_previous) > patternsAudio[i].ms_animation_max_duration) 
    {
      patternsAudio[i].stop();      
      patternsAudio[i].moveRandom(1, i);
      //patterns.move(1);
      patternsAudio[i].start(i);  
      Serial.print("Changing pattern to:  "); Serial.println(patternsAudio[i].getCurrentPatternName());

      patternsAudio[i].ms_previous = millis();
      patternsAudio[i].fps_timer = millis();
      // Select a random palette as well
      //effects.RandomPalette();
    }
    if ( 1000 / patternsAudio[i].pattern_fps + patternsAudio[i].last_frame < millis())
    {
      patternsAudio[i].last_frame = millis();
      patternsAudio[i].pattern_fps = patternsAudio[i].drawFrame(i, MAX_PATTERNS_AUDIO);
      if (!patternsAudio[i].pattern_fps)
        patternsAudio[i].pattern_fps = patternsAudio[i].default_fps;
      ++patternsAudio[i].fps;
    }

    if (patternsAudio[i].fps_timer + 1000 < millis()){
      //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);
      patternsAudio[i].fps_timer = millis();
      patternsAudio[i].fps = 0;
    }

  }

  for (uint8_t i=0; i < MAX_PATTERNS_STATIC; i++)
  {
    // #-------- start next animation if maxduration reached --------#
    if ( (millis() - patternsStatic[i].ms_previous) > patternsStatic[i].ms_animation_max_duration) 
    {
      patternsStatic[i].stop();      
      patternsStatic[i].moveRandom(1, i);
      //patterns.move(1);
      patternsStatic[i].start(i);  
      //Serial.print("Changing pattern to:  ");
      Serial.println(patternsStatic[i].getCurrentPatternName());

      patternsStatic[i].ms_previous = millis();
      patternsStatic[i].fps_timer = millis();
      //ms_previous[i] = millis();
      //fps_timer[i] = millis();

      // Select a random palette as well
      //effects.RandomPalette();

    }
    if ( 1000 / patternsStatic[i].pattern_fps + patternsStatic[i].last_frame < millis())
    {
      patternsStatic[i].last_frame = millis();
      patternsStatic[i].pattern_fps = patternsStatic[i].drawFrame(i, MAX_PATTERNS_STATIC);
      if (!patternsStatic[i].pattern_fps)
        patternsStatic[i].pattern_fps = patternsStatic[i].default_fps;
      ++patternsStatic[i].fps;
    }

    if (patternsStatic[i].fps_timer + 1000 < millis()){
      //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);
      patternsStatic[i].fps_timer = millis();
      patternsStatic[i].fps = 0;
    }

  }



/*
  // apply final set of effects
  for (uint8_t i=0; i < MAX_PATTERNS_1_EFFECT; i++)
  {
    // #-------- start next animation if maxduration reached --------#
    if ( (millis() - patternsFinalEffects[i].ms_previous) > patternsFinalEffects[i].ms_animation_max_duration) 
    {
      patternsFinalEffects[i].stop();      
      patternsFinalEffects[i].moveRandom(1, i);
      //patterns.move(1);
      patternsFinalEffects[i].start(i);  
      //Serial.print("Changing pattern to:  ");
      Serial.println(patternsFinalEffects[i].getCurrentPatternName());

      patternsFinalEffects[i].ms_previous = millis();
      patternsFinalEffects[i].fps_timer = millis();
      //ms_previous[i] = millis();
      //fps_timer[i] = millis();

      // Select a random palette as well
      //effects.RandomPalette();

    }
    if ( 1000 / patternsFinalEffects[i].pattern_fps + patternsFinalEffects[i].last_frame < millis())
    {
      patternsFinalEffects[i].last_frame = millis();
      patternsFinalEffects[i].pattern_fps = patternsFinalEffects[i].drawFrame(i, MAX_PATTERNS_1_EFFECT);
      if (!patternsFinalEffects[i].pattern_fps)
        patternsFinalEffects[i].pattern_fps = patternsFinalEffects[i].default_fps;
      ++patternsFinalEffects[i].fps;
    }

    if (patternsFinalEffects[i].fps_timer + 1000 < millis()){
      //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);
      patternsFinalEffects[i].fps_timer = millis();
      patternsFinalEffects[i].fps = 0;
    }

  }
*/




  // finally, show the frame
  effects.ShowFrame();

}


void listPatterns() {
  patternsAudio[0].listPatterns();
}
