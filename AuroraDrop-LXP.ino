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
*   ESP32 HUB75 LED MATRIX PANEL DMA Display (tested with v2.0.6)
*   https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA
*
*
*/

// =================================================================================================
// == START basic user configuration ===============================================================
// =================================================================================================

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 64               
#define PANELS_NUMBER 2

int GLOBAL_BRIGHTNESS = 128;    //0-255 - this gets overridden with the ADC value if BRIGHT_PIN is defined - otherwise it stays here.

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#define PRODUCTION_BOARD

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
    #define ONBOARD_RGB_LED 48

    // Brightness ADC pin
    //
    #define BRIGHT_PIN 1

#else

    #pragma message("HUB75 - Testbed Board Pinout")

    // Testbed Board

    // HUB75 Pins
    //
    #define R1_PIN 4
    #define G1_PIN 5
    #define B1_PIN 6
    // placeholder GND - 7
    #define R2_PIN 8
    #define G2_PIN 9
    #define B2_PIN 10
    #define E_PIN 11
    #define A_PIN 12
    #define B_PIN 13
    #define C_PIN 14
    #define D_PIN 21 // GND on silkscreen, but working example is assigned to a pin tho where "D" should be on the HUB75 connector, after C
    #define CLK_PIN 47
    #define LAT_PIN 48
    #define OE_PIN 35
    // placeholder GND - G

    // INMP441 Pins
    //
    #define I2S_WS  1
    #define I2S_SD  2
    #define I2S_SCK 17

    // ESP32-S3 Devkit C/M have these
    //
    #define ONBOARD_RGB_LED 48

#endif

#ifdef ONBOARD_RGB_LED

    // If it's present and not initialized, it can behave randonly...

    #include <Adafruit_NeoPixel.h>
    Adafruit_NeoPixel pixels(1, ONBOARD_RGB_LED, NEO_GRB + NEO_KHZ800); // I use this as an FPS indicator in Diagnostics.h

#endif

MatrixPanel_I2S_DMA *dma_display = nullptr;

#include <FastLED.h>

#define MATRIX_WIDTH (PANEL_WIDTH * PANELS_NUMBER)          // works with 1x and 2x 632x64 and 64x32 panels in landscape
#define MATRIX_HEIGHT (PANEL_HEIGHT)                        // doesn't seem to making "tall" portrait arrays - yet.
#define MATRIX_CENTER_X (MATRIX_WIDTH / 2)
#define MATRIX_CENTER_Y (MATRIX_HEIGHT / 2)

#include "FftData.h"
FFTData fftData;

#include "FftMic.h"

// fixed maximums here for memory allocation, these must be >= variables used below
//
//#define MAX_PATTERNS_AMBIENT_BACKGROUND 1      // not used yet? useful? plasma effects?
#define MAX_PLAYLISTS_EFFECT 2                   // bluring/fading/sweeping effects
#define MAX_PLAYLISTS_AUDIO 2                    // audio re-active effects
#define MAX_PLAYLISTS_STATIC 2                   // standard non-audio animations inc. boids etc.
#define MAX_PLAYLISTS_FINAL_EFFECT 0             // This just adds another "initial effect" layer. Not really needed.

// (in future) limits may be applied in real-time by logic on how many patterns are being looped simultaneously, change these for quick testing
//
//static uint8_t maxPatternAmbient = 0;                                    // not implemented yet! for plasma effects, backgrounds, etc.
static uint8_t CountPlaylistsInitialEffect = MAX_PLAYLISTS_EFFECT;         // <------- 1 or 2 is reasonable
static uint8_t CountPlaylistsAudio = MAX_PLAYLISTS_AUDIO;                  // <------- 2 or 3
static uint8_t CountPlaylistsStatic = MAX_PLAYLISTS_STATIC;                // <------- 2 or 3
static uint8_t CountPlaylistsFinalEffect = MAX_PLAYLISTS_FINAL_EFFECT;     // not used yet! you can try it, but can have a big impact on rendered output at the moment

// diagnostic options, these can be set via web interface when enabled
//
bool option1Diagnostics = false;
bool option2LockFps = false;
bool option3ShowRenderTime = false;
bool option4PauseCycling = false;
bool option5LianLi120Mode = false;
bool option6DisableInitialEffects = false;
bool option7DisableAudio = false;
bool option8DisableStatic = false;
bool option9DisableFinalEffects = false;        // not used
bool option10DisableCaleidoEffects = false;

#include "Geometry.h"
#include "Effects.h"
Effects effects;
#include "Drawable.h"
#include "Playlist.h"

#include "Vector.h"
#include "Boid.h"
#include "Attractor.h"

#include "Playlist_InitialEffects.h"
#include "Playlist_Audio.h"
#include "Playlist_Static.h"

Playlist_InitialEffects playlistInitialEffects[MAX_PLAYLISTS_EFFECT];
Playlist_Audio playlistAudio[MAX_PLAYLISTS_AUDIO];
Platlist_Static playlistStatic[MAX_PLAYLISTS_STATIC];
Playlist_InitialEffects playlistFinalEffects[MAX_PLAYLISTS_EFFECT];

// TODO: sort? useful or not
static uint8_t PatternsAudioMainEffectCount = 0;
static uint8_t PatternsAudioBackdropCount = 0;
static uint8_t PatternsAudioCaleidoscopeCount = 0;
static uint16_t PatternsAudioBluringCount = 0;

uint32_t startMillis = millis();
uint32_t Xlast_render_ms = millis();

#include "Diagnostics.h"

void setup() {
 
    Serial.begin(115200);
    delay(1000);

    Serial.printf("ESP32 IDF version: %s\n",IDF_VER);

    #ifdef ONBOARD_RGB_LED

        pixels.begin();
        pixels.setBrightness(10);
        pixels.clear();
        pixels.show();

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
    mxconfig.driver = HUB75_I2S_CFG::FM6126A;         // my panels (with D or E pins) have needed this.

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
    for (uint8_t i=0; i < MAX_PLAYLISTS_EFFECT; i++) {

        playlistInitialEffects[i].default_fps = 90;
        playlistInitialEffects[i].pattern_fps = 90;
        playlistInitialEffects[i].ms_animation_max_duration = animation_duration;

        do {
            playlistInitialEffects[i].moveRandom(1, i);
        } while (!playlistInitialEffects[i].getCurrentItemEnabled());

        Serial.print("Starting with intitial effects pattern: ");
        Serial.println(playlistInitialEffects[i].getCurrentPatternName());

        playlistInitialEffects[i].start(i);
        playlistInitialEffects[i].ms_previous = millis();
        playlistInitialEffects[i].fps_timer = millis();

        // TESTING: enable all the effects
        for (uint8_t j=0; j < playlistInitialEffects[i].getPatternCount(); j++) {
            playlistInitialEffects[i].setItemEnabled(j, 1);
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

    // NOT CURRENTLY USED
    // initialise all the final effects patterns
    //
    for (uint8_t i=0; i < CountPlaylistsFinalEffect; i++) {

        playlistFinalEffects[i].default_fps = 90;
        playlistFinalEffects[i].pattern_fps = 90;
        playlistFinalEffects[i].ms_animation_max_duration = animation_duration;

        do {
            playlistFinalEffects[i].moveRandom(1, i);
        } while (!playlistFinalEffects[i].getCurrentItemEnabled());

        Serial.print("Starting with intitial effects pattern: ");
        Serial.println(playlistFinalEffects[i].getCurrentPatternName());

        playlistFinalEffects[i].start(i);
        playlistFinalEffects[i].ms_previous = millis();
        playlistFinalEffects[i].fps_timer = millis();

        // TESTING: enable all the effects
        for (uint8_t j=0; j < playlistFinalEffects[i].getPatternCount(); j++) {
            playlistFinalEffects[i].setItemEnabled(j, 1);
        }

    }

    Xlast_render_ms = millis();

    #ifdef BRIGHT_PIN

        analogReadResolution(8);

        GLOBAL_BRIGHTNESS = analogRead(BRIGHT_PIN);
    
    #endif

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

    start_render_ms = millis();

    UpdateDiagnosticsData();

    // check here if we are ready to render the next frame or not
    //
    if (option2LockFps && millis() < Xlast_render_ms + 50) return;  // 40=25fps, 50=20fps;

    Xlast_render_ms = millis();

    if (CountPlaylistsInitialEffect==0 || option6DisableInitialEffects) effects.DimAll(230);       // if we have no effects enabled, dim screen by small amount (e.g. during testing)

    // clear counters/flags for psuedo randomness workings inside pattern setup and drawing
    //
    PatternsAudioMainEffectCount = 0;
    PatternsAudioBackdropCount = 0;
    PatternsAudioCaleidoscopeCount = 0;
    PatternsAudioBluringCount = 0;

    // -------------- loop through, and apply each of the initial effects patterns ----------------
    //
    if (!option6DisableInitialEffects) {

        for (uint8_t i=0; i < CountPlaylistsInitialEffect; i++) {

            // -------- start next animation if max duration reached --------
            //
            if ( (millis() - playlistInitialEffects[i].ms_previous) > playlistInitialEffects[i].ms_animation_max_duration) {

                if (!option4PauseCycling) {

                    playlistInitialEffects[i].stop();      

                    do {
                        playlistInitialEffects[i].moveRandom(1, i);
                    } while (!playlistInitialEffects[i].getCurrentItemEnabled());

                    playlistInitialEffects[i].ms_animation_max_duration = animation_duration;
                    playlistInitialEffects[i].start(i); 

                    Serial.print("Changing initial effects pattern to: ");
                    Serial.println(playlistInitialEffects[i].getCurrentPatternName());

                    playlistInitialEffects[i].ms_previous = millis();
                    playlistInitialEffects[i].fps_timer = millis();

                }

            }

            // -------- draw the next frame if fps timer dictates so --------
            //
            if ( 1000 / playlistInitialEffects[i].pattern_fps + playlistInitialEffects[i].last_frame < millis()) {

                playlistInitialEffects[i].last_frame = millis();
                playlistInitialEffects[i].pattern_fps = playlistInitialEffects[i].drawFrame(i, CountPlaylistsInitialEffect);

                if (!playlistInitialEffects[i].pattern_fps) {

                    playlistInitialEffects[i].pattern_fps = playlistInitialEffects[i].default_fps;

                }

                ++playlistInitialEffects[i].fps;
                playlistInitialEffects[i].render_ms = millis() - playlistInitialEffects[i].last_frame;

            }

            // ----- every 1000ms update fps and timer
            //
            if (playlistInitialEffects[i].fps_timer + 1000 < millis()) {

                playlistInitialEffects[i].fps_timer = millis();
                playlistInitialEffects[i].fps_last = playlistInitialEffects[i].fps;

                actual_fps = playlistInitialEffects[i].fps;

                playlistInitialEffects[i].fps = 0;

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

    // apply final set of effects
    //
    if (!option9DisableFinalEffects) {

        for (uint8_t i=0; i < CountPlaylistsFinalEffect; i++) {

            // #-------- start next animation if maxduration reached --------#
            //
            if ( (millis() - playlistFinalEffects[i].ms_previous) > playlistFinalEffects[i].ms_animation_max_duration) {

                if (!option4PauseCycling) {

                    playlistFinalEffects[i].stop();

                    do {
                        playlistFinalEffects[i].moveRandom(1, i);
                    } while (!playlistFinalEffects[i].getCurrentItemEnabled());

                    playlistFinalEffects[i].ms_animation_max_duration = animation_duration;
                    playlistFinalEffects[i].start(i);  

                    Serial.print("Changing final effect pattern to: ");
                    Serial.println(playlistFinalEffects[i].getCurrentPatternName());

                    playlistFinalEffects[i].ms_previous = millis();
                    playlistFinalEffects[i].fps_timer = millis();

                }

            }

            // -------- draw the next frame if fps timer dictates so --------
            //
            if (1000 / playlistFinalEffects[i].pattern_fps + playlistFinalEffects[i].last_frame < millis()) {

                playlistFinalEffects[i].last_frame = millis();
                playlistFinalEffects[i].pattern_fps = playlistFinalEffects[i].drawFrame(i, CountPlaylistsFinalEffect);

                if (!playlistFinalEffects[i].pattern_fps) {

                    playlistFinalEffects[i].pattern_fps = playlistFinalEffects[i].default_fps;

                }

                ++playlistFinalEffects[i].fps;
                playlistFinalEffects[i].render_ms = millis() - playlistStatic[i].last_frame;

            }

            if (playlistFinalEffects[i].fps_timer + 1000 < millis()){

                playlistFinalEffects[i].fps_timer = millis();
                playlistFinalEffects[i].fps_last = playlistFinalEffects[i].fps;

                actual_fps = playlistFinalEffects[i].fps;

                playlistFinalEffects[i].fps = 0;

            }

        }   

    }

    effects.updateBpmOscillators();

    actual_render_ms = millis() - start_render_ms;

    if (option5LianLi120Mode) {

        effects.ShowLianLi120();

    }

    effects.ShowFrame();

    total_render_ms = millis() - start_render_ms;

}

void listPatterns() {

    if (CountPlaylistsAudio > 0) {

        playlistAudio[0].listPatterns();

    }

}
