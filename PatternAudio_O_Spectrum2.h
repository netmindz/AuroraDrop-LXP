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

#ifndef PatternAudioSpectrum2_H
#define PatternAudioSpectrum2_H

class PatternAudioSpectrum2 : public Drawable {
  private:

    uint8_t x1,y1,x2,y2;
    bool vertical;
    bool mirror;
    uint8_t mirrorDim;
    uint8_t dimAmount;
    bool useCurrentPalette;
    uint8_t colorSpread;

  public:
    PatternAudioSpectrum2() 
    {
      name = (char *)"Audio Spectrum 2";
      id = (char *)"O";
    }


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){

      vertical = random(0,2);
      mirror = random(0,2);
      mirrorDim = random(0,2) ? random8(64,128) : 255;
      //mirror = true;
      dimAmount = random(0,10);
      useCurrentPalette = random(0,2);
      colorSpread = random(1,5);

    };



    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

    // always apply small dim 
    effects.DimAll(250 - (dimAmount * 10)); 

    // centered spectrum analyser
    uint8_t space;
    uint8_t data;
    for (byte i = 0; i < MATRIX_WIDTH / 2; i++) 
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
        if (useCurrentPalette)
        {
          effects.BresenhamLine(x1, y1, x2, y2, i * colorSpread, 255);
          if (mirror) effects.BresenhamLine(MATRIX_WIDTH - x1 - 1, y1, MATRIX_WIDTH - x2 - 1, y2, i * colorSpread, mirrorDim);
        }
        else
        {
          effects.BresenhamLine(x1, y1, x2, y2, 16, 255);
          //effects.BresenhamLine(x1, y1, x2, y2, dma_display->color565(128, 128, 128), 255);
          if (mirror) effects.BresenhamLine(MATRIX_WIDTH - x1 - 1, y1, MATRIX_WIDTH - x2 - 1, y2, 16, mirrorDim);
        }
    }

    //effects.ShowFrame();

    return 0;
  }
};

#endif


