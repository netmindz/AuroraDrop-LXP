#ifndef PatternEffectBlurMove_H
#define PatternEffectBlurMove_H

class PatternEffectBlurMove: public Drawable {
  
  private:

    int count = 0;
    int dir = 1;

  public:
  
    PatternEffectBlurMove() {
  
      name = (char *)"Blur Move";
      id = "K";
      enabled = false;
  
    }

    void start(uint8_t _pattern){

      count = 1;
      x = random(0,MATRIX_WIDTH);

    };

    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

        if (x > MATRIX_WIDTH) {

            x = 0;
            
        }

      for (int i = 0; i < count; i++) {

        int x = random(0,MATRIX_WIDTH-2);
        int y = random(0,MATRIX_HEIGHT-2);

        effects.leds[XY(x, 0)] = effects.ColorFromCurrentPalette(color, audio);
        effects.leds[XY(x+1, y+0)] = effects.ColorFromCurrentPalette(color, audio);
        effects.leds[XY(x+0, y+1)] = effects.ColorFromCurrentPalette(color, audio);
        effects.leds[XY(x+1, y+1)] = effects.ColorFromCurrentPalette(color, audio);

      }

      count += dir;
        
      return 0;

    }

};

#endif