
#ifndef PatternTestSpectrum_H
#define PatternTestSpectrum_H

class PatternTestSpectrum : public Drawable {
  private:

  //int xxx = 0;
  uint32_t ddd = 0;

  public:
    PatternTestSpectrum() 
    {
      name = (char *)"Test Spectrum Pattern";
      id2 = 1;
    }


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){

    };



    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

    effects.DimAll(250); 




    for (byte i = 0; i < MATRIX_WIDTH; i++) 
    {
      //effects.BresenhamLine(i, serialData.specData[i] / 3, i, 0, dma_display->color565(128, 128, 128));
      effects.BresenhamLine(i, MATRIX_WIDTH - 1, i, MATRIX_HEIGHT -(serialData.specData[i] / 3), dma_display->color565(128, 128, 128));
    }

    //////effects.ShowFrame();

    return 0;
  }
};

#endif
