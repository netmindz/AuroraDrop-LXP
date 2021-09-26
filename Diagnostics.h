

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
    dma_display->setTextColor(65535);
    if (actual_render_ms > 50) dma_display->setTextColor(65280);
    dma_display->setCursor(40,56);
    dma_display->print(actual_render_ms);
    dma_display->print("ms");
  }

  // if WiFi has just connected, show IP address
  #ifdef USE_WIFI 
    if (wifiMessage > 0) {
      wifiMessage--;
      dma_display->setTextSize(1);
      dma_display->setTextColor(65535, 255);
      dma_display->setCursor(0,30);
      dma_display->print(WiFi.localIP()); 
    }
  #endif

  // ---------------------- DIAGNOSTICS -------------------------

  if (option1Diagnostics) {

    dma_display->setTextSize(1);
    dma_display->setTextColor(65535);
    dma_display->setCursor(40,46);
    dma_display->print(total_render_ms);
    dma_display->print("ms");

    dma_display->setCursor(2,46);
    dma_display->print(actual_fps);
    dma_display->print("fps");

    dma_display->setCursor(2,56);
    dma_display->print(serialData.bpm);
    dma_display->print("bpm");

  for (uint8_t i=0; i < maxPatternInitEffect; i++) {
      dma_display->setTextColor(65535);
      dma_display->setCursor(2,2+(i*10));
      dma_display->print(patternsInitEffects[i].getCurrentPatternId());      //dma_display->print(patternsAudio[i].id3);                 // why not?
      dma_display->setTextColor(255);
      if (patternsInitEffects[i].render_ms > 10) dma_display->setTextColor(65280);
      if (patternsInitEffects[i].render_ms > 15) dma_display->setTextColor(63519);
      dma_display->print(patternsInitEffects[i].render_ms);
      //dma_display->print(patternsInitEffects[i].fps_last);
  }
  for (uint8_t i=0; i < maxPatternAudio; i++) {
      dma_display->setTextColor(65535);
      dma_display->setCursor(22,2+(i*10));
      dma_display->print(patternsAudio[i].getCurrentPatternId());      //dma_display->print(patternsAudio[i].id3);                 // why not?
      dma_display->setTextColor(255);
      if (patternsAudio[i].render_ms > 10) dma_display->setTextColor(65280);
      if (patternsAudio[i].render_ms > 15) dma_display->setTextColor(63519);
      dma_display->print(patternsAudio[i].render_ms);
      //dma_display->print(patternsAudio[i].fps_last);
  }
  for (uint8_t i=0; i < maxPatternStatic; i++) {
      dma_display->setTextColor(65535);
      dma_display->setCursor(42,2+(i*10));
      dma_display->print(patternsStatic[i].getCurrentPatternId());      //dma_display->print(patternsAudio[i].id3);                 // why not?
      dma_display->setTextColor(255);
      if (patternsStatic[i].render_ms > 10) dma_display->setTextColor(65280);
      if (patternsStatic[i].render_ms > 15) dma_display->setTextColor(63519);
      dma_display->print(patternsStatic[i].render_ms);
      //dma_display->print(patternsStatic[i].fps_last);
  }

  // for 1000ms / 60bpm   65

/*
  uint16_t time1 = 999;
  fract8 frac1 = 63;                                       // 65 = 1/4
  uint8_t ans1 = scale16by8(time1,frac1);
  dma_display->setCursor(2,42);
  dma_display->print(ans1);

  uint16_t time2 = 999;
  fract8 frac2 = 65;            // 64 = 1/4
  uint8_t ans2 = scale16by8(time2,frac2);
  dma_display->setCursor(24,42);
  dma_display->print(ans2);
*/


  }

}