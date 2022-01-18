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

#ifndef PatternAudio3dGrid_H
#define PatternAudio3dGrid_H

class PatternAudio3dGrid : public Drawable {
  private:

    bool hueCycle = true;
    byte hue = 0;
    uint32_t hue_ms;

    bool drawVertical = false;
    bool drawWave = true;

  public:
    PatternAudio3dGrid() {
      name = (char *)"3D Grid";
      id = "T";
      enabled = true;
    }


    // ------------------ start -------------------
    void start(uint8_t _pattern) {

      hue_ms = millis();

      // randomise
      hueCycle = random8(0,5);          // mostly cycle through the the hues
      hue = random8(0,255);

      drawVertical = random8(0,2);
      drawWave = random8(0,2);
      if (!drawVertical && !drawWave) drawVertical = true;
    }


    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      int x,y;
      int oldx = 0, oldy = 0;
      int xStart = 12;
      int yStart = 17;
      byte audio;

    //effects.DimAll(200);

    if (!fftData.noAudio) {


      for (int i=0; i<121; i++) {
        audio = fftData.specData[i] / 10;

        if (drawVertical) {
          if (i < 11) {
            x = xStart + (i * 5);
            y = yStart + (i * 2);
            effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
            effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
          }

          if (i >= 11 && i < 22) {
            x = xStart - 1 + ((i - 11) * 5);
            y = yStart + 2 + ((i - 11) * 2);
            effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
            effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
          }

          if (i >= 22 && i < 33) {
            x = xStart - 2 + ((i - 22) * 5);
            y = yStart + 4 + ((i - 22) * 2);
            effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
            effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
          }

      if (i >= 33 && i < 44) {
        x = xStart - 3 + ((i - 33) * 5);
        y = yStart + 6 + ((i - 33) * 2);
        effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
        effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
      }

      if (i >= 44 && i < 55) {
        x = xStart - 4 + ((i - 44) * 5);
        y = yStart + 8 + ((i - 44) * 2);
        effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
        effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
      }

      if (i >= 55 && i < 66) {
        x = xStart - 5 + ((i - 55) * 5);
        y = yStart + 10 + ((i - 55) * 2);
        effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
        effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
      }

      if (i >= 66 && i < 77) {
        x = xStart - 6 + ((i - 66) * 5);
        y = yStart + 12 + ((i - 66) * 2);
        effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
        effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
      }

      if (i >= 77 && i < 88) {
        x = xStart - 7 + ((i - 77) * 5);
        y = yStart + 14 + ((i - 77) * 2);
        effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
        effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
      }

      if (i >= 88 && i < 99) {
        x = xStart - 8 + ((i - 88) * 5);
        y = yStart + 16 + ((i - 88) * 2);
        effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
        effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
      }

      if (i >= 99 && i < 110) {
        x = xStart - 9 + ((i - 99) * 5);
        y = yStart + 18 + ((i - 99) * 2);
        effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
        effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
      }

      if (i >= 110 && i < 121) {
        x = xStart - 10 + ((i - 110) * 5);
        y = yStart + 20 + ((i - 110) * 2);
        effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
        effects.BresenhamLine(x, y-audio, x, y-audio, hue + (i*2), 255);
      }
    }

    if (drawWave) {
      // draw wave like pattern
      if (i < 11) {
        x = xStart + (i * 5);
        y = yStart + (i * 2);
        if (i != 0) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
      }

      if (i >= 11 && i < 22) {
        x = xStart - 1 + ((i - 11) * 5);
        y = yStart + 2 + ((i - 11) * 2);
        if (i != 11) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
      }

      if (i >= 22 && i < 33) {
        x = xStart - 2 + ((i - 22) * 5);
        y = yStart + 4 + ((i - 22) * 2);
        if (i != 22) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
      }

      if (i >= 33 && i < 44) {
        x = xStart - 3 + ((i - 33) * 5);
        y = yStart + 6 + ((i - 33) * 2);
        if (i != 33) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
      }

      if (i >= 44 && i < 55) {
        x = xStart - 4 + ((i - 44) * 5);
        y = yStart + 8 + ((i - 44) * 2);
        if (i != 44) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
      }

          if (i >= 55 && i < 66) {
            x = xStart - 5 + ((i - 55) * 5);
            y = yStart + 10 + ((i - 55) * 2);
            if (i != 55) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
          }

          if (i >= 66 && i < 77) {
            x = xStart - 6 + ((i - 66) * 5);
            y = yStart + 12 + ((i - 66) * 2);
            if (i != 66) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
          }

          if (i >= 77 && i < 88) {
            x = xStart - 7 + ((i - 77) * 5);
            y = yStart + 14 + ((i - 77) * 2);
            if (i != 77) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
          }

          if (i >= 88 && i < 99) {
            x = xStart - 8 + ((i - 88) * 5);
            y = yStart + 16 + ((i - 88) * 2);
            if (i != 88) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
          }

          if (i >= 99 && i < 110) {
            x = xStart - 9 + ((i - 99) * 5);
            y = yStart + 18 + ((i - 99) * 2);
            if (i != 99) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
          }

          if (i >= 110 && i < 121) {
            x = xStart - 10 + ((i - 110) * 5);
            y = yStart + 20 + ((i - 110) * 2);
            if (i != 110) effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 128);
          }

        }

        oldx = x;
        oldy = y-audio;

      }

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
