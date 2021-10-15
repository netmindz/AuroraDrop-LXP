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



#ifndef PatternAudioDiagonalSpectrum_H
#define PatternAudioDiagonalSpectrum_H

class PatternAudioDiagonalSpectrum : public Drawable {
  public:
    PatternAudioDiagonalSpectrum() {
      name = (char *)"Audio Lines";
      id = (char *)"P";
    }

    bool cycleColors;
    uint8_t color1 = 0;
    uint8_t color2 = 64;
    uint8_t color3 = 128;
    uint8_t color4 = 192;
    uint8_t brightness = 192;


    uint8_t caleidoscopeMode = 0;

    uint8_t canvasWidth;

    uint8_t mirrorMode = 0;   // 0 = top left / bottom right, top right / bottom left, 2= both

    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){
      cycleColors = random(0, 2);
      caleidoscopeMode = random(0, 3);
      mirrorMode = random(0, 3);
      Serial.print("caleidoscopeMode="); Serial.println(caleidoscopeMode);
      //effects.ClearFrame();




      canvasWidth = MATRIX_WIDTH;
    };



    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      cycleColors = true;
      //caleidoscopeMode = 0;
      //mirrorMode = 1;

      if (cycleColors) {
        color1++;
        color2++;
        color3++;
        color4++;
        if (brightness > 250)
          brightness = 63;  
        brightness++;
      }
      else {
        color1 = 0;
        color2 = 64;
        color3 = 128;
        color4 = 192;
      }




      int tx1,ty1,tx2,ty2,tx3,ty3;
      int x0,y0,x1,y1,x2,y2,x3,y3;
      byte height = 2;


      if (mirrorMode == 0 || mirrorMode == 2) {
        //for (int i=canvasWidth/8; i<canvasWidth-(canvasWidth/8); i++) {
        for (int i=0; i<canvasWidth; i++) {
          height = fftData.specData[i] / (512 / canvasWidth);
          if (height > 0) {
            tx1 = i;
            ty1 = i;
            x0 = tx1 + height;
            y0 = ty1 - height;
            x1 = tx1 - height;
            y1 = ty1 + height;
            //effects.leds[XY(x1, y1)] += effects.ColorFromCurrentPalette(color1, brightness);
            effects.BresenhamLine(x0, y0, x1, y1, 16, 255);
            x0 = tx1 + (height / 2);
            y0 = ty1 - (height / 2);
            x1 = tx1 - (height / 2);
            y1 = ty1 + (height / 2);
            effects.BresenhamLine(x0, y0, x1, y1, 16, 255);
          }
        }
      } 
      else {
        for (int i=0; i<canvasWidth; i++) {
          height = fftData.specData[i] / (512 / canvasWidth);
          if (height > 0) {
            tx1 = canvasWidth - i;
            ty1 = i;
            x0 = tx1 - height;
            y0 = ty1 - height;
            x1 = tx1 + height;
            y1 = ty1 + height;
            effects.BresenhamLine(x0, y0, x1, y1, 16, 255);
            //x0 = tx1 - (height / 2);
            //y0 = ty1 + (height / 2);
            //x1 = tx1 - (height / 2);
            //y1 = ty1 + (height / 2);
            //effects.BresenhamLine(x0, y0, x1, y1, dma_display->color565(128, 128, 128));
          }
        }
      }






    switch (caleidoscopeMode) {
      case 0:
        // do nothing
        break;
      case 1:
        effects.Caleidoscope1();
        break;
      case 2:
        effects.Caleidoscope2();  // 2 = good
        break;
      default:
        break;
    }


      //effects.DimAll(150);
      
      effects.DimAll(230);
      effects.SpiralStream(31, 31, 64, 128);        // for 64 pixel wide matrix!



      return 0;
    }
  };

#endif
