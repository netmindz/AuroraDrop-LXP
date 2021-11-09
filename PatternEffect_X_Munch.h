#ifndef PatternEffectMunch_H
#define PatternEffectMunch_H

class PatternEffectMunch : public Drawable {
  private:
    byte count = 0;
    byte dir = 1;
    byte flip = 0;
    byte generation = 0;

  public:
    PatternEffectMunch() {
      name = (char *)"Munch";
      id = "M";
      enabled = true;
    }


    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
      uint8_t bin = 0;
      for (byte x = 0; x < MATRIX_WIDTH; x++) {
        for (byte y = 0; y < MATRIX_HEIGHT; y++) {
          byte audio = fftData.specDataPeak[bin];
          if (audio > 127) audio = 127;
          audio = audio * 2;
          if (audio > 127) audio = 127;
          //audio = 127;
          effects.leds[XY(x, y)] += (x ^ y ^ flip) < count ? effects.ColorFromCurrentPalette(((x ^ y) << 3) + generation, audio * 2) : CRGB::Black;
          bin++;
          if (bin >127) bin = 0;

        }
      }
        
      count += dir;
        
      if (count <= 0 || count >= MATRIX_WIDTH) {  // was MATRIX_WIDTH (a bit loud)
        dir = -dir;
      }
        
      if (count <= 0) {
        if (flip == 0)
          flip = 7; //31
        else
          flip = 0;
      }

      generation++;
          
        
      return 0;
    }
};

#endif