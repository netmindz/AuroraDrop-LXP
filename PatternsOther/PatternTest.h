
#ifndef PatternTest_H
#define PatternTest_H

class PatternTest : public Drawable {
  private:

  public:
    PatternTest() {
      name = (char *)"Test Pattern";
      id = "T";




    }

    int x=0;
    int y=0;


    void start(uint8_t _pattern) {
      


    }


    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      effects.DimAll(240);

      effects.BresenhamLine(x, y, x, y, 127, 255);


      x++;
      if (x >= MATRIX_WIDTH) {
        x = 0;
        y++;
        if (y >= MATRIX_HEIGHT) {
          y = 0;
        }
      }

      effects.BresenhamLine(0, 0, MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1, 127, 255);
      effects.BresenhamLine(0, MATRIX_HEIGHT - 1, MATRIX_WIDTH - 1, 0, 255, 255);
      effects.BresenhamLine(0, 0, MATRIX_WIDTH - 1, 0, 63, 255);
      effects.BresenhamLine(0, MATRIX_HEIGHT - 1, MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1, 192, 255);

      //effects.BresenhamLine(0, 0, 0, 0, 127, 255);
      //effects.BresenhamLine(MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1, MATRIX_WIDTH - 2, MATRIX_HEIGHT - 1, 63, 255);


      //effects.BresenhamLine(0, 11, MATRIX_WIDTH - 1, 11, 255, 255);
      //effects.BresenhamLine(0, 23, MATRIX_WIDTH - 1, 23, 255, 255);
      //effects.BresenhamLine(0, 35, MATRIX_WIDTH - 1, 35, 255, 255);
      //effects.BresenhamLine(0, 47, MATRIX_WIDTH - 1, 47, 255, 255);
      //effects.BresenhamLine(0, 59, MATRIX_WIDTH - 1, 59, 255, 255);
      //effects.BresenhamLine(0, 71, MATRIX_WIDTH - 1, 71, 255, 255);
      //effects.BresenhamLine(0, 83, MATRIX_WIDTH - 1, 83, 255, 255);

      byte audio1;
      byte audio2;
      //int startX = 0;
      //int startY = 0;


/*
      for (byte i = 0; i < MATRIX_WIDTH - 1; i++) 
      {
        audio1 = fftData.specData[i] / 3;
        audio2 = fftData.specData[i+1] / 3;
        effects.BresenhamLine(i, audio1, i+1, audio2, 16);
        //startX = i;
        //startY = audio;
        //effects.BresenhamLine(i, MATRIX_WIDTH - 1, i, MATRIX_HEIGHT -(fftData.specData[i] / 3), dma_display->color565(128, 128, 128));
      }
*/



        //effects.BresenhamLine(startX, startY, MATRIX_WIDTH - 1, 0, dma_display->color565(128, 128, 128));


      //effects.CaleidoscopeA2();


      //dma_display->fillScreen(dma_display->color565(128, 0, 0));  
      return 0;
    }
};

#endif
