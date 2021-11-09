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
*   Optional WiFi libraries needed:-
*
*   https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip
*   https://github.com/me-no-dev/AsyncTCP/archive/master.zip
*
*   These will need to be manually installed in your arduino libraries folder as
*   they are not available via the arduino library manager.
*
*   See WiFi tutorials at:
*   https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/
*
*/

// =================================================================================================
// == START basic user configuration ===============================================================
// =================================================================================================

// uncomment whichever display is relevent, hardware pins are defined later on if you need to change
#define USE_HUB75
//#define USE_TTGO_TFT
//#define USE_LEDSTRIP
//#define USE_TTGO_WATCH

// !! DO NOT change these if using a TTGO-T-Display or Watch (tested with T-WATCH-2020 V3)
#define PANEL_WIDTH 64                                      // not tested with anything other than single square HUB75_E 64x64 panel and one/two 64x32 panels, 128x64 breaks memory!
#define PANEL_HEIGHT 64                                     // matirx made from WS2812B led strips will work up to 32x32 (any larger is too currently slow)
#define PANELS_NUMBER 1                                     // number of chained HUB75 panels, working with just a single panel 64x64 or up to 2 64x32 panels at the moment, so set to 1 or 2

#define VERNO "13"      // if defined, checks the git repository for updates on startup, comment out if delay is annoying/not needed

// -- comment below lines to disable basic web sever interface and TCP comms interface, if not needed/used --
#define USE_WIFI
const char* ssid = "your_ssid";
const char* password = "your_password";
const char* hostPcIpAddress = "192.168.99.99";    // needed if using TCP comms
const uint16_t hostPcIpPort = 81;                 // needed if using TCP comms


// uncomment whichever fft input is relevent
#define USE_SERIAL_DATA
//#define USE_TCP_DATA


// very experimental (breaks memory or DMA):- uncomment if a connected INMP441 microphone will be used (define pins below)
//#define USE_INMP441_MICROPHONE
//#define USE_WATCH_MICROPHONE


// enable/disable debugging output over serial, comment if not used (not sorted yet)
#define DEBUG 1

// =================================================================================================
// == END user configuration =======================================================================
// =================================================================================================

// check to ensure user has uncommented display type definition above, otherwise through a complie error 
#ifndef USE_HUB75
#ifndef USE_TTGO_TFT
#ifndef USE_LEDSTRIP
#ifndef USE_TTGO_WATCH
#error --- uncomment whichever display is relevent to your setup ---
#endif 
#endif 
#endif
#endif

#include <FastLED.h>
#include <SPIFFS.h>

#ifdef USE_HUB75
  #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
  /*--------------------- HUB75 MATRIX GPIO CONFIG  ---------------------*/
  #define R1_PIN 25
  #define G1_PIN 26
  #define B1_PIN 27
  #define R2_PIN 14
  #define G2_PIN 12
  #define B2_PIN 13
  #define A_PIN 23
  #define B_PIN 19
  #define C_PIN 5
  #define D_PIN 17
  #define E_PIN 18
  #define LAT_PIN 4
  #define OE_PIN 15
  #define CLK_PIN 16
  MatrixPanel_I2S_DMA *dma_display = nullptr;
#endif


#ifdef USE_LEDSTRIP
  /*--------------------- LED STRIP GPIO ETC. CONFIG --------------------*/
  #define LEDSTRIP_RGB_PIN          18 // PWM capable
  #define LEDSTRIP_TYPE             WS2812B
  #define LEDSTRIP_COLOR_ORDER      GRB
  #define LEDSTRIP_BRIGHTNESS       255
  #define SERPENTINE                true
#endif

#ifdef USE_TTGO_TFT
  /*--------------------- LED STRIP GPIO ETC. CONFIG --------------------*/
  #include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
  #include <SPI.h>
  #define TFT_PANEL_WIDTH 240     // not used currently
  #define TFT_PANEL_HEIGHT 135    // not used currently
  #define TFT_PANEL_ROTATE 1
  #define TFT_PANEL_PIXELATE      // uncomment for HUB75 like pixelated render //todo
  TFT_eSPI *tft;
  TFT_eSPI t = TFT_eSPI();
#endif

#ifdef USE_TTGO_WATCH
  #define PANEL_WIDTH 60                  // over write defaults
  #define PANEL_HEIGHT 60
  #define LILYGO_WATCH_2020_V3
  #include <LilyGoWatch.h>
  TTGOClass *ttgo;
#endif


/*----------------------- MATRIX PANEL CONFIG -------------------------*/
#define MATRIX_WIDTH (PANEL_WIDTH * PANELS_NUMBER)          // not tested with anything other than square 64x64, 32x32 and 16x16
#define MATRIX_HEIGHT (PANEL_HEIGHT)
#define MATRIX_CENTER_X (MATRIX_WIDTH / 2)
#define MATRIX_CENTER_Y (MATRIX_HEIGHT / 2)

// moved from below for testing
#include "FftData.h"
FFTData fftData;

#ifdef USE_INMP441_MICROPHONE
  //#define I2S_WS  32       // aka LRCL     2, 32, 34 and 34(input) avaiable? when using HUB75
  //#define I2S_SD  2        // aka DOUT     was 34
  //#define I2S_SCK 33       // aka BCLK
  #include "audio_reactive.h"
#endif

#ifdef USE_WATCH_MICROPHONE
  #define I2S_WS  1          // aka LRCL     2, 32, 34 and 34(input) avaiable? when using HUB75
  #define I2S_SD  2        // aka DOUT     was 34
  #define I2S_SCK 0         // aka BCLK
  #include "audio_reactive.h"
#endif

// fixed maximums here for memory allocation, these must be >= variables used below
//#define MAX_PATTERNS_AMBIENT_BACKGROUND 1     // not used yet? useful? plasma effects?
#define MAX_PLAYLISTS_EFFECT 2                   // bluring/fading/sweeping effects
#define MAX_PLAYLISTS_AUDIO 4                    // audio re-active effects
#define MAX_PLAYLISTS_STATIC 4                   // standard non-audio animations inc. boids etc.


// (in future) limits may be applied in real-time by logic on how many patterns are being looped simultaneously, change these for quick testing
//static uint8_t maxPatternAmbient = 0;       // not implemented yet! for plasma effects, backgrounds, etc.
static uint8_t maxPlaylistsInitialEffect = 1;      // <------- 1 or 2 is reasonable
static uint8_t maxPlaylistsAudio = 2;           // <------- 2 or 3
static uint8_t maxPlaylistsStatic = 2;          // <------- 2 or 3
static uint8_t maxPlaylistsFinalEffect = 0;     // not used yet! you can try it, but can have a big impact on rendered output at the moment

// diagnostic options, these can be set via web interface when enabled
bool option1Diagnostics = false;
bool option2LockFps = true;
bool option3ShowRenderTime = false;
bool option4PauseCycling = false;
bool option5LianLi120Mode = false;
bool option6DisableInitialEffects = false;
bool option7DisableAudio = false;
bool option8DisableStatic = false;
bool option9DisableFinalEffects = false;        // not used
bool option10DisableCaleidoEffects = false;

//#include "FFTData.h"
//FFTData fftData;

#include "Geometry.h"  // Point
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

#ifdef USE_WIFI
  #include "ServerDiagnostics.h"
#endif

#ifdef USE_SERIAL_DATA
  #include "FftComClient.h"
#endif

#ifdef USE_TCP_DATA
  #include "FftTcpClient.h"
#endif

#include "Diagnostics.h"

//long restartCount = 0;    // for rebooting

// #################################################################################################################################
//  SETUP
// #################################################################################################################################
void setup()
{

  // serial
  Serial.begin(115200);
  delay(1000);     // allow comms to initilialise

  // very experimental
  #ifdef USE_INMP441_MICROPHONE
  // setup audio prior to anything else
    setupAudio();
    //setupNothing();
  #endif

// very experimental
  #ifdef USE_WATCH_MICROPHONE
  // setup audio prior to anything else
    setupAudio();
    //setupNothing();
  #endif



  #ifdef USE_HUB75
    HUB75_I2S_CFG mxconfig;
    mxconfig.mx_height = PANEL_HEIGHT;                // we have 64 pix height panels, as tested
    mxconfig.mx_width = PANEL_WIDTH;                  // we have 64 pix height panels, as tested
    mxconfig.chain_length = PANELS_NUMBER;            // we have x panels chained
    mxconfig.gpio.e = E_PIN;                          // we MUST assign pin e to some free pin on a board to drive 64 pix height panels with 1/32 scan
    mxconfig.clkphase = false;                        // change this if you have issues with ghosting.
    mxconfig.driver = HUB75_I2S_CFG::FM6126A;         // in case that we don't use panels based on FM6126A chip, we can change that

    // test display
    Serial.println("Starting HUB75 Display...");
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
    dma_display->setBrightness8(128); //0-255   // 150 is good for me
    dma_display->fillScreenRGB888(255,0,0);
    delay(150);
    dma_display->fillScreenRGB888(0,255,0);
    delay(150);
    dma_display->fillScreenRGB888(0,0,255);
    delay(150);
    dma_display->fillScreenRGB888(128,128,128);
    delay(150);
    dma_display->clearScreen();  
  #endif

  #ifdef USE_LEDSTRIP
    // test display, WS2812B led matrix/strip
    Serial.println("Starting LED Strip...");
    FastLED.addLeds<LEDSTRIP_TYPE, LEDSTRIP_RGB_PIN, LEDSTRIP_COLOR_ORDER>(effects.leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LEDSTRIP_BRIGHTNESS);
    fill_solid(effects.leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
    delay(250);
    fill_solid(effects.leds, NUM_LEDS, CRGB::Green);
    FastLED.show();
    delay(250);
    fill_solid(effects.leds, NUM_LEDS, CRGB::Blue);
    FastLED.show();
    delay(250);
    fill_solid(effects.leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    delay(250);
    fill_solid(effects.leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  #endif

  #ifdef USE_TTGO_TFT
    // test display
    tft = &t;
    Serial.println("Starting TFT Display...");
    tft->init();
    tft->setRotation(TFT_PANEL_ROTATE);
    tft->fillScreen(tft->color565(255,0,0));
    delay(250);
    tft->fillScreen(tft->color565(0,255,0));
    delay(250);
    tft->fillScreen(tft->color565(0,0,255));
    delay(250);
    tft->fillScreen(tft->color565(255,255,255));
    delay(250);
    tft->fillScreen(tft->color565(0,0,0));
  #endif


  #ifdef USE_TTGO_WATCH
    Serial.println("Starting Watch Display...");
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    ttgo->tft->fillScreen(ttgo->tft->color565(0,0,0));
    ttgo->tft->setTextFont(2);
  #endif

  // mount filesystem
  if(!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    #ifdef USE_HUB75
      dma_display->setTextColor(255);
      dma_display->setCursor(2,2);
      dma_display->print("SPIFFS Error"); 
      delay(1000);
    #endif
  }

  // Connect to Wi-Fi
  #ifdef USE_WIFI
    Serial.print("Connecting to WiFi SSID - ");
    Serial.print(ssid);
    Serial.println("...");
    #ifdef USE_HUB75
      dma_display->setTextColor(65535);
      dma_display->setCursor(2,2);
      dma_display->print("Connecting to WiFi.."); 
      dma_display->setCursor(2,32);
      dma_display->print(ssid);
    #endif 
    #ifdef USE_TTGO_TFT
      tft->setTextSize(2);
      tft->setTextColor(0xffff);
      tft->setCursor(24,12);
      tft->print("Connecting.."); 
      tft->setCursor(24,32);
      tft->print(ssid);
    #endif 
    #ifdef USE_TTGO_WATCH
      ttgo->tft->setTextSize(2);
      ttgo->tft->setTextColor(0xffff);
      ttgo->tft->setCursor(4,12);
      ttgo->tft->print("Connecting.."); 
      ttgo->tft->setCursor(4,36);
      ttgo->tft->print(ssid);
    #endif 
    WiFi.begin(ssid, password);
    delay(1000);
  #else
    option1Diagnostics = false;
    option3ShowRenderTime = false;
  #endif

  #ifdef USE_TCP_DATA
    Serial.println("Starting TCP Client...");
    setupTCPClient();
  #endif

  #ifdef USE_SERIAL_DATA
    Serial.println("Starting Serial Comms Client...");
    setupCOMClient();
  #endif


  Serial.println("Starting AuroraDrop Demo...");

  // setup the effects and noise generator
  effects.Setup();
  
  // TODO: redo this
  Serial.println("Effects being loaded: ");
  listPatterns();

  // initialise all the initial effects patterns
  for (uint8_t i=0; i < MAX_PLAYLISTS_EFFECT; i++)
  {
    playlistInitialEffects[i].default_fps = 90;
    playlistInitialEffects[i].pattern_fps = 90;
    playlistInitialEffects[i].ms_animation_max_duration = 5000;
    do {
      playlistInitialEffects[i].moveRandom(1, i);
    }
    while (!playlistInitialEffects[i].getCurrentItemEnabled());
    Serial.print("Starting with intitial effects pattern: ");
    Serial.println(playlistInitialEffects[i].getCurrentPatternName());
    playlistInitialEffects[i].start(i);
    playlistInitialEffects[i].ms_previous = millis();
    playlistInitialEffects[i].fps_timer = millis();
  }

  // initialise all the audio effects patterns
  for (uint8_t i=0; i < MAX_PLAYLISTS_AUDIO; i++)
  {
    playlistAudio[i].default_fps = 90;
    playlistAudio[i].pattern_fps = 90;
    playlistAudio[i].ms_animation_max_duration = 5000;          // 5000ms is good, but should be bpm related
    do {
      playlistAudio[i].moveRandom(1, i);
    }
    while (!playlistAudio[i].getCurrentItemEnabled());
    Serial.print("Starting with static effects pattern: ");
    Serial.println(playlistAudio[i].getCurrentPatternName());
    playlistAudio[i].start(i);
    playlistAudio[i].ms_previous = millis();
    playlistAudio[i].fps_timer = millis();
  }

  // initialise all the static/non-audio effects patterns
  for (uint8_t i=0; i < MAX_PLAYLISTS_STATIC; i++)
  {
    playlistStatic[i].default_fps = 90;
    playlistStatic[i].pattern_fps = 90;
    playlistStatic[i].ms_animation_max_duration = 10000;  // 60000, 1 minute is good to give effects time to do stuff
    do {
      playlistStatic[i].moveRandom(1, i);
    }
    while (!playlistStatic[i].getCurrentItemEnabled());
    Serial.print("Starting with static effects pattern: ");
    Serial.println(playlistStatic[i].getCurrentPatternName());
    playlistStatic[i].start(i);
    playlistStatic[i].ms_previous = millis();
    playlistStatic[i].fps_timer = millis();
  }

  // NOT CURRENTLY USED
  // initialise all the final effects patterns
  for (uint8_t i=0; i < maxPlaylistsFinalEffect; i++)
  {
    playlistFinalEffects[i].default_fps = 90;
    playlistFinalEffects[i].pattern_fps = 90;
    playlistFinalEffects[i].ms_animation_max_duration = 5000;
    do {
      playlistFinalEffects[i].moveRandom(1, i);
    }
    while (!playlistFinalEffects[i].getCurrentItemEnabled());
    Serial.print("Starting with intitial effects pattern: ");
    Serial.println(playlistFinalEffects[i].getCurrentPatternName());
    playlistFinalEffects[i].start(i);
    playlistFinalEffects[i].ms_previous = millis();
    playlistFinalEffects[i].fps_timer = millis();
  }

  Xlast_render_ms = millis();

}

// #################################################################################################################################
//  LOOP
// #################################################################################################################################
void loop()
{


  // record start time
  start_render_ms = millis();

  // show diagnostics info if optioned
  UpdateDiagnosticsData();


  // check if WiFi has connected yet
  #ifdef USE_WIFI
    checkWifiStatus();
  #endif

  // check if there is any new audio data from serial port and proccess it to useful bins and flags used by the patterns
  // todo: move this to core 0?
  #ifdef USE_SERIAL_DATA
    //fftData.processSerialData();
  #endif

  // TODO: implement aurora demo menu system?
  // menu.run(mainMenuItems, mainMenuItemCount);  


  // check here if we are ready to render the next frame or not
  if (option2LockFps && millis() < Xlast_render_ms + 50)    // 40=25fps, 50=20fps;
    return;

  Xlast_render_ms = millis();

  if (maxPlaylistsInitialEffect==0 || option6DisableInitialEffects) effects.DimAll(230);       // if we have no effects enabled, dim screen by small amount (e.g. during testing)

  // clear counters/flags for psuedo randomness workings inside pattern setup and drawing
  PatternsAudioMainEffectCount = 0;
  PatternsAudioBackdropCount = 0;
  PatternsAudioCaleidoscopeCount = 0;
  PatternsAudioBluringCount = 0;


  // -------------- loop through, and apply each of the initial effects patterns ----------------

  if (!option6DisableInitialEffects) {
  for (uint8_t i=0; i < maxPlaylistsInitialEffect; i++)
  {
    // -------- start next animation if max duration reached --------
    if ( (millis() - playlistInitialEffects[i].ms_previous) > playlistInitialEffects[i].ms_animation_max_duration) 
    {
      if (!option4PauseCycling) {
        playlistInitialEffects[i].stop();      
        do {
          playlistInitialEffects[i].moveRandom(1, i);
        }
        while (!playlistInitialEffects[i].getCurrentItemEnabled());
        // patterns.move(1);
        playlistInitialEffects[i].start(i);  
        Serial.print("Changing initial effects pattern X  to:  ");
        Serial.println(playlistInitialEffects[i].getCurrentPatternName());
        playlistInitialEffects[i].ms_previous = millis();
        playlistInitialEffects[i].fps_timer = millis();
      }
    }
    // -------- draw the next frame if fps timer dictates so --------
    if ( 1000 / playlistInitialEffects[i].pattern_fps + playlistInitialEffects[i].last_frame < millis())
    {
      playlistInitialEffects[i].last_frame = millis();
      playlistInitialEffects[i].pattern_fps = playlistInitialEffects[i].drawFrame(i, maxPlaylistsInitialEffect);
      if (!playlistInitialEffects[i].pattern_fps)
        playlistInitialEffects[i].pattern_fps = playlistInitialEffects[i].default_fps;
      ++playlistInitialEffects[i].fps;
      playlistInitialEffects[i].render_ms = millis() - playlistInitialEffects[i].last_frame;
    }
    // ----- every 1000ms update fps and timer
    if (playlistInitialEffects[i].fps_timer + 1000 < millis()){
      //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);
      playlistInitialEffects[i].fps_timer = millis();
      playlistInitialEffects[i].fps_last = playlistInitialEffects[i].fps;
      actual_fps = playlistInitialEffects[i].fps;
      playlistInitialEffects[i].fps = 0;
    }
  }
  }


  // -------------- loop through, and apply each of the audio re-active patterns ----------------
  if (!option7DisableAudio) {
  for (uint8_t i=0; i < maxPlaylistsAudio; i++)
  {
    // -------- start next animation if max duration reached --------
    if ( (millis() - playlistAudio[i].ms_previous) > playlistAudio[i].ms_animation_max_duration) 
    {
      if (!option4PauseCycling) {
        playlistAudio[i].stop();      
        do {
          playlistAudio[i].moveRandom(1, i);
        }
        while (!playlistAudio[i].getCurrentItemEnabled());
        //patterns.move(1);
        playlistAudio[i].start(i);  
        Serial.print("Changing audio pattern X to:  ");
        Serial.println(playlistAudio[i].getCurrentPatternName());
        playlistAudio[i].ms_previous = millis();
        playlistAudio[i].fps_timer = millis();
        // select a random palette when ANY of the audio patterns start/re-start, this can look funky when/if they start changing out out sync
        // TODO: consider randomly locking palette change to only when the first pattern of the group re-starts (or maybe also when an initial effect restarts)
        // possible bug? if we change the palette when the palette is currently transitioning to another, the palette changes abruptly?
        effects.RandomPalette();
      }
    }
    // -------- draw the next frame if fps timer dictates so --------
    if ( 1000 / playlistAudio[i].pattern_fps + playlistAudio[i].last_frame < millis())
    {
      playlistAudio[i].last_frame = millis();
      playlistAudio[i].pattern_fps = playlistAudio[i].drawFrame(i, maxPlaylistsAudio);
      if (!playlistAudio[i].pattern_fps)
        playlistAudio[i].pattern_fps = playlistAudio[i].default_fps;
      ++playlistAudio[i].fps;
      playlistAudio[i].render_ms = millis() - playlistAudio[i].last_frame;
    }
    // ----- every 1000ms update fps and timer
    if (playlistAudio[i].fps_timer + 1000 < millis()){
      //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);


      // fillScreen(dma_display->color565(128, 0, 0));
      playlistAudio[i].fps_timer = millis();
      playlistAudio[i].fps_last = playlistAudio[i].fps;
      actual_fps = playlistAudio[i].fps;
      playlistAudio[i].fps = 0;
    }
  }
  }



  // ---------- loop through, and apply each of the audio static (mostly) patterns, e.g. boids --------
  if (!option8DisableStatic) {
  for (uint8_t i=0; i < maxPlaylistsStatic; i++)
  {
    // -------- start next animation if max duration reached --------
    if ( (millis() - playlistStatic[i].ms_previous) > playlistStatic[i].ms_animation_max_duration) 
    {
      if (!option4PauseCycling) {
        playlistStatic[i].stop();      
        do {
          playlistStatic[i].moveRandom(1, i);
        }
        while (!playlistStatic[i].getCurrentItemEnabled());
        //patterns.move(1);
        playlistStatic[i].start(i);  
        Serial.print("Changing statix pattern X to:  ");
        Serial.println(playlistStatic[i].getCurrentPatternName());
        playlistStatic[i].ms_previous = millis();
        playlistStatic[i].fps_timer = millis();
      }
    }
    // -------- draw the next frame if fps timer dictates so --------
    if (1000 / playlistStatic[i].pattern_fps + playlistStatic[i].last_frame < millis())
    {
      playlistStatic[i].last_frame = millis();
      playlistStatic[i].pattern_fps = playlistStatic[i].drawFrame(i, maxPlaylistsStatic);
      if (!playlistStatic[i].pattern_fps)
        playlistStatic[i].pattern_fps = playlistStatic[i].default_fps;
      ++playlistStatic[i].fps;
      playlistStatic[i].render_ms = millis() - playlistStatic[i].last_frame;
    }
    // ----- every 1000ms update fps and timer
    if (playlistStatic[i].fps_timer + 1000 < millis()){
      //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);
      playlistStatic[i].fps_timer = millis();
      playlistStatic[i].fps_last = playlistStatic[i].fps;
      actual_fps = playlistStatic[i].fps;
      playlistStatic[i].fps = 0;
    }
  }
  }


  // apply final set of effects   ??? WE NEED BETTER POST EFFECTS RANDONISM ???
  if (!option9DisableFinalEffects) {
    for (uint8_t i=0; i < maxPlaylistsFinalEffect; i++)
    {
      // #-------- start next animation if maxduration reached --------#
      if ( (millis() - playlistFinalEffects[i].ms_previous) > playlistFinalEffects[i].ms_animation_max_duration) 
      {
        if (!option4PauseCycling) {
          playlistFinalEffects[i].stop();
          do {
            playlistFinalEffects[i].moveRandom(1, i);
          }
          while (!playlistFinalEffects[i].getCurrentItemEnabled());
          //patterns.move(1);
          playlistFinalEffects[i].start(i);  
          Serial.print("Changing final effect pattern X to:  ");
          Serial.println(playlistFinalEffects[i].getCurrentPatternName());
          playlistFinalEffects[i].ms_previous = millis();
          playlistFinalEffects[i].fps_timer = millis();
        }
      }
      // -------- draw the next frame if fps timer dictates so --------
      if (1000 / playlistFinalEffects[i].pattern_fps + playlistFinalEffects[i].last_frame < millis())
      {
        playlistFinalEffects[i].last_frame = millis();
        playlistFinalEffects[i].pattern_fps = playlistFinalEffects[i].drawFrame(i, maxPlaylistsFinalEffect);
        if (!playlistFinalEffects[i].pattern_fps)
          playlistFinalEffects[i].pattern_fps = playlistFinalEffects[i].default_fps;
        ++playlistFinalEffects[i].fps;
        playlistFinalEffects[i].render_ms = millis() - playlistStatic[i].last_frame;
      }

      if (playlistFinalEffects[i].fps_timer + 1000 < millis()){
        //Serial.printf_P(PSTR("Effect fps: %ld\n"), fps[i]);
        playlistFinalEffects[i].fps_timer = millis();
        playlistFinalEffects[i].fps_last = playlistFinalEffects[i].fps;
        actual_fps = playlistFinalEffects[i].fps;
        playlistFinalEffects[i].fps = 0;
      }
    }
  }


  // still tweaking bpm oscillators
  effects.updateBpmOscillators();
  // effects.MoveOscillators();     // old aurora




  actual_render_ms = millis() - start_render_ms;
  // if optioned wait the full XXms before we render


  total_render_ms = millis() - start_render_ms;



  // seriously dim anything left rendering on the panel if there is no audio
  //if(fftData.noAudio) effects.DimAll(50);

  if (option5LianLi120Mode) {
    effects.ShowLianLi120();
  }

  effects.ShowFrame();

/*
  #ifdef USE_HUB75
    // update the panel frame with the final effects rendered frame
    effects.ShowFrame();
  #endif

  // ledstrip
  #ifdef USE_LEDSTRIP
    // you don't need to do both
    #ifndef USE_HUB75
      FastLED.show();
    #endif
  #endif

  #ifdef USE_TTGO_TFT
    // update the panel frame with the final effects rendered frame
    effects.ShowFrame();
  #endif
*/

  // ----------------------- end ---------------------------

}

void listPatterns() {
  // TODO:  working functionality
  if (maxPlaylistsAudio > 0) {
    playlistAudio[0].listPatterns();
  }
}
