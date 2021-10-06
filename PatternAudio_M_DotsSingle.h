#ifndef PatternAudioDotsSingle_H
#define PatternAudioDotsSingle_H

class PatternAudioDotsSingle : public Drawable {
  private:

    uint8_t x0,y0,x1,y1;
    bool flipVert = false;
    bool diagonalLines = true;
    bool diagonalOffset = 4;
    uint8_t caleido = 0;
    uint8_t caleidoscopeEffect;

    bool useCurrentPalette = false;
    uint8_t colorSpread = 1;
    bool upwards = true;

    uint8_t rolling = 0;

    long  rollDelay = 100;

    long  ms_previous;

  public:
    PatternAudioDotsSingle() 
    {
      name = (char *)"Audio Dots Single";
      id = (char *)"M";
    }


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){
      flipVert = random(0,2);           // 50% chance of bveing flipped
      caleido = random(0,3);            // 1 in 3 chance of not getting caleidoscope
      caleidoscopeEffect = random8(1, CALEIDOSCOPE_COUNT + 1);

      diagonalLines = random(0,10);     // 1 in 10 chance we'll get dialgonal lines
      diagonalOffset = diagonalLines ? 0 : 4;
      useCurrentPalette = random(0,6);
      colorSpread = random(1,5);
      upwards = random(0,2);

      ms_previous = millis();

      // overidden for testing
      //caleido = 1;
      flipVert = 0;
      diagonalLines = 0;
      diagonalOffset = 0;
    };



    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      uint8_t data1;
      uint8_t data2;


      // temporary  
      //effects.DimAll(250); 

    for (byte i = 0; i < MATRIX_WIDTH; i++) 
    {
      data1 = fftData.specData[i] / 6;
      if (data1 >  MATRIX_CENTER_Y - 1) data1 = MATRIX_CENTER_Y - 1;

        x0 = i;
        y0 = data1;

        // roll the dots sideways
        x0 = x0 + rolling;
        if (x0 > MATRIX_WIDTH) x0 = x0 - MATRIX_WIDTH;

      // only draw bars if there is non zero data
      if (!fftData.noAudio) {


        if (useCurrentPalette)
        {
          // current palette
          if (upwards) 
          {
            effects.drawBackgroundFastLEDPixelCRGB(x0, MATRIX_CENTER_Y - y0, effects.ColorFromCurrentPalette(i * colorSpread));
          }
          else
          {
            effects.drawBackgroundFastLEDPixelCRGB(x0, y0, effects.ColorFromCurrentPalette(i * colorSpread));
          }
        }
        else
        {
          // original fixed palette, need to add more options
          effects.drawBackgroundFastLEDPixelCRGB(x0, y0, dma_display->color565(128, i, 128));

          // need more sparkles? don't work well
          if (data1 >= MATRIX_CENTER_Y - 3) {
            effects.drawBackgroundFastLEDPixelCRGB(x0, y0, dma_display->color565(255, 255, 255));
            effects.drawBackgroundFastLEDPixelCRGB(x0, y0 / 2, dma_display->color565(255, 255, 255));
          }
        }


      }


    }

    if (caleido) 
    {
    effects.RandomCaleidoscope(caleidoscopeEffect);
/*
    switch (caleido) {
      case 1:
        effects.Caleidoscope1();
        break;
      case 2:
        effects.Caleidoscope2();
        break;
    }
*/
    }

    // roll the display sideways one more pixel
    if (ms_previous + rollDelay < millis()) {
      rolling++;
      if (rolling >= MATRIX_WIDTH) rolling = 0;
      ms_previous = millis();
    }


    return 0;
  }
};

#endif
