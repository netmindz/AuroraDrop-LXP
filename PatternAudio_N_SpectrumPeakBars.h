/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef PatternAudioSpectrumPeakBars_H
#define PatternAudioSpectrumPeakBars_H

class PatternAudioSpectrumPeakBars : public Drawable {
  private:

    uint8_t x1,y1,x2,y2;
    bool flipVert = false;
    bool diagonalLines = true;
    bool diagonalOffset = 4;
    uint8_t caleido;
    uint8_t caleidoscopeEffect;

  public:
    PatternAudioSpectrumPeakBars() 
    {
      name = (char *)"Audio Spectrum 1";
      id2 = 1;
      id = (char *)"N";
    }


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){
      flipVert = random(0,2);           // 50% chance of bveing flipped
      caleido = random(0,3);            // 1 in 3 change of not getting caleidoscope
      caleidoscopeEffect = random8(1, CALEIDOSCOPE_COUNT + 1);
      diagonalLines = random(0,10);     // 1 in 10 chance we'll get dialgonal lines
      diagonalOffset = diagonalLines ? 0 : 4;
    };



    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

    // temporary  
    effects.DimAll(250); 

    // 16 simple peak only bars
    // no caleidoscope version
    if (!caleido)
    {
      // draw on full screen
      uint8_t data;
      for (byte i = 0; i < 64; i=i+4) 
      {
        data = fftData.specData16[i/4] / 3;    // use the 16 bins for this!
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
          //effects.BresenhamLine(x1, y1, x2, y2, dma_display->color565(128, 128, 128));
          effects.BresenhamLine(x1, y1, x2, y2, dma_display->color565(128, 128, 128), 255);
      }
    }
    else 
    {
      // draw on quarter of screen an create caleido effect
      uint8_t data;
      for (byte i = 0; i < 32; i=i+2) 
      {
        data = fftData.specData16[i/2] / 6;
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
          //effects.BresenhamLine(x1, y1, x2, y2, dma_display->color565(128, 128, 128));
          effects.BresenhamLine(x1, y1, x2, y2, dma_display->color565(128, 128, 128), 255);
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
  }




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