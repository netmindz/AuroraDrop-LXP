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

#ifndef PatternAudioAngles_H
#define PatternAudioAngles_H

class PatternAudioAngles : public Drawable {
  private:
    
    bool topBottom = true;
    bool centerOut = false;
    uint8_t count1 = 0;
    uint8_t count2 = 0;

  public:
    PatternAudioAngles() {
      name = (char *)"Angles";
      id = "A";
      enabled = true;
    }


    // ------------------ start -------------------
    void start(uint8_t _pattern) {

      topBottom = random8(0,2);
      centerOut = random8(0,2);

    }


    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      uint16_t x0, y0, x1, y1;


      for (byte i = 0; i < MATRIX_WIDTH / 2; i = i + 2) {


        if (topBottom) {
          x0 = i + count1;
          y0 = fftData.specData64[i] / 8;
          x1 = i + (MATRIX_WIDTH / 3);
          y1 = fftData.specData64[i] / 3;
        }
        else
        {
          y0 = i + count1;
          x0 = fftData.specData64[i] / 8;
          y1 = i + (MATRIX_WIDTH / 3);
          x1 = fftData.specData64[i] / 3;
        }

        if (centerOut) {
          if (y0 < MATRIX_HEIGHT/2) {
            y0 = (MATRIX_HEIGHT/2) - y0;
          }
          else
          {
            y0 = y0 - (MATRIX_HEIGHT/2);
          }
          if (y1 < MATRIX_HEIGHT/2) {
            y1 = (MATRIX_HEIGHT/2) - y1;
          }
          else
          {
            y1 = y1 - (MATRIX_HEIGHT/2);
          }
        }

/*
        effects.BresLine(x0, y0, x1, y1, i * 8, 200, LINEARBLEND);
        effects.BresLine(MATRIX_WIDTH - x0, y0, MATRIX_WIDTH - x1, y1, i * 8, 200, LINEARBLEND);
        // bright dots on tips
        effects.BresLine(x1, y1, x1, y1, 255 - (i*8), 255, LINEARBLEND);
        effects.BresLine(MATRIX_WIDTH - x1, y1, MATRIX_WIDTH - x1, y1, 255 - (i*8), 255, LINEARBLEND);

        effects.BresLine(x0, MATRIX_HEIGHT-1 - y0, x1, MATRIX_HEIGHT-1 - y1, i * 8, 200, LINEARBLEND);
        effects.BresLine(MATRIX_WIDTH - x0, MATRIX_HEIGHT-1 - y0, MATRIX_WIDTH - x1, MATRIX_HEIGHT-1 - y1, i * 8, 200, LINEARBLEND);
        // bright dots on tips
        effects.BresLine(x1, MATRIX_HEIGHT-1 - y1, x1, MATRIX_HEIGHT-1 - y1, 255 - (i*8), 255, LINEARBLEND);
        effects.BresLine(MATRIX_WIDTH - x1, MATRIX_HEIGHT-1 - y1, MATRIX_WIDTH - x1, MATRIX_HEIGHT-1 - y1, 255 - (i*8), 255, LINEARBLEND);
*/

      }





      // canvas
      effects.ClearCanvas(1);        // clear half width canvas 1
      uint8_t canvasScale = 2;
      uint8_t canvasWidth = MATRIX_WIDTH / canvasScale;
      uint8_t canvasHeight = MATRIX_HEIGHT / canvasScale;
      uint8_t centreX = (MATRIX_WIDTH  / 2) / canvasScale;
      uint8_t centreY = (MATRIX_HEIGHT  / 2) / canvasScale;
      uint8_t audioScale = canvasScale * 2;
      uint8_t color1 = 0;

      for (byte i = 0; i < canvasWidth / 2; i = i + 2) {


        if (topBottom) {
          x0 = i + count2;
          y0 = fftData.specData64[i] / 8;
          x1 = i + (canvasWidth / 3);
          y1 = fftData.specData64[i] / 3;
        }
        else
        {
          y0 = i + count2;
          x0 = fftData.specData64[i] / 8;
          y1 = i + (canvasWidth / 3);
          x1 = fftData.specData64[i] / 3;
        }

        if (centerOut) {
          if (y0 < canvasHeight/2) {
            y0 = (canvasHeight/2) - y0;
          }
          else
          {
            y0 = y0 - (canvasHeight/2);
          }
          if (y1 < canvasHeight/2) {
            y1 = (canvasHeight/2) - y1;
          }
          else
          {
            y1 = y1 - (canvasHeight/2);
          }
        }
        effects.BresLineCanvasH(effects.canvasH, x0, y0, x1, y1, effects.ColorFromCurrentPalette((i*16) + color1, 200, NOBLEND));
        effects.BresLineCanvasH(effects.canvasH, canvasWidth - x0, y0, canvasWidth - x1, y1, effects.ColorFromCurrentPalette((i*16) + color1, 200, NOBLEND));
        // bright dots on tips
        //effects.BresLineCanvasH(effects.canvasH, x1, y1, x1, y1, 255 - (i*8));
        //effects.BresLineCanvasH(effects.canvasH, canvasWidth - x1, y1, canvasWidth - x1, y1, 255 - (i*8));

        effects.BresLineCanvasH(effects.canvasH, x0, canvasHeight-1 - y0, x1, canvasHeight-1 - y1, effects.ColorFromCurrentPalette((i*16) + color1, 200, NOBLEND));
        effects.BresLineCanvasH(effects.canvasH, canvasWidth - x0, canvasHeight-1 - y0, canvasWidth - x1, canvasHeight-1 - y1, effects.ColorFromCurrentPalette((i*16) + color1, 200, NOBLEND));
        // bright dots on tips
        //effects.BresLineCanvasH(effects.canvasH, x1, canvasHeight-1 - y1, x1, canvasHeight-1 - y1, 255 - (i*8));
        //effects.BresLineCanvasH(effects.canvasH, canvasWidth - x1, canvasHeight-1 - y1, canvasWidth - x1, canvasHeight-1 - y1, 255 - (i*8));






      }

      effects.ApplyCanvasH(effects.canvasH, 0, 0);    
      effects.ApplyCanvasH(effects.canvasH, 0, 32);    
      effects.ApplyCanvasH(effects.canvasH, 32, 0);    
      effects.ApplyCanvasH(effects.canvasH, 32, 32);    

      effects.ApplyCanvasH(effects.canvasH, -16, 16);    
      effects.ApplyCanvasH(effects.canvasH, 48, 16);    

      effects.ApplyCanvasH(effects.canvasH, 16, 16);    



      //count1++;
      //if (count1 >= MATRIX_WIDTH/2) count1 = 0;
      count1 = effects.beatSineOsci8[4] / 8;


      return 0;
    }


};

#endif
