#ifndef PatternAudioSpectrum2_H
#define PatternAudioSpectrum2_H

class PatternAudioSpectrum2 : public Drawable {
  private:

    uint8_t x1,y1,x2,y2;
    bool vertical = false;

  public:
    PatternAudioSpectrum2() 
    {
      name = (char *)"Audio Spectrum 2";
      id2 = 1;
      id = (char *)"s";
    }


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){
      vertical = random(0,2);
    };



    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {


    effects.DimAll(250); 

    // centered spectrum analyser
    uint8_t space;
    uint8_t data;
    for (byte i = 0; i < 32; i++) 
    {
      data = fftData.specData32[i] / 3;
      x1 = i;
      x2 = i;
      y1 = MATRIX_HEIGHT - 1;
      y2 = MATRIX_HEIGHT - data;

      space = 64 - (y1 - y2);
      y1 = y1 - (space/2);
      y2 = y2 - (space/2);

      if (data)
        effects.BresenhamLine(x1, y1, x2, y2, dma_display->color565(128, 128, 128));
    }

    //effects.ShowFrame();

    return 0;
  }
};

#endif


