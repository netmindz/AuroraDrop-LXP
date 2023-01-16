/*
*   Suggested simple hardware solution:-
*   
*   ESP32 I2S Matrix Shield
*   https://github.com/witnessmenow/ESP32-i2s-Matrix-Shield
*   https://www.youtube.com/watch?v=ZiR93TmSyE0
*
*   Libraries needed:-
*
*   FastLED (tested with v3.5)
*   https://github.com/FastLED/FastLED
*
*   ESP32 HUB75 LED MATRIX PANEL DMA Display (tested with v3.0.0)
*   https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA
*
*   ArduinoFFT - "develop" branch (Last commit Sept 26, 2022)
*   https://github.com/kosme/arduinoFFT/tree/develop
*
*/

// =================================================================================================
// == START basic user configuration ===============================================================
// =================================================================================================

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 64               
#define PANELS_NUMBER 2

int GLOBAL_BRIGHTNESS = 64;    //0-255 - this gets overridden with the ADC value if BRIGHT_PIN is defined - otherwise it stays here.

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// #define PRODUCTION_BOARD

#ifdef PRODUCTION_BOARD

    #pragma message("HUB75 - Production Board Pinout")
    // HUB75 Pins
    //
    #define R1_PIN 21
    #define G1_PIN 16
    #define B1_PIN 26
    // placeholder GND - skip
    #define R2_PIN 33
    #define G2_PIN 14
    #define B2_PIN 34
    #define E_PIN 13
    #define A_PIN 35
    #define B_PIN 12
    #define C_PIN 36
    #define D_PIN 11 // GND on silkscreen, but working example is assigned to a pin tho where "D" should be on the HUB75 connector, after C
    #define CLK_PIN 37
    #define LAT_PIN 10
    #define OE_PIN 38
    // placeholder GND - skip

    // INMP441 Pins
    //
    #define I2S_WS  6
    #define I2S_SCK 7
    #define I2S_SD  8

    // ESP32-S3 Devkit C/M have these
    //
    #define ONBOARD_RGB_LED_PIN 48

    // Brightness ADC pin
    //
    #define BRIGHT_PIN 1

    // Quick definition for debug pin, optional
    // Boot button is often 0 and LOW
    //
    #define PIN_FOR_DEBUG_MODE 0
    #define PIN_FOR_DEBUG_MODE_STATE LOW

    // Show personalized dedication on debug screen - don't use this
    //
    // #define DEDICATION

#else

    #pragma message("HUB75 - Testbed Board Pinout")

    // Testbed Board

    // HUB75 Pins
    //
    #define R1_PIN 42
    #define G1_PIN 6
    #define B1_PIN 41
    // placeholder GND - 7
    #define R2_PIN 40
    #define G2_PIN 15
    #define B2_PIN 39
    #define E_PIN 16
    #define A_PIN 38
    #define B_PIN 17
    #define C_PIN 37
    #define D_PIN 18 // GND on silkscreen, but working example is assigned to a pin tho where "D" should be on the HUB75 connector, after C
    #define CLK_PIN 36
    #define LAT_PIN 8
    #define OE_PIN 35
    // placeholder GND - 3

    // INMP441 Pins
    //
    #define I2S_WS  19
    #define I2S_SCK 20
    #define I2S_SD  21

    // ESP32-S3 Devkit C/M have these
    //
    #define ONBOARD_RGB_LED_PIN 48

    // Brightness ADC pin
    //
    // #define BRIGHT_PIN 1

    // Quick definition for debug pin, optional
    // Boot button is often 0 and LOW
    //
    #define PIN_FOR_DEBUG_MODE 0
    #define PIN_FOR_DEBUG_MODE_STATE LOW

#endif

MatrixPanel_I2S_DMA *dma_display = nullptr;

#include <FastLED.h>

#ifdef ONBOARD_RGB_LED_PIN

    // If it's present and not initialized, it can behave randonly...

    CRGB statusled[1];
    CLEDController* statuscontroller = &FastLED.addLeds<NEOPIXEL, ONBOARD_RGB_LED_PIN>(statusled, 1);

#endif

#define MATRIX_WIDTH (PANEL_WIDTH * PANELS_NUMBER)         // works with 1x to 4x 32x64 and 64x32 panels in landscape
#define MATRIX_HEIGHT (PANEL_HEIGHT)                        // doesn't seem to making "tall" portrait arrays - yet.
#define MATRIX_CENTER_X (MATRIX_WIDTH / 2)
#define MATRIX_CENTER_Y (MATRIX_HEIGHT / 2)

// diagnostic options, some can be enabled by a button if setup
//
bool option1Diagnostics = false;
bool option2LockFps = false;
bool option3ShowRenderTime = false;
bool option4PauseCycling = false;
bool option5ShowEffectsStack = false;
bool option6DisableForeground = false;
bool option7DisableAudio = false;
bool option8DisableStatic = false;
bool option9DisableBackground = false;
bool option10DisableCaleidoEffects = false;

class FFTData{

    // this class holds some data, but nothing calls its functions...
    // ... and a lot of things are defined and never used, so paired down significantly.
    // ... mostly this is here to avoid many edits to remove the references to 
    //     this class.

    public:
    
    #define BINS 128        

    uint16_t bpm = 120;    // for testing currently, not fully implemented yet by most test patterns

    byte specDataMaxVolume;
    byte specDataMinVolume;

    byte specData8[8];
    byte specData16[16];
    byte specData32[32];
    byte specData64[64];
    byte specData[128];

    bool noAudio = true;

};

FFTData fftData;

#include "FftMic.h"

// fixed maximums here for memory allocation, these must be >= variables used below
//
#define MAX_PLAYLISTS_BACKGROUND 1               // Background effect. Leave it at 1.
#define MAX_PLAYLISTS_AUDIO 2                    // audio reactive effects
#define MAX_PLAYLISTS_STATIC 2                   // standard non-audio animations inc. boids etc.
#define MAX_PLAYLISTS_FOREGROUND 2               // bluring/fading/sweeping effects
//
// ...this is also the order the effects are layered - with "FOREGROUND" on top

static uint8_t CountPlaylistsBackground = MAX_PLAYLISTS_BACKGROUND;        // This is now the farthest back effects, background of the entire frame.
static uint8_t CountPlaylistsAudio = MAX_PLAYLISTS_AUDIO;                  // <------- 2 or 3
static uint8_t CountPlaylistsStatic = MAX_PLAYLISTS_STATIC;                // <------- 2 or 3
static uint8_t CountPlaylistsForeground = MAX_PLAYLISTS_FOREGROUND;        // <------- 1 or 2 - Foreground effect 

#include "Geometry.h"
#include "Effects.h"
Effects effects;
#include "Drawable.h"
#include "Playlist.h"

#include "Vector.h"
#include "Boid.h"
#include "Attractor.h"

#include "Playlist_Foreground.h"
#include "Playlist_Background.h"
#include "Playlist_Audio.h"
#include "Playlist_Static.h"

Playlist_Background playlistBackground[MAX_PLAYLISTS_BACKGROUND];
Playlist_Audio playlistAudio[MAX_PLAYLISTS_AUDIO];
Platlist_Static playlistStatic[MAX_PLAYLISTS_STATIC];
Playlist_Foreground playlistForeground[MAX_PLAYLISTS_FOREGROUND];

// TODO: sort? useful or not - I don't know if this does anything...
//
static uint8_t PatternsAudioMainEffectCount = 0;
static uint8_t PatternsAudioBackdropCount = 0;
static uint8_t PatternsAudioCaleidoscopeCount = 0;
static uint16_t PatternsAudioBluringCount = 0;

uint32_t startMillis = millis();
uint32_t Xlast_render_ms = millis();

#include "Diagnostics.h"

void setup() {
 
     #ifdef BRIGHT_PIN

        // set on startup so we don't blow up someone's eyeballs 
        
        analogReadResolution(8);

        GLOBAL_BRIGHTNESS = analogRead(BRIGHT_PIN);
    
    #endif

    Serial.begin(115200);
    delay(1000);

    Serial.printf("ESP32 IDF version: %s\n",IDF_VER);

    #ifdef ONBOARD_RGB_LED_PIN

        statuscontroller->leds()[0] = CRGB::Purple;
        statuscontroller->showLeds(10);

    #endif

    HUB75_I2S_CFG mxconfig;
    mxconfig.mx_height = PANEL_HEIGHT;
    mxconfig.mx_width = PANEL_WIDTH; 
    mxconfig.chain_length = PANELS_NUMBER;
    mxconfig.gpio.r1  =  R1_PIN; 
    mxconfig.gpio.g1  =  G1_PIN; 
    mxconfig.gpio.b1  =  B1_PIN; 
    mxconfig.gpio.r2  =  R2_PIN; 
    mxconfig.gpio.g2  =  G2_PIN; 
    mxconfig.gpio.b2  =  B2_PIN; 
    mxconfig.gpio.a   =   A_PIN; 
    mxconfig.gpio.b   =   B_PIN; 
    mxconfig.gpio.c   =   C_PIN; 
    mxconfig.gpio.d   =   D_PIN;
    mxconfig.gpio.e   =   E_PIN; 
    mxconfig.gpio.lat = LAT_PIN; 
    mxconfig.gpio.oe  =  OE_PIN;
    mxconfig.gpio.clk = CLK_PIN; 
    mxconfig.driver   = HUB75_I2S_CFG::FM6126A;         // my panels (with D or E pins) have needed this.

    Serial.println("Starting HUB75 Display Test...");

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);

    dma_display->begin();
    dma_display->setBrightness8(GLOBAL_BRIGHTNESS); 
    dma_display->fillScreenRGB888(255,0,0);
    delay(150);
    dma_display->fillScreenRGB888(0,255,0);
    delay(150);
    dma_display->fillScreenRGB888(0,0,255);
    delay(150);
    dma_display->fillScreenRGB888(128,128,128);
    delay(150);
    dma_display->clearScreen();

    setupAudio();

    Serial.println("Starting AuroraDrop LXG Demo...");

    // setup the effects and noise generator
    //
    effects.Setup();

    Serial.println("Effects being loaded: ");
    listPatterns();

    // initialise all the initial effects patterns
    //
    for (uint8_t i=0; i < MAX_PLAYLISTS_FOREGROUND; i++) {

        playlistForeground[i].default_fps = 90;
        playlistForeground[i].pattern_fps = 90;
        playlistForeground[i].ms_animation_max_duration = animation_duration;

        do {
            playlistForeground[i].moveRandom(1, i);
        } while (!playlistForeground[i].getCurrentItemEnabled());

        Serial.print("Starting with foreground pattern: ");
        Serial.println(playlistForeground[i].getCurrentPatternName());

        playlistForeground[i].start(i);
        playlistForeground[i].ms_previous = millis();
        playlistForeground[i].fps_timer = millis();

        for (uint8_t j=0; j < playlistForeground[i].getPatternCount(); j++) {
            
            playlistForeground[i].setItemEnabled(j, 1);

            if (MATRIX_WIDTH > 128+64) {

                if (playlistForeground[i].getItemName(j) == "Munch") playlistForeground[i].setItemEnabled(j, 0);

            }

        }

    }

    // initialise all the audio effects patterns
    //
    for (uint8_t i=0; i < MAX_PLAYLISTS_AUDIO; i++) {
        
        playlistAudio[i].default_fps = 90;
        playlistAudio[i].pattern_fps = 90;
        playlistAudio[i].ms_animation_max_duration = animation_duration;

        do {
            playlistAudio[i].moveRandom(1, i);
        } while (!playlistAudio[i].getCurrentItemEnabled());

        Serial.print("Starting with static effects pattern: ");
        Serial.println(playlistAudio[i].getCurrentPatternName());

        playlistAudio[i].start(i);
        playlistAudio[i].ms_previous = millis();
        playlistAudio[i].fps_timer = millis();

        // TESTING: enable all the effects
        for (uint8_t j=0; j < playlistAudio[i].getPatternCount(); j++) {
            
            playlistAudio[i].setItemEnabled(j, 1);

            if (MATRIX_WIDTH > 128) {

                if (playlistAudio[i].getItemName(j) == "Audio Dots Single") playlistAudio[i].setItemEnabled(j, 0);

                if (playlistAudio[i].getItemName(j) == "AuroraDrop") playlistAudio[i].setItemEnabled(j, 0);

            }

            if (MATRIX_WIDTH > 128+64) {

                if (playlistAudio[i].getItemName(j) == "Test Spectrum Pattern") playlistAudio[i].setItemEnabled(j, 0);

                if (playlistAudio[i].getItemName(j) == "Classic 128 Spectrum") playlistAudio[i].setItemEnabled(j, 0);

            }


        }

    }

    // initialise all the static/non-audio effects patterns
    //
    for (uint8_t i=0; i < MAX_PLAYLISTS_STATIC; i++) {

        playlistStatic[i].default_fps = 90;
        playlistStatic[i].pattern_fps = 90;
        playlistStatic[i].ms_animation_max_duration = animation_duration*8;  // 16 beats * 8

        do {
            playlistStatic[i].moveRandom(1, i);
        } while (!playlistStatic[i].getCurrentItemEnabled());

        Serial.print("Starting with static effects pattern: ");
        Serial.println(playlistStatic[i].getCurrentPatternName());

        playlistStatic[i].start(i);
        playlistStatic[i].ms_previous = millis();
        playlistStatic[i].fps_timer = millis();

        // TESTING: enable all the effects
        for (uint8_t j=0; j < playlistStatic[i].getPatternCount(); j++) {
            playlistStatic[i].setItemEnabled(j, 1);
        }

    }

    for (uint8_t i=0; i < MAX_PLAYLISTS_BACKGROUND; i++) {

        playlistBackground[i].default_fps = 90;
        playlistBackground[i].pattern_fps = 90;
        playlistBackground[i].ms_animation_max_duration = animation_duration;

        do {
            playlistBackground[i].moveRandom(1, i);
        } while (!playlistBackground[i].getCurrentItemEnabled());

        Serial.print("Starting with intitial effects pattern: ");
        Serial.println(playlistBackground[i].getCurrentPatternName());

        playlistBackground[i].start(i);
        playlistBackground[i].ms_previous = millis();
        playlistBackground[i].fps_timer = millis();

        // TESTING: enable all the effects
        for (uint8_t j=0; j < playlistBackground[i].getPatternCount(); j++) {
            playlistBackground[i].setItemEnabled(j, 1);
        }

    }

    Xlast_render_ms = millis();

}

void loop() {

    #ifdef BRIGHT_PIN
    
        GLOBAL_BRIGHTNESS = (GLOBAL_BRIGHTNESS * 0.9) + (analogRead(BRIGHT_PIN) * 0.1);
    
        if (GLOBAL_BRIGHTNESS < 2) {

            GLOBAL_BRIGHTNESS = 0;

        }

        if (GLOBAL_BRIGHTNESS == 0) {
            
            delay(500);

            return;
        }
        
    #endif

    #ifdef PIN_FOR_DEBUG_MODE
    
        if (digitalRead(PIN_FOR_DEBUG_MODE) == PIN_FOR_DEBUG_MODE_STATE) {

            // OFF, Main Debug, Effects Stack List

            if (option1Diagnostics == false && option5ShowEffectsStack == false) {

                option1Diagnostics = true;
                option5ShowEffectsStack = false;

            } else if (option1Diagnostics == true && option5ShowEffectsStack == false) {

                option1Diagnostics = false;
                option5ShowEffectsStack = true;
                
            } else {

                option1Diagnostics = false;
                option5ShowEffectsStack = false;

            }

            delay(500); // simple debounce

        }

    #endif
    
    start_render_ms = millis();

    // check here if we are ready to render the next frame or not
    //
    if (option2LockFps && millis() < Xlast_render_ms + 50) return;  // 40=25fps, 50=20fps;

    Xlast_render_ms = millis();

    if (CountPlaylistsForeground==0 || option6DisableForeground) effects.DimAll(230);       // if we have no effects enabled, dim screen by small amount (e.g. during testing)

    // clear counters/flags for psuedo randomness workings inside pattern setup and drawing
    //
    PatternsAudioMainEffectCount = 0;
    PatternsAudioBackdropCount = 0;
    PatternsAudioCaleidoscopeCount = 0;
    PatternsAudioBluringCount = 0;

    // Background effects - rendered first (and the back)
    //
    if (!option9DisableBackground) {

        for (uint8_t i=0; i < CountPlaylistsBackground; i++) {

            // #-------- start next animation if maxduration reached --------#
            //
            if ( (millis() - playlistBackground[i].ms_previous) > playlistBackground[i].ms_animation_max_duration) {

                if (!option4PauseCycling) {

                    playlistBackground[i].stop();

                    do {

                        playlistBackground[i].moveRandom(1, i);

                    } while (!playlistBackground[i].getCurrentItemEnabled());

                    playlistBackground[i].ms_animation_max_duration = animation_duration;
                    playlistBackground[i].start(i);  

                    Serial.print("Changing background effect pattern to: ");
                    Serial.println(playlistBackground[i].getCurrentPatternName());

                    playlistBackground[i].ms_previous = millis();
                    playlistBackground[i].fps_timer = millis();

                }

            }

            // -------- draw the next frame if fps timer dictates so --------
            //
            if (1000 / playlistBackground[i].pattern_fps + playlistBackground[i].last_frame < millis()) {

                playlistBackground[i].last_frame = millis();
                playlistBackground[i].pattern_fps = playlistBackground[i].drawFrame(i, CountPlaylistsBackground);

                if (!playlistBackground[i].pattern_fps) {

                    playlistBackground[i].pattern_fps = playlistBackground[i].default_fps;

                }

                ++playlistBackground[i].fps;
                playlistBackground[i].render_ms = millis() - playlistBackground[i].last_frame;

            }

            if (playlistBackground[i].fps_timer + 1000 < millis()){

                playlistBackground[i].fps_timer = millis();
                playlistBackground[i].fps_last = playlistBackground[i].fps;

                actual_fps = playlistBackground[i].fps;

                playlistBackground[i].fps = 0;

            }

        }   

    }

    // -------------- loop through, and apply each of the audio re-active patterns ----------------
    //
    if (!option7DisableAudio) {

        for (uint8_t i=0; i < CountPlaylistsAudio; i++) {

            // -------- start next animation if max duration reached --------
            //
            if ( (millis() - playlistAudio[i].ms_previous) > playlistAudio[i].ms_animation_max_duration) {

                if (!option4PauseCycling) {

                    playlistAudio[i].stop();      

                    do {

                        playlistAudio[i].moveRandom(1, i);
                        
                    } while (!playlistAudio[i].getCurrentItemEnabled());

                    playlistAudio[i].ms_animation_max_duration = animation_duration;
                    playlistAudio[i].start(i);  

                    Serial.print("Changing audio pattern to: ");
                    Serial.println(playlistAudio[i].getCurrentPatternName());

                    Serial.printf("multAgc: %f\n", multAgc);
                    Serial.printf("Brightness: %d\n", GLOBAL_BRIGHTNESS);

                    playlistAudio[i].ms_previous = millis();
                    playlistAudio[i].fps_timer = millis();

                    // select a random palette when ANY of the audio patterns start/re-start, this can look funky when/if they start changing out out sync
                    // TODO: consider randomly locking palette change to only when the first pattern of the group re-starts (or maybe also when an initial effect restarts)
                    // possible bug? if we change the palette when the palette is currently transitioning to another, the palette changes abruptly?
                    //
                    effects.RandomPalette();

                }

            }

            // -------- draw the next frame if fps timer dictates so --------
            //
            if ( 1000 / playlistAudio[i].pattern_fps + playlistAudio[i].last_frame < millis()) {

                playlistAudio[i].last_frame = millis();
                playlistAudio[i].pattern_fps = playlistAudio[i].drawFrame(i, CountPlaylistsAudio);

                if (!playlistAudio[i].pattern_fps) {

                    playlistAudio[i].pattern_fps = playlistAudio[i].default_fps;

                }

                ++playlistAudio[i].fps;
                playlistAudio[i].render_ms = millis() - playlistAudio[i].last_frame;

            }

            // ----- every 1000ms update fps and timer
            //
            if (playlistAudio[i].fps_timer + 1000 < millis()){

                playlistAudio[i].fps_timer = millis();
                playlistAudio[i].fps_last = playlistAudio[i].fps;

                actual_fps = playlistAudio[i].fps;

                playlistAudio[i].fps = 0;

            }
            
        }

    }

    // ---------- loop through, and apply each of the audio static (mostly) patterns, e.g. boids --------
    //
    if (!option8DisableStatic) {

        for (uint8_t i=0; i < CountPlaylistsStatic; i++) {

            // -------- start next animation if max duration reached --------
            //
            if ((millis() - playlistStatic[i].ms_previous) > playlistStatic[i].ms_animation_max_duration) {

                if (!option4PauseCycling) {

                    playlistStatic[i].stop();      

                    do {

                        playlistStatic[i].moveRandom(1, i);

                    } while (!playlistStatic[i].getCurrentItemEnabled());

                    playlistStatic[i].ms_animation_max_duration = animation_duration;
                    playlistStatic[i].start(i);  

                    Serial.print("Changing static pattern to: ");
                    Serial.println(playlistStatic[i].getCurrentPatternName());

                    playlistStatic[i].ms_previous = millis();
                    playlistStatic[i].fps_timer = millis();

                }

            }

            // -------- draw the next frame if fps timer dictates so --------
            //
            if (1000 / playlistStatic[i].pattern_fps + playlistStatic[i].last_frame < millis()) {

                playlistStatic[i].last_frame = millis();
                playlistStatic[i].pattern_fps = playlistStatic[i].drawFrame(i, CountPlaylistsStatic);

                if (!playlistStatic[i].pattern_fps) {

                    playlistStatic[i].pattern_fps = playlistStatic[i].default_fps;

                }

                ++playlistStatic[i].fps;
                playlistStatic[i].render_ms = millis() - playlistStatic[i].last_frame;

            }

            // ----- every 1000ms update fps and timer
            //
            if (playlistStatic[i].fps_timer + 1000 < millis()){

                playlistStatic[i].fps_timer = millis();
                playlistStatic[i].fps_last = playlistStatic[i].fps;

                actual_fps = playlistStatic[i].fps;

                playlistStatic[i].fps = 0;

            }

        }

    }

    // This is the most foreground effect(s). This allows blurs over the entire rendered frame, for example 
    //
    if (!option6DisableForeground) {

        for (uint8_t i=0; i < CountPlaylistsForeground; i++) {

            // -------- start next animation if max duration reached --------
            //
            if ( (millis() - playlistForeground[i].ms_previous) > playlistForeground[i].ms_animation_max_duration) {

                if (!option4PauseCycling) {

                    playlistForeground[i].stop();      

                    do {

                        playlistForeground[i].moveRandom(1, i);
                        
                    } while (!playlistForeground[i].getCurrentItemEnabled());

                    playlistForeground[i].ms_animation_max_duration = animation_duration;
                    playlistForeground[i].start(i); 

                    Serial.print("Changing foreground effect pattern to: ");
                    Serial.println(playlistForeground[i].getCurrentPatternName());

                    playlistForeground[i].ms_previous = millis();
                    playlistForeground[i].fps_timer = millis();

                }

            }

            // -------- draw the next frame if fps timer dictates so --------
            //
            if ( 1000 / playlistForeground[i].pattern_fps + playlistForeground[i].last_frame < millis()) {

                playlistForeground[i].last_frame = millis();
                playlistForeground[i].pattern_fps = playlistForeground[i].drawFrame(i, CountPlaylistsForeground);

                if (!playlistForeground[i].pattern_fps) {

                    playlistForeground[i].pattern_fps = playlistForeground[i].default_fps;

                }

                ++playlistForeground[i].fps;
                playlistForeground[i].render_ms = millis() - playlistForeground[i].last_frame;

            }

            // ----- every 1000ms update fps and timer
            //
            if (playlistForeground[i].fps_timer + 1000 < millis()) {

                playlistForeground[i].fps_timer = millis();
                playlistForeground[i].fps_last = playlistForeground[i].fps;

                actual_fps = playlistForeground[i].fps;

                playlistForeground[i].fps = 0;

            }

        }

    }

    effects.updateBpmOscillators();

    actual_render_ms = millis() - start_render_ms;

    effects.ShowFrame();

    total_render_ms = millis() - start_render_ms;

    UpdateDiagnosticsData(); // put this at the end so it paints over everything else.

    // Serial.print("RAM: ");
    // Serial.println(ESP.getFreeHeap());

}

void listPatterns() {

    if (CountPlaylistsAudio > 0) {

        playlistAudio[0].listPatterns();

    }

}
