// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

uint32_t actual_render_ms = 0;
uint32_t start_render_ms = millis();
uint32_t total_render_ms = 0;
uint32_t actual_fps = 0;

void UpdateDiagnosticsData() {

#ifdef USE_HUB75

  // ------------------------ INFO --------------------------

  // ---------------- show total render time ----------------
  // show actual render time if over limit, 50 = 20fps
  if (option3ShowRenderTime || option1Diagnostics) {
    dma_display->setTextSize(1);

    // test - black out area behind text
    dma_display->setTextColor(BLACK);
    dma_display->setCursor(39,56);
    dma_display->print(actual_render_ms);
    dma_display->print("ms");
    dma_display->setCursor(41,56);
    dma_display->print(actual_render_ms);
    dma_display->print("ms");
    dma_display->setCursor(40,55);
    dma_display->print(actual_render_ms);
    dma_display->print("ms");
    dma_display->setCursor(40,57);
    dma_display->print(actual_render_ms);
    dma_display->print("ms");

    // display total render time
    dma_display->setTextColor(WHITE);
    if (actual_render_ms > 50) dma_display->setTextColor(YELLOW);
    dma_display->setCursor(40,56);
    dma_display->print(actual_render_ms);
    dma_display->print("ms");
  }

  // if WiFi has just connected, show IP address
  #ifdef USE_WIFI 
    if (wifiMessage > 0) {
      wifiMessage--;
      dma_display->setTextSize(1);
      dma_display->setTextColor(WHITE, BLUE);
      dma_display->setCursor(0,30);
      dma_display->print(WiFi.localIP()); 
    }
  #endif

  // BLINK BPM still testing bpm!
  if (effects.beatSawOsci8[0] > 200) {
    dma_display->drawPixelRGB888(MATRIX_WIDTH - 4, 2, 255, 255, 255);
    dma_display->drawPixelRGB888(MATRIX_WIDTH - 3, 2, 255, 255, 255);
    dma_display->drawPixelRGB888(MATRIX_WIDTH - 4, 3, 255, 255, 255);
    dma_display->drawPixelRGB888(MATRIX_WIDTH - 3, 3, 255, 255, 255);
  }

  // ---------------------- DIAGNOSTICS -------------------------

  if (option1Diagnostics) {

    dma_display->setTextSize(1);
    dma_display->setTextColor(WHITE);
    dma_display->setCursor(40,46);
    dma_display->print(total_render_ms);
    dma_display->print("ms");

    // actual fps
    //dma_display->setCursor(2,46);
    //dma_display->print(actual_fps);
    //dma_display->print("fps");

    // testing udp packet xfer rate
    //dma_display->setCursor(2,46);
    //dma_display->print(fftData.test1);
    //dma_display->print("pk");

    //testing serial packets per second
    dma_display->setCursor(2,46);
    dma_display->print(fftData.serial_pps);
    dma_display->print("pps");

    
    

    dma_display->setCursor(2,56);
    dma_display->print(fftData.bpm);
    dma_display->print("bpm");

  for (uint8_t i=0; i < maxPlaylistsInitialEffect; i++) {
      dma_display->setTextColor(WHITE);
      dma_display->setCursor(2,2+(i*10));
      dma_display->print(playlistInitialEffects[i].getCurrentPatternId());      //dma_display->print(patternsAudio[i].id3);                 // why not?
      dma_display->setTextColor(BLUE);
      if (playlistInitialEffects[i].render_ms > 10) dma_display->setTextColor(YELLOW);
      if (playlistInitialEffects[i].render_ms > 15) dma_display->setTextColor(MAGENTA);
      dma_display->print(playlistInitialEffects[i].render_ms);
      //dma_display->print(patternsInitEffects[i].fps_last);
  }
  for (uint8_t i=0; i < maxPlaylistsAudio; i++) {
      dma_display->setTextColor(WHITE);
      dma_display->setCursor(22,2+(i*10));
      dma_display->print(playlistAudio[i].getCurrentPatternId());      //dma_display->print(patternsAudio[i].id3);                 // why not?
      dma_display->setTextColor(BLUE);
      if (playlistAudio[i].render_ms > 10) dma_display->setTextColor(YELLOW);
      if (playlistAudio[i].render_ms > 15) dma_display->setTextColor(MAGENTA);
      dma_display->print(playlistAudio[i].render_ms);
      //dma_display->print(patternsAudio[i].fps_last);
  }
  for (uint8_t i=0; i < maxPlaylistsStatic; i++) {
      dma_display->setTextColor(WHITE);
      dma_display->setCursor(42,2+(i*10));
      dma_display->print(playlistStatic[i].getCurrentPatternId());      //dma_display->print(patternsAudio[i].id3);                 // why not?
      dma_display->setTextColor(BLUE);
      if (playlistStatic[i].render_ms > 10) dma_display->setTextColor(YELLOW);
      if (playlistStatic[i].render_ms > 15) dma_display->setTextColor(MAGENTA);
      dma_display->print(playlistStatic[i].render_ms);
      //dma_display->print(patternsStatic[i].fps_last);
  }

  for (uint8_t i=MATRIX_HEIGHT-10; i<MATRIX_HEIGHT-8; i++) {

    // saw wave
    dma_display->drawPixelRGB888(effects.beatSawOsciWidth[0], i, 0, 0, 128);          // blue - fast
    dma_display->drawPixelRGB888(effects.beatSawOsciWidth[1], i, 0, 128, 0);          // green
    dma_display->drawPixelRGB888(effects.beatSawOsciWidth[2], i, 128, 0, 0);          // red
    dma_display->drawPixelRGB888(effects.beatSawOsciWidth[3], i, 128, 128, 128);      // white - slow
    // sine wave
    dma_display->drawPixelRGB888(effects.beatSineOsciWidth[0], i-1, 0, 0, 128);
    dma_display->drawPixelRGB888(effects.beatSineOsciWidth[1], i-1, 0, 128, 0);
    dma_display->drawPixelRGB888(effects.beatSineOsciWidth[2], i-1, 128, 0, 0);
    dma_display->drawPixelRGB888(effects.beatSineOsciWidth[3], i-1, 128, 128, 128);   // white - slow
    // square wave
    dma_display->drawPixelRGB888(effects.beatSquareOsci8[0] / 4, i, 128, 0, 128);
 
  }

  // audio spectrum - what a mess
  for (uint8_t i=0; i < MATRIX_WIDTH; i++) {
      uint8_t height = map8(fftData.specData[i],0,15);
      dma_display->drawFastVLine(i, 36 + (8-height), height, GREEN);
  }
 
  /*
  static uint16_t offset = 0;

  for (uint8_t i=0; i < MATRIX_WIDTH; i++) {
      uint16_t x = i + offset;
      if (x >= MATRIX_WIDTH) {
        x = x - 64;
      }
      uint16_t color = RED;
      if (i+offset > 32) color = GREEN;
      if (i+offset > 64) color = CYAN;
      //color = i * 1024;   // 682 for 96
      uint8_t height = map8(fftData.specData[i+offset],0,16);
      dma_display->drawFastVLine(x, 36 + (8-height), height, color);
  }

  offset++;
  if (offset >= 32) {
    offset = offset - 32;
  }
  */



  }

#endif

#ifdef USE_LEDSTRIP

  // if WiFi has just connected, show IP address
  #ifdef USE_WIFI 
    if (wifiMessage > 0) {
      wifiMessage--;

      // TODO: write ip to matrix

    }
  #endif

#endif


}