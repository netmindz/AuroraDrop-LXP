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

#ifndef PatternAudioRotatingWave_H
#define PatternAudioRotatingWave_H

class PatternAudioRotatingWave : public Drawable {
  private:

    int x1,y1,x2,y2;
    Point p1,p2;
    Point rp1,rp2;
    byte audio1;
    byte audio2;
    uint8_t audioScale;

    // randomise these
    float angle = -1;
    float direction;
    bool rotate;
    bool caleidoscope;
    uint8_t caleidoscopeEffect;

    uint8_t dimVal;
    uint8_t dimStart;
    uint8_t dimEnd;


  public:
    PatternAudioRotatingWave() {
      name = (char *)"Rotating Wave";
      id = "A";
    }


    // ------------------ start -------------------
    void start(uint8_t _pattern) {

      rotate = random8(0,2);
      if (angle == -1) angle = 0;
      direction = 0.01;
      if (random8(0,2)) direction = -direction;
      audioScale = random8(3,6);
      caleidoscope = random8(0, 5);                             // 80% chance of caleidoscope
      caleidoscopeEffect = random8(1, CALEIDOSCOPE_COUNT + 1);
      dimStart = 255;
      dimEnd = random8(0, 180);
      dimVal = dimStart;

    }


    // --------------------- DRAW FRAME -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {


      //effects.DimAll(180);   // looks good with 0
      //blur2d(effects.leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, 255);


      // if we are going to dim all, then do it gradually
      effects.DimAll(dimVal);
      if (dimVal > dimEnd) dimVal--;

      if (!fftData.noAudio) 
      {

        /*
        // from top down
        if (true == true) 
        {
          for (byte i = 0; i < MATRIX_WIDTH - 1 + 16; i++) 
          {
            audio1 = fftData.specData[i] / 3;
            audio2 = fftData.specData[i+1] / 3;
            x1 = i - 8;
            y1 = audio1;
            x2 = i+1 - 8;
            y2 = audio2;
            Point p1(x1,y1);
            Point p2(x2,y2);
            Point rp1 = effects.RotatePoint(31,31,angle,p1);
            Point rp2 = effects.RotatePoint(31,31,angle,p2);
            effects.BresenhamLine(rp1.x, rp1.y, rp2.x, rp2.y, dma_display->color565(128, 128, 128));
          }
        }
        */

        // from top down backwards
        if (true == true) 
        {
          for (byte i = 0; i < MATRIX_WIDTH - 1 + 16; i++) 
          {
            audio1 = fftData.specData[i] / audioScale;
            audio2 = fftData.specData[i+1] / audioScale;
            x1 = (MATRIX_WIDTH + 16) - i - 8;
            y1 = audio1;
            x2 = (MATRIX_WIDTH + 16) - i-1 - 8;
            y2 = audio2;
            Point p1(x1,y1);
            Point p2(x2,y2);
            Point rp1 = effects.RotatePoint(31,31,angle,p1);
            Point rp2 = effects.RotatePoint(31,31,angle,p2);
            effects.BresenhamLine(rp1.x, rp1.y, rp2.x, rp2.y, effects.ColorFromCurrentPalette(i*3));
          }
        }


        // from bottom up
        if (true == true) 
        {
          for (byte i = 0; i < MATRIX_WIDTH - 1 + 16; i++) 
          {
            audio1 = fftData.specData[i] / 3;
            audio2 = fftData.specData[i+1] / 3;
            x1 = i - 8;
            y1 = MATRIX_HEIGHT - audio1;
            x2 = i+1 - 8;
            y2 = MATRIX_HEIGHT - audio2;
            Point p1(x1,y1);
            Point p2(x2,y2);
            Point rp1 = effects.RotatePoint(31,31,angle,p1);
            Point rp2 = effects.RotatePoint(31,31,angle,p2);
            effects.BresenhamLine(rp1.x, rp1.y, rp2.x, rp2.y, effects.ColorFromCurrentPalette(i*3));
          }
        }


      }


      // do caleidoscope sometimes
      if (caleidoscope) {
        effects.RandomCaleidoscope(caleidoscopeEffect);
      }



      if (rotate) 
      {
        angle = angle + direction;
        if (angle >= 3.14*2) angle = 0;
      }


      // simple test
      //for (byte i = 0; i < MATRIX_WIDTH - 1; i++) 
      //{
      //  audio1 = fftData.specData[i] / 3;
      //  audio2 = fftData.specData[i+1] / 3;
      //  effects.BresenhamLine(i, audio1, i+1, audio2, dma_display->color565(128, 128, 128));
      //}



      return 0;
    }





};

#endif
