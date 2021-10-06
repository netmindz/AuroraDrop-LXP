
#ifndef PatternTest_H
#define PatternTest_H

class PatternTest : public Drawable {
  private:

  public:
    PatternTest() {
      name = (char *)"Test Pattern";
      id2 = 1;
    }

    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      effects.DimAll(0);

      effects.BresenhamLine(0, 0, MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1, 255, 255);
      effects.BresenhamLine(0, MATRIX_HEIGHT - 1, MATRIX_WIDTH - 1, 0, 255, 255);

      effects.BresenhamLine(0, 11, MATRIX_WIDTH - 1, 11, 255, 255);
      effects.BresenhamLine(0, 23, MATRIX_WIDTH - 1, 23, 255, 255);
      effects.BresenhamLine(0, 35, MATRIX_WIDTH - 1, 35, 255, 255);
      effects.BresenhamLine(0, 47, MATRIX_WIDTH - 1, 47, 255, 255);
      effects.BresenhamLine(0, 59, MATRIX_WIDTH - 1, 59, 255, 255);
      effects.BresenhamLine(0, 71, MATRIX_WIDTH - 1, 71, 255, 255);
      effects.BresenhamLine(0, 83, MATRIX_WIDTH - 1, 83, 255, 255);

      effects.CaleidoscopeA2();


      //dma_display->fillScreen(dma_display->color565(128, 0, 0));  
      return 0;
    }
};

#endif
