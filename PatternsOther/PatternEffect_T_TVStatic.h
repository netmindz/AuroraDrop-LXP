#ifndef PatternEffectTVStatic_H
#define PatternEffectTVStatic_H

class PatternEffectTVStatic : public Drawable {
  
  private:

    int count = 0;
    int dir = 1;

  public:
  
    PatternEffectTVStatic() {
  
      name = (char *)"TV Static";
      id = "T";
      enabled = false;
  
    }

    void start(uint8_t _pattern){

      count = MATRIX_WIDTH * MATRIX_HEIGHT * 0.05;

    };

    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      uint8_t bin = 0;

      for (int i = 0; i < count; i++) {

        byte audio = fftData.specData16[0];

        if (audio > 127) {
          
          audio = 127;

        }

        int x = random(0,MATRIX_WIDTH-1);
        int y = random(0,MATRIX_HEIGHT-1);

        effects.leds[XY(x+0, y+0)] += (x+0 ^ y+0) < count ? effects.ColorFromCurrentPalette(((x+0 ^ y+0) << 3), audio * 2) : CRGB::Black;

      }

      // uint8_t blurAmount = beatsin8(2, 10, 255);
      // blur2d(effects.leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, blurAmount);
      
      // effects.Caleidoscope2(); 

      count += dir;
        
      return 0;

    }

};

#endif