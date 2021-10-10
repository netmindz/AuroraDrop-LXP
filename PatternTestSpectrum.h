
#ifndef PatternTestSpectrum_H
#define PatternTestSpectrum_H

class PatternTestSpectrum : public Drawable {
  private:

  public:
    PatternTestSpectrum() 
    {
      name = (char *)"Test Spectrum Pattern";
      id = "T";
    }


    // ------------- START ### ------------
    void start(uint8_t _pattern){

    };

    // ------------ DRAW FRAME --------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      effects.DimAll(240); 

      for (byte i = 0; i < MATRIX_WIDTH; i++) 
      {
        effects.BresLine(i, MATRIX_WIDTH - 1, i, MATRIX_HEIGHT -(fftData.specData[i] / 6), i*4, 255, NOBLEND);
        effects.BresLine(i, 0, i, fftData.specData[i] / 6, i*4, 255, LINEARBLEND);
      }


    return 0;
  }
};

#endif
