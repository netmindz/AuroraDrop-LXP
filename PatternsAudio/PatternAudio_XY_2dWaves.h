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

#ifndef PatternAudio2dWaves_H
#define PatternAudio2dWaves_H

class PatternAudio2dWaves : public Drawable {
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
    int direction = 1;

    bool effect1;

  public:
    PatternAudio2dWaves() {
      name = (char *)"2D Waves";
      id = "T";
      enabled = true;
    }


    // ------------------ start -------------------
    void start(uint8_t _pattern) {

        hue_ms = millis();

        // randomise
        hueCycle = random8(0,5);          // mostly cycle through the the hues
        hue = random8(0,255);

        effect1 = random8(0,2);

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
        int audio;
        int oldaudio = 0;

        //effects.DimAll(200);

        if (!fftData.noAudio) {

            drawHorizontal = true;

            if (drawHorizontal) {
                
                x = 0;
                y = 8;
                for (int i=0; i<128; i++) {
                    if (x != 0) {
                        audio = direction * (fftData.specData[i] / 16);
                        effects.BresenhamLine(oldx, oldy, x, y-audio, hue + (i*2), 255);
                        oldx = x;
                        oldy = y-audio;

                    }
                    else {
                        oldx = x;
                        oldy = y;
                    }

                    if (audio != oldaudio || !effect1) direction = -direction;
                    oldaudio = audio;

                    x = x + 2;
                    if (x > MATRIX_WIDTH - 1) {
                        x = 0;
                        y = y + 16;
                    }
                }
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
