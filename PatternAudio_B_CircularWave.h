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

#ifndef PatternAudioCircularWave_H
#define PatternAudioCircularWave_H

class PatternAudioCircularWave : public Drawable {
  private:

    int8_t cx, cy;
    uint8_t dimVal;
    uint8_t dimStart;

    
    // randomise these
    uint8_t dimEnd;
    int8_t patternChoice;
    bool hueCycle = true;
    byte hue = 0;
    uint32_t hue_ms;


    void DrawWave(int8_t cx, int8_t cy, int8_t dia, uint8_t rotate, uint8_t hue_offset, bool direction) {

      int8_t x1,y1,x2,y2;
      byte data1, data2;
      uint8_t brightness;
      uint8_t len1,len2;

      for (uint8_t i = 0; i < MATRIX_WIDTH - 1; i++) 
      {
        data1 = fftData.specData[i];
        data2 = fftData.specData[i+1];
        if (data1 > 127) data1 = 127;
        if (data2 > 127) data2 = 127;
        brightness = data1 > 16 ? 255 : data1 * 16;
        data1 = data1 / 8;
        data2 = data2 / 8;
        if (direction) {
          len1 = dia - data1;
          len2 = dia - data2;
        }
        else
        {
          len1 = dia + data1;
          len2 = dia + data2;
        }

        x1 = mapcos8((i*4) + rotate, 0, len1);
        y1 = mapsin8((i*4) + rotate,0,len1);
        x1 = x1 - ((len1) / 2);
        y1 = y1 - ((len1) / 2);
        x1 = x1 + cx;
        y1 = y1 + cy;
        x2 = mapcos8(((i+1)*4) + rotate,0,len2);
        y2 = mapsin8(((i+1)*4) + rotate,0,len2);
        x2 = x2 - ((len2) / 2);
        y2 = y2 - ((len2) / 2);
        x2 = x2 + cx;
        y2 = y2 + cy;
        //effects.BresLine(x1, y1, x2, y2, effects.ColorFromCurrentPalette((i*4) + hue + hue_offset), 255);
        effects.BresLine(x1, y1, x2, y2, (i*4) + hue + hue_offset, brightness);
      }
      // draw final trailing line(s)
      data1 = fftData.specData[0];
      if (data1 > 127) data1 = 127;
      data1 = data1 / 8;
        if (direction) {
          len1 = dia - data1;
        }
        else
        {
          len1 = dia + data1;
        }
      x1 = mapcos8(rotate,0,len1);
      y1 = mapsin8(rotate,0,len1);
      x1 = x1 - ((len1) / 2);
      y1 = y1 - ((len1) / 2);
      x1 = x1 + cx;
      y1 = y1 + cy;
      //effects.BresLine(x2, y2, x1, y1, effects.ColorFromCurrentPalette(hue + hue_offset), 255);
      effects.BresLine(x2, y2, x1, y1, hue + hue_offset, brightness);
    }


  public:
    PatternAudioCircularWave() {
      name = (char *)"Circular Wave";
      id = "B";
    }

    // ------------------ start -------------------
    void start(uint8_t _pattern) {

      hue_ms = millis();
      cx = MATRIX_CENTER_X;
      cy = MATRIX_CENTER_Y;
      dimStart = 255;
      dimVal = dimStart;

      // randomise stuff
      dimEnd = random8(128, 218);
      patternChoice = random8(0,5);
      hueCycle = random8(0,2);
      hue = random8(0,255);

    }

    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      int8_t x1,y1,x2,y2;

      //effects.DimAll(128);
      // if we are going to dim all, then do it gradually
      effects.DimAll(dimVal);
      if (dimVal > dimEnd) dimVal--;


      // if we have audio, draw pattern frame of choice
      if (!fftData.noAudio) {
        switch (patternChoice) {
          case 0:
            // all centred going outwards
            DrawWave(cx, cy, 16, 0, 64, false);
            DrawWave(cx, cy, 32, 0, 128, false);
            DrawWave(cx, cy, 48, 0, 192, false);
            DrawWave(cx, cy, 64, 0, 0, false);
            DrawWave(cx, cy, 80, 0, 64, false);
            break;
          case 1:
            // all centred going inwards
            DrawWave(cx, cy, 16, 0, 64, true);
            DrawWave(cx, cy, 32, 0, 128, true);
            DrawWave(cx, cy, 48, 0, 192, true);
            DrawWave(cx, cy, 64, 0, 0, true);
            DrawWave(cx, cy, 80, 0, 64, true);
            break;
          case 2:
            // centred in and out
            DrawWave(cx, cy, 16, 0, 64, false);
            DrawWave(cx, cy, 48, 128, 128, true);
            DrawWave(cx, cy, 48, 0, 192, false);
            DrawWave(cx, cy, 80, 128, 0, true);
            DrawWave(cx, cy, 80, 0, 64, false);
            break;
          case 3:
            // centred top left
            DrawWave(cx/2, cy/2, 16, 0, 64, false);
            DrawWave(cx/2, cy/2, 37, 0, 128, false);
            DrawWave(cx/2, cy/2, 58, 0, 192, false);
            DrawWave(cx/2, cy/2, 80, 0, 0, false);
            // centred bottom right 
            DrawWave(cx+(cx/2), cy+(cy/2), 16, 0, 64, false);
            DrawWave(cx+(cx/2), cy+(cy/2), 37, 0, 128, false);
            DrawWave(cx+(cx/2), cy+(cy/2), 58, 0, 192, false);
            DrawWave(cx+(cx/2), cy+(cy/2), 80, 0, 0, false);
            break;
          case 4:
            // centered left
            DrawWave(cx/2, cy, 16, 0, 64, false);
            DrawWave(cx/2, cy, 37, 0, 128, false);
            DrawWave(cx/2, cy, 58, 0, 0, false);
            DrawWave(cx/2, cy, 80, 0, 64, false);
            // centered right
            DrawWave(cx+(cx/2), cy, 16, 0, 64, false);
            DrawWave(cx+(cx/2), cy, 37, 0, 128, false);
            DrawWave(cx+(cx/2), cy, 58, 0, 0, false);
            DrawWave(cx+(cx/2), cy, 80, 0, 64, false);
            break;
          default:
            break;
          
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
