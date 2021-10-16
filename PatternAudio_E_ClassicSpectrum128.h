
#ifndef PatternAudioClassicSpectrum128_H
#define PatternAudioClassicSpectrum128_H

class PatternAudioClassicSpectrum128 : public Drawable {
  private:

    bool started = false;
    // random stuff
    bool peakBars;
    uint8_t audioScale;
    uint8_t peakBrightness;
    bool hueCycle;
    byte hue;
    uint32_t hue_ms;


  public:
    PatternAudioClassicSpectrum128() 
    {
      name = (char *)"Classic 128 Spectrum ";
      id = "T";
      enabled = true;
    }


    // ------------- START ### ------------
    void start(uint8_t _pattern) {
      if (!started) {
        started = true;
        hue_ms = millis();
        // randomise stuff
        hueCycle = random8(0,2);
        hue = random8(0,255);
      }
      // re-randomise stuff
      peakBars = random8(0,2);              // 0=no peaks, 1=peaks, 2=both
      audioScale = random8(5,8);
      peakBrightness = random8(63,95);      // 63 is good

    };

    // ------------ DRAW FRAME --------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      effects.DimAll(240); 

      switch (peakBars) {
        case 0:
          for (byte i = 0; i < MATRIX_WIDTH; i++) 
          {
            // treble on top, full brightness
            effects.BresLine(i, 0, i, fftData.specData[127-i] / audioScale, 255-(i*2), 255, LINEARBLEND);
            // bass on bottom, full brightness
            effects.BresLine(i, MATRIX_WIDTH - 1, i, MATRIX_HEIGHT -(fftData.specData[i] / audioScale), i*2, 255, LINEARBLEND);
          }
          break;
        case 1:
          for (byte i = 0; i < MATRIX_WIDTH; i++) 
          {
            // treble on top, dim on peak, then full brightness on current
            effects.BresLine(i, 0, i, fftData.specDataPeak[127-i] / audioScale, 255-(i*2), peakBrightness, NOBLEND);
            effects.BresLine(i, 0, i, fftData.specData[127-i] / audioScale, 255-(i*2), 255, LINEARBLEND);
            // bass on bottom, full brightness
            effects.BresLine(i, MATRIX_WIDTH - 1, i, MATRIX_HEIGHT -(fftData.specDataPeak[i] / audioScale), i*2, peakBrightness, NOBLEND);
            effects.BresLine(i, MATRIX_WIDTH - 1, i, MATRIX_HEIGHT -(fftData.specData[i] / audioScale), i*2, 255, LINEARBLEND);
          }
          break;
        case 2:
          break;
      }

/*
      for (byte i = 0; i < MATRIX_WIDTH; i++) 
      {
        //bass
        effects.BresLine(i, MATRIX_WIDTH - 1, i, MATRIX_HEIGHT -(fftData.specData[i] / 6), i*4, 255, LINEARBLEND);
      }

      for (byte i = 0; i < MATRIX_WIDTH; i++) 
      {
        //treble
        effects.BresLine(i, 0, i, fftData.specDataPeak[127-i] / 6, i*4, 63, NOBLEND);
        effects.BresLine(i, 0, i, fftData.specData[127-i] / 6, i*4, 255, LINEARBLEND);
      }
*/

      // cylce through hue, this should be bpm related?
      if (hueCycle && hue_ms + 50 < millis()) 
      {
        hue++;
        hue_ms = millis();
      }


    return 0;
  }
};

#endif
