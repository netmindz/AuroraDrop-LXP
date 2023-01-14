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
      enabled = false;
    }

    void start(uint8_t _pattern){

      count = 0;
      dir = 1;
      flip = 0;
      generation = 0;

    };

    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      if (count >= MATRIX_WIDTH || count < 0) {

        dir = dir * -1;

        // effects.DimAll(254);
        // return 0;

      }

      for (byte x = 0; x < MATRIX_WIDTH; x += 2) {

        for (byte y = 0; y < MATRIX_HEIGHT; y += 2) {

          byte audio = fftData.specData16[0];

          if (audio > 127) {
            
            audio = 127;

          }

          effects.leds[XY(x+0, y+0)] += (x+0 ^ y+0 ^ flip) < count ? effects.ColorFromCurrentPalette(((x+0 ^ y+0) << 3) + generation, audio * 2) : CRGB::Black;
          effects.leds[XY(x+1, y+0)] += (x+1 ^ y+0 ^ flip) < count ? effects.ColorFromCurrentPalette(((x+1 ^ y+0) << 3) + generation, audio * 2) : CRGB::Black;
          effects.leds[XY(x+0, y+1)] += (x+0 ^ y+1 ^ flip) < count ? effects.ColorFromCurrentPalette(((x+0 ^ y+1) << 3) + generation, audio * 2) : CRGB::Black;
          effects.leds[XY(x+1, y+1)] += (x+1 ^ y+1 ^ flip) < count ? effects.ColorFromCurrentPalette(((x+1 ^ y+1) << 3) + generation, audio * 2) : CRGB::Black;

        }

      }
        
      count += dir;
    
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