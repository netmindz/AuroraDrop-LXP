/*
*   Suggested simple hardware solution:-
*   
*   ESP32 I2S Matrix Shield
*   https://github.com/witnessmenow/ESP32-i2s-Matrix-Shield
*   https://www.youtube.com/watch?v=ZiR93TmSyE0
*
*   Libraries needed:-
*
*   FastLED (tested with v3.4)
*   https://github.com/FastLED/FastLED
*
*   ESP32 HUB75 LED MATRIX PANEL DMA Display (tested with v2.0.5)
*   https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA
*
*/

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
#define E_PIN 14 // 32 or 18
#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 16

#define SERIAL_MSG_AUDIO_SPECTRUM 65  // A

/*--------------------- MATRIX PANEL CONFIG -------------------------*/
#define MATRIX_HEIGHT 64
#define MATRIX_WIDTH 64
#define MATRIX_CENTER_X (MATRIX_WIDTH / 2)
#define MATRIX_CENTER_Y (MATRIX_HEIGHT / 2)
#define PANELS_NUMBER 1                         // Number of chained panels, if just a single panel, obviously set to 1

// fixed maximums here for memory allocation, these must be >= variables used below
//#define MAX_PATTERNS_AMBIENT_BACKGROUND 1     // not used yet? useful? plasma effects?
#define MAX_PATTERNS_EFFECT 3                   // bluring/fading/sweeping effects
#define MAX_PATTERNS_AUDIO 3                    // audio re-active effects
#define MAX_PATTERNS_STATIC 3                   // standard non-audio animations inc. boids etc.

// (in future) limits may be applied in real-time by logic on how many patterns are being looped simultaneously, change these for quick testing
//static uint8_t maxPatternAmbient = 0;       // not used yet?
static uint8_t maxPatternFinalEffect = 0;     // not used yet?
static uint8_t maxPatternInitEffect = 2;      // <------- 2 is reasonable
static uint8_t maxPatternAudio = 3;           // <------- 2/3
static uint8_t maxPatternStatic = 2;          // <------- 2/3

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

static uint8_t PatternsAudioMainEffectCount = 0;
static uint8_t PatternsAudioBackdropCount = 0;
static uint8_t PatternsAudioCaleidoscopeCount = 0;
static uint16_t PatternsAudioBluringCount = 0;

/* -------------------------- Setup -------------------------- */
void setup()
{
  HUB75_I2S_CFG mxconfig;
  mxconfig.mx_height = MATRIX_HEIGHT;               // we have 64 pix height panels
  mxconfig.mx_width = MATRIX_WIDTH;                 // we have 64 pix height panels
  mxconfig.chain_length = PANELS_NUMBER;            // we have x panels chained
  mxconfig.gpio.e = E_PIN;                          // we MUST assign pin e to some free pin on a board to drive 64 pix height panels with 1/32 scan
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
  dma_display->fillScreenRGB888(255,0,0);
  delay(200);
  dma_display->fillScreenRGB888(0,255,0);
  delay(200);
  dma_display->fillScreenRGB888(0,0,255);
  delay(200);
  dma_display->fillScreenRGB888(128,128,128);
  delay(200);
  dma_display->clearScreen();  
  Serial.println("**************** Starting Aurora Effects Demo ****************");

  // setup the effects and NOISE generator
  effects.Setup();
  
  Serial.println("Effects being loaded: ");
  listPatterns();

  // initialise all the initial effects patterns
  for (uint8_t i=0; i < maxPatternInitEffect; i++)
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

  // initialise all the audio effects patterns
  for (uint8_t i=0; i < maxPatternAudio; i++)
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

  // initialise all the static/non-audio effects patterns
  for (uint8_t i=0; i < maxPatternStatic; i++)
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

  // initialise all the final effects patterns
  for (uint8_t i=0; i < maxPatternFinalEffect; i++)
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
  // check if there is any new audio data and proccess it to useful bins and flags used by the patterns
  serialData.processSerialData();

  // TODO: implement aurora demo menu system?
  // menu.run(mainMenuItems, mainMenuItemCount);  

  // TODO: nearly? always dim the screen by an arbitrary amount
  //       or do we allow effects to always do the dimming? 
  if (maxPatternInitEffect==0) effects.DimAll(250);       // if we have no effects enabled, dim screen by small amount (e.g. when testing)

  // clear counters/flags for psuedo randomness workings inside pattern setup and drawing
  PatternsAudioMainEffectCount = 0;
  PatternsAudioBackdropCount = 0;
  PatternsAudioCaleidoscopeCount = 0;
  PatternsAudioBluringCount = 0;


  // -------------- loop through, and apply each of the initial effects patterns ----------------
  for (uint8_t i=0; i < maxPatternInitEffect; i++)
  {
    // -------- start next animation if max duration reached --------
    if ( (millis() - patternsInitEffects[i].ms_previous) > patternsInitEffects[i].ms_animation_max_duration) 
    {
      patternsInitEffects[i].stop();      
      patternsInitEffects[i].moveRandom(1, i);
      // patterns.move(1);
      patternsInitEffects[i].start(i);  
      Serial.print("Changing initial effects pattern X  to:  ");
      Serial.println(patternsInitEffects[i].getCurrentPatternName());
      patternsInitEffects[i].ms_previous = millis();
      patternsInitEffects[i].fps_timer = millis();
    }
    // -------- draw the next frame if fps timer dictates so --------
    if ( 1000 / patternsInitEffects[i].pattern_fps + patternsInitEffects[i].last_frame < millis())
    {
      patternsInitEffects[i].last_frame = millis();
      patternsInitEffects[i].pattern_fps = patternsInitEffects[i].drawFrame(i, maxPatternInitEffect);
      if (!patternsInitEffects[i].pattern_fps)
        patternsInitEffects[i].pattern_fps = patternsInitEffects[i].default_fps;
      ++patternsInitEffects[i].fps;
    }
    // ----- update fps and timer
    if (patternsInitEffects[i].fps_timer + 1000 < millis()){
      //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);
      patternsInitEffects[i].fps_timer = millis();
      patternsInitEffects[i].fps = 0;
    }
  }



  // -------------- loop through, and apply each of the audio re-active patterns ----------------
  for (uint8_t i=0; i < maxPatternAudio; i++)
  {
    // -------- start next animation if max duration reached --------
    if ( (millis() - patternsAudio[i].ms_previous) > patternsAudio[i].ms_animation_max_duration) 
    {
      patternsAudio[i].stop();      
      patternsAudio[i].moveRandom(1, i);
      //patterns.move(1);
      patternsAudio[i].start(i);  
      Serial.print("Changing audio pattern X to:  ");
      Serial.println(patternsAudio[i].getCurrentPatternName());
      patternsAudio[i].ms_previous = millis();
      patternsAudio[i].fps_timer = millis();
      // select a random palette when ANY of the audio patterns start/re-start, this can look funky when/if they start changing out out sync
      // TODO: consider randomly locking palette change to only when the first pattern of the group re-starts (or maybe also when an initial effect restarts)
      // possible bug? if we change the palette when the palette is currently transitioning to another, the palette changes abruptly?
      effects.RandomPalette();
    }
    // -------- draw the next frame if fps timer dictates so --------
    if ( 1000 / patternsAudio[i].pattern_fps + patternsAudio[i].last_frame < millis())
    {
      patternsAudio[i].last_frame = millis();
      patternsAudio[i].pattern_fps = patternsAudio[i].drawFrame(i, maxPatternAudio);
      if (!patternsAudio[i].pattern_fps)
        patternsAudio[i].pattern_fps = patternsAudio[i].default_fps;
      ++patternsAudio[i].fps;
    }
    // ----- update fps and timer
    if (patternsAudio[i].fps_timer + 1000 < millis()){
      //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);
      patternsAudio[i].fps_timer = millis();
      patternsAudio[i].fps = 0;
    }
  }




  // ---------- loop through, and apply each of the audio static (mostly) patterns, e.g. boids --------
  for (uint8_t i=0; i < maxPatternStatic; i++)
  {
    // -------- start next animation if max duration reached --------
    if ( (millis() - patternsStatic[i].ms_previous) > patternsStatic[i].ms_animation_max_duration) 
    {
      patternsStatic[i].stop();      
      patternsStatic[i].moveRandom(1, i);
      //patterns.move(1);
      patternsStatic[i].start(i);  
      Serial.print("Changing statix pattern X to:  ");
      Serial.println(patternsStatic[i].getCurrentPatternName());
      patternsStatic[i].ms_previous = millis();
      patternsStatic[i].fps_timer = millis();
    }
    // -------- draw the next frame if fps timer dictates so --------
    if ( 1000 / patternsStatic[i].pattern_fps + patternsStatic[i].last_frame < millis())
    {
      patternsStatic[i].last_frame = millis();
      patternsStatic[i].pattern_fps = patternsStatic[i].drawFrame(i, maxPatternStatic);
      if (!patternsStatic[i].pattern_fps)
        patternsStatic[i].pattern_fps = patternsStatic[i].default_fps;
      ++patternsStatic[i].fps;
    }
    // ----- update fps and timer
    if (patternsStatic[i].fps_timer + 1000 < millis()){
      //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);
      patternsStatic[i].fps_timer = millis();
      patternsStatic[i].fps = 0;
    }
  }



/*
  // apply final set of effects   ??? DO WE NEED MOST POST EFFECTS ???
  for (uint8_t i=0; i < maxPatternFinalEffect; i++)
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
      patternsFinalEffects[i].pattern_fps = patternsFinalEffects[i].drawFrame(i, maxPatternFinalEffect);
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


  // finally, update the panel with the final rendered frame
  effects.ShowFrame();
}

void listPatterns() {
  patternsAudio[0].listPatterns();
}
