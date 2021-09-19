#ifndef PatternAudioWaveSingle_H
#define PatternAudioWaveSingle_H

class PatternAudioWaveSingle : public Drawable {
  private:

    uint8_t x0,y0,x1,y1;
    bool flipVert = false;
    bool diagonalLines = true;
    bool diagonalOffset = 4;
    uint8_t caleido = 0;

  public:
    PatternAudioWaveSingle() 
    {
      name = (char *)"Audio Wave Single";
    }


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){
      flipVert = random(0,2);           // 50% chance of bveing flipped
      caleido = random(0,3);            // 1 in 3 change of not getting caleidoscope
      diagonalLines = random(0,10);     // 1 in 10 chance we'll get dialgonal lines
      diagonalOffset = diagonalLines ? 0 : 4;

      // overidden for testing
      caleido = 0;
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
      effects.DimAll(100); 


    // draw on full screen
    //bool invert = 0;
    int8_t invert2 = -1;
    for (byte i = 0; i < MATRIX_WIDTH - 4; i=i+2) 
    {
      // use the 16 bins for these!
      data1 = serialData.specData32[i/2] / 6;
      data2 = serialData.specData32[(i/2) + 1] / 6; 
      // limit peaks
      if (data1 > MATRIX_HEIGHT - 10) data1 = MATRIX_HEIGHT - 10;
      if (data2 > MATRIX_HEIGHT - 10) data2 = MATRIX_HEIGHT - 10;

        x0 = i;
        y0 = (-data1 * invert2) + MATRIX_CENTRE_Y;

        x1 = x0 + 4;
        y1 = (data2 * invert2) + MATRIX_CENTRE_Y;


/*
      if (!invert) {
        x0 = i;
        y0 = -data1 + MATRIX_CENTRE_Y;

        x1 = x0 + 4;
        y1 = data2 + MATRIX_CENTRE_Y;

      }
      else {
        x0 = i;
        y0 = data1 + MATRIX_CENTRE_Y;

        x1 = x0 + 4;
        y1 = -data2 + MATRIX_CENTRE_Y;
      }
*/


      // TODO: only draw bars if there is non zero data
      if (!serialData.noAudio)
        effects.BresenhamLine(x0, y0, x1, y1, dma_display->color565(128, 128, 128));

      // invert direction next loop
      //invert = !invert;
      invert2 = -invert2;

    }
    // TODO: only draw bars if there is non zero data
    // final clean-up line
    if (!serialData.noAudio)
      effects.BresenhamLine(x1, y1, MATRIX_WIDTH, MATRIX_CENTRE_Y, dma_display->color565(128, 128, 128));



/*

    // 16 simple peak only bars
    // no caleidoscope version
    if (!caleido)
    {
      // draw on full screen
      uint8_t data;
      for (byte i = 0; i < 64; i=i+4) 
      {
        data = serialData.specData16[i/4] / 3;
        if (data > 63) data = 63;
        x1 = i;
        x2 = i+4;
        if (flipVert) {
          y1 = data;
          y2 = y1 + diagonalOffset;
        }
        else {
          y1 = MATRIX_HEIGHT - data;
          y2 = y1 - diagonalOffset;
        }
        // only draw bars if there is non zero data
        if (data)
          effects.BresenhamLine(x1, y1, x2, y2, dma_display->color565(128, 128, 128));
      }
    }
    else 
    {
      // draw on quarter of screen an create caleido effect
      uint8_t data;
      for (byte i = 0; i < 32; i=i+2) 
      {
        data = serialData.specData16[i/2] / 6;
        if (data > 31) data = 31;
        x1 = i;
        x2 = i+4;
        if (flipVert) {
          y1 = data;
          y2 = y1 + (diagonalOffset/2);
        }
        else {
          y1 = MATRIX_HEIGHT - data - 32;
          y2 = y1 - (diagonalOffset/2);
        }
        // only draw bars if there is non zero data
        if (data)
          effects.BresenhamLine(x1, y1, x2, y2, dma_display->color565(128, 128, 128));
      }
      switch (caleido) {
        case 1:
          effects.Caleidoscope1();
          break;
        case 2:
          effects.Caleidoscope2();
          break;
      }
    }



*/
      




/*
    // centered spectrum analyser
    uint8_t space;
    for (byte i = 0; i < 32; i++) 
    {
      x1 = i;
      y1 = MATRIX_HEIGHT - 1;
      x2 = i;
      y2 = MATRIX_HEIGHT -(serialData.specData32[i] / 3);
      space = 64 - (y1 - y2);
      y1 = y1 - (space/2);
      y2 = y2 - (space/2);
      effects.BresenhamLine(x1, y1, x2, y2, dma_display->color565(128, 128, 128));
    }
*/


/*
    for (byte i = 0; i < MATRIX_WIDTH; i++) 
    {
      //effects.BresenhamLine(i, serialData.spec255Data[i] / 3, i, 0, dma_display->color565(128, 128, 128));
      effects.BresenhamLine(i, MATRIX_WIDTH - 1, i, MATRIX_HEIGHT -(serialData.spec255Data[i] / 3), dma_display->color565(128, 128, 128));
    }
*/

/*
    for (byte i = 0; i < 32; i++) 
    {
      effects.BresenhamLine(i, MATRIX_WIDTH - 1, i, MATRIX_HEIGHT -(serialData.specData32[i] / 3), dma_display->color565(128, 128, 128));
    }
*/

/*
    for (byte i = 0; i < 8; i++) 
    {
      effects.BresenhamLine(i*2, MATRIX_WIDTH - 1, i*2, MATRIX_HEIGHT -(serialData.specData8[i] / 3), dma_display->color565(128, 128, 128));
    }
*/
    //////effects.ShowFrame();

    return 0;
  }
};

#endif


/*

    //for (byte i = 0; i < MATRIX_WIDTH; i++) 
    for (byte i = 0; i < 32; i++) 
    {
      effects.BresenhamLine(i, serialData.spec255Data[i] / 3, i, 0, dma_display->color565(128, 128, 128));
      //effects.BresenhamLine(i, MATRIX_WIDTH - 1, i, MATRIX_HEIGHT -(serialData.specData32[i] / 3), dma_display->color565(128, 128, 128));
    }


*/