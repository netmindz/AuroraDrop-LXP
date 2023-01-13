#ifndef PatternEffectTVStatic_H
#define PatternEffectTVStatic_H

class PatternEffectTVStatic : public Drawable {
  
  private:

    int count = 0;
    int growth = 0;
    int staticsize = 1;

  public:
  
    PatternEffectTVStatic() {
  
      name = (char *)"TV Static";
      id = "T";
      enabled = false;
  
    }

    void start(uint8_t _pattern){

      staticsize = random(1,8);

      int overallcount = MATRIX_WIDTH * MATRIX_HEIGHT * 0.2;
      count = overallcount / (staticsize*staticsize);         // trying to keep the same number of LEDs updated per pass regardless of size

    };

    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      byte audio = fftData.specData16[0];

      audio = map(audio,0,255,128,240);

      for (int i = 0; i < count; i++) {

        int x = random(0,MATRIX_WIDTH-staticsize);
        int y = random(0,MATRIX_HEIGHT-staticsize);

        for (int sizex = 0; sizex < staticsize; sizex++) {

          for (int sizey = 0; sizey < staticsize; sizey++) {

            effects.leds[XY(x+sizex, y+sizey)].nscale8_video(audio);

          }

        }

      }

      count += growth;
        
      return 0;

    }

};

#endif