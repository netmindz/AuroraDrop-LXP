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

  // ---------------------- DIAGNOSTICS -------------------------

  if (option1Diagnostics) {

    dma_display->setTextSize(1);
    dma_display->setTextColor(WHITE);
    dma_display->setCursor(40,46);
    dma_display->print(total_render_ms);
    dma_display->print("ms");

    dma_display->setCursor(2,46);
    dma_display->print(actual_fps);
    dma_display->print("fps");

    dma_display->setCursor(2,56);
    dma_display->print(fftData.bpm);
    dma_display->print("bpm");

  for (uint8_t i=0; i < maxPatternInitEffect; i++) {
      dma_display->setTextColor(WHITE);
      dma_display->setCursor(2,2+(i*10));
      dma_display->print(patternsInitEffects[i].getCurrentPatternId());      //dma_display->print(patternsAudio[i].id3);                 // why not?
      dma_display->setTextColor(BLUE);
      if (patternsInitEffects[i].render_ms > 10) dma_display->setTextColor(YELLOW);
      if (patternsInitEffects[i].render_ms > 15) dma_display->setTextColor(MAGENTA);
      dma_display->print(patternsInitEffects[i].render_ms);
      //dma_display->print(patternsInitEffects[i].fps_last);
  }
  for (uint8_t i=0; i < maxPatternAudio; i++) {
      dma_display->setTextColor(WHITE);
      dma_display->setCursor(22,2+(i*10));
      dma_display->print(patternsAudio[i].getCurrentPatternId());      //dma_display->print(patternsAudio[i].id3);                 // why not?
      dma_display->setTextColor(BLUE);
      if (patternsAudio[i].render_ms > 10) dma_display->setTextColor(YELLOW);
      if (patternsAudio[i].render_ms > 15) dma_display->setTextColor(MAGENTA);
      dma_display->print(patternsAudio[i].render_ms);
      //dma_display->print(patternsAudio[i].fps_last);
  }
  for (uint8_t i=0; i < maxPatternStatic; i++) {
      dma_display->setTextColor(WHITE);
      dma_display->setCursor(42,2+(i*10));
      dma_display->print(patternsStatic[i].getCurrentPatternId());      //dma_display->print(patternsAudio[i].id3);                 // why not?
      dma_display->setTextColor(BLUE);
      if (patternsStatic[i].render_ms > 10) dma_display->setTextColor(YELLOW);
      if (patternsStatic[i].render_ms > 15) dma_display->setTextColor(MAGENTA);
      dma_display->print(patternsStatic[i].render_ms);
      //dma_display->print(patternsStatic[i].fps_last);
  }

  for (uint8_t i=MATRIX_HEIGHT-22; i<MATRIX_HEIGHT-19; i++) {

    // saw wave
    dma_display->drawPixelRGB888(effects.beatSawOsciWidth[0], i, 0, 0, 128);          // blue - fast
    dma_display->drawPixelRGB888(effects.beatSawOsciWidth[1], i, 0, 128, 0);          // green
    dma_display->drawPixelRGB888(effects.beatSawOsciWidth[2], i, 128, 0, 0);          // red
    dma_display->drawPixelRGB888(effects.beatSawOsciWidth[3], i, 128, 128, 128);      // white - slow
    // sine wave
    dma_display->drawPixelRGB888(effects.beatSineOsciWidth[0], i-4, 0, 0, 128);
    dma_display->drawPixelRGB888(effects.beatSineOsciWidth[1], i-4, 0, 128, 0);
    dma_display->drawPixelRGB888(effects.beatSineOsciWidth[2], i-4, 128, 0, 0);
    dma_display->drawPixelRGB888(effects.beatSineOsciWidth[3], i-4, 128, 128, 128);   // white - slow
    // square wave
    dma_display->drawPixelRGB888(effects.beatSquareOsci8[0] / 4, i, 128, 0, 128);
 
  }



  }

}