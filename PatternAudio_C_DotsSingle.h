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

#ifndef PatternAudioDotsSingle_H
#define PatternAudioDotsSingle_H

class PatternAudioDotsSingle : public Drawable {
  private:

    uint8_t dimVal;
    uint8_t dimStart;

    uint8_t x0,y0,x1,y1;
    bool vertical = false;
    bool diagonalLines = true;
    bool diagonalOffset = 4;

    // randomise theses
    uint8_t dimEnd;
    bool useCurrentPalette = false;
    uint8_t colorSpread = 1;
    bool upwards = true;
    uint8_t audioScale;
    bool caleido = 0;
    uint8_t caleidoEffect;
    bool hueCycle = true;
    byte hue = 0;
    uint32_t hue_ms;


    uint8_t rolling = 0;

    long  rollDelay = 100;

    long  ms_previous;

  public:
    PatternAudioDotsSingle() 
    {
      name = (char *)"Audio Dots Single";
      id = (char *)"C";
    }


    // ------------------ START ------------------
    void start(uint8_t _pattern){

      hue_ms = millis();
      dimStart = 255;
      dimVal = dimStart;

      // randomise stuff
      dimEnd = random8(128, 218);
      caleido = random(0,3);            // 1 in 3 chance of not getting caleidoscope
      caleidoEffect = random8(1, 3);    // only chose effect 1 or 2
      vertical = random(0,2);
      upwards = random(0,2);
      useCurrentPalette = random(0,6);  // throw in fire or ocean colours occasionaly
      colorSpread = random(2,5);
      audioScale = random(6,11);        // 6 and above is good
      hueCycle = random8(0,5);          // mostly cycle through the the hues
      hue = random8(0,255);


      ms_previous = millis();

      //caleido = true;
      //vertical = true;
      //upwards = true;
      //hueCycle = true;
    };

    // --------------- DRAW FRAME ----------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      uint8_t data1;

      // if we are going to dim all, then do it gradually
      effects.DimAll(dimVal);
      if (dimVal > dimEnd) dimVal--;
      //effects.DimAll(128);

    for (byte i = 0; i < MATRIX_WIDTH; i++) 
    {
      data1 = fftData.specData[i] / audioScale;
      if (data1 >  MATRIX_CENTER_Y - 1) data1 = MATRIX_CENTER_Y - 1;

        x0 = i;
        y0 = data1;

        // roll the dots sideways
        x0 = x0 + rolling;
        if (x0 > MATRIX_WIDTH) x0 = x0 - MATRIX_WIDTH;

      // only draw bars if there is non zero data
      if (!fftData.noAudio) {
        CRGB color1, color2;

        if (useCurrentPalette)
        {
          color1 = effects.ColorFromCurrentPalette((i * colorSpread) + hue);
        }
        else
        {
          color1 = dma_display->color565(128, i, 128);
          color2 = dma_display->color565(255, 255, 255);
        }

        if (vertical) {
          if (upwards) 
          {
            effects.drawBackgroundFastLEDPixelCRGB(y0, x0, color1);
            effects.drawBackgroundFastLEDPixelCRGB(MATRIX_WIDTH - y0, x0, color1);
            //effects.drawBackgroundFastLEDPixelCRGB(MATRIX_WIDTH - y0, MATRIX_HEIGHT - x0, color1);
          }
          else
          {
            effects.drawBackgroundFastLEDPixelCRGB(y0, MATRIX_HEIGHT - x0, color1);
            effects.drawBackgroundFastLEDPixelCRGB(MATRIX_WIDTH - y0, MATRIX_HEIGHT - x0, color1);
          }
        }
        else
        {
          if (upwards) 
          {
            effects.drawBackgroundFastLEDPixelCRGB(x0, MATRIX_CENTER_Y - y0, color1);
            effects.drawBackgroundFastLEDPixelCRGB(x0, MATRIX_CENTER_Y + y0, color1);
          }
          else
          {
            effects.drawBackgroundFastLEDPixelCRGB(x0, y0, color1);
            effects.drawBackgroundFastLEDPixelCRGB(x0, MATRIX_HEIGHT - y0, color1);
          }
        }



      }


    }

    if (caleido) 
    {
    effects.RandomCaleidoscope(caleidoEffect);


    }

    // roll the display sideways one more pixel
    if (ms_previous + rollDelay < millis()) {
      rolling++;
      if (rolling >= MATRIX_WIDTH) rolling = 0;
      ms_previous = millis();
    }


    // cylce through hue, this should be bpm related?
    if (hueCycle && hue_ms + 50 < millis()) 
    {
      hue++;
      hue_ms = millis();
    }

    return 0;
  }
};

#endif
