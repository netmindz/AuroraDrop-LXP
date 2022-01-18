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

#ifndef PatternAudio2dGrid_H
#define PatternAudio2dGrid_H

class PatternAudio2dGrid : public Drawable {
  private:

    bool hueCycle = true;
    byte hue = 0;
    uint32_t hue_ms;

    bool drawVertical;
    bool drawHorizontal;
    bool drawDiagonal;
    bool drawDoubleVerticalLines;
    bool drawDoubleHorizontalLines;
    bool drawDoubleDiagonalLines1;
    bool drawDoubleDiagonalLines2;

  public:
    PatternAudio2dGrid() {
      name = (char *)"2D Grid";
      id = "T";
      enabled = true;
    }


    // ------------------ start -------------------
    void start(uint8_t _pattern) {

        hue_ms = millis();

        // randomise
        hueCycle = random8(0,5);          // mostly cycle through the the hues
        hue = random8(0,255);

        drawDoubleVerticalLines = random8(0,2);
        drawDoubleHorizontalLines = random8(0,2);
        drawDoubleDiagonalLines1 = random8(0,2);
        drawDoubleDiagonalLines2 = random8(0,2);
        drawVertical = random8(0,2);
        drawHorizontal = random8(0,2);
        drawDiagonal = random8(0,2);
        if (!drawVertical && !drawHorizontal && !drawDiagonal) drawVertical = true;

        //drawVertical = false;
        //drawHorizontal = false;
        //drawDiagonal = true;
    }

    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

        int x, y;
        int oldx = 0, oldy = 0;
        int xStart = 12;
        int yStart = 17;
        byte audio;

        //effects.DimAll(200);

        if (!fftData.noAudio) {


            if (drawVertical) {
                x = 0;
                y = 10;
                for (int i=0; i<120; i++) {

                    audio = fftData.specData[i] / 16;
                    effects.BresenhamLine(x, y, x, y-audio, hue + (i*2), 128);
                    if (drawDoubleVerticalLines) effects.BresenhamLine(x+1, y, x+1, y-audio, hue + (i*2), 128);

                    x = x + 4;
                    if (x > MATRIX_WIDTH - 1) {
                        x = 0;
                        y = y + 9;
                    }
                }
            }

            if (drawHorizontal) {
                x = 1;
                y = 0;
                for (int i=0; i<120; i++) {

                    audio = fftData.specData[i] / 16;
                    effects.BresenhamLine(x, y, x+audio, y, hue + (i*2), 128);
                    if (drawDoubleHorizontalLines) effects.BresenhamLine(x, y+1, x+audio, y+1, hue + (i*2), 128);

                    y = y + 4;
                    if (y > MATRIX_HEIGHT - 1) {
                        y = 0;
                        x = x + 9;
                    }
                }
            }

            if (drawDiagonal) {
                x = 0;
                y = 10;
                for (int i=0; i<120; i++) {

                    audio = fftData.specData[i] / 16;
                    effects.BresenhamLine(x, y, x+audio, y-audio, hue + (i*2), 128);
                    if (drawDoubleDiagonalLines1) effects.BresenhamLine(x, y+1, x+audio, y-audio+1, hue + (i*2), 128);
                    if (drawDoubleDiagonalLines2) effects.BresenhamLine(x+1, y+1, x+audio+1, y-audio+1, hue + (i*2), 128);

                    x = x + 4;
                    if (x > MATRIX_WIDTH - 1) {
                        x = 0;
                        y = y + 9;
                    }
                }
            }



            oldx = x;
            oldy = y-audio;

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
