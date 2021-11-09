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

#ifndef PatternEffectPlasma_H
#define PatternEffectPlasma_H

class PatternEffectPlasma : public Drawable {
private:
    int time = 0;
    int cycles = 0;
    uint8_t brightness;

  public:
    PatternEffectPlasma() {
      name = (char *)"Plasma";
      id = "P";
      enabled = true;
    }


    // ------------------ start -------------------
    void start(uint8_t _pattern) {
      // do some randomsing here sometimes
      brightness = 8;
    }


    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

        if (brightness < 255) brightness++;

        for (int x = 0; x < MATRIX_WIDTH; x++) {
            for (int y = 0; y < MATRIX_HEIGHT; y++) {
                int16_t v = 0;
                uint8_t wibble = sin8(time);
                v += sin16(x * wibble * 2 + time);
                v += cos16(y * (128 - wibble) * 2 + time);
                v += sin16(y * x * cos8(-time) / 2);

                // fade plasma effect in gently
                effects.Pixel(x, y, (v >> 8) + 127, brightness);
            }
        }

        //effects.Caleidoscope3();      // not bad
        //effects.Caleidoscope1();

        effects.Caleidoscope2();        // not bad

        //effects.Caleidoscope1_BottomRight();      // needs work

        time += 1;
        cycles++;

        if (cycles >= 2048) {
            time = 0;
            cycles = 0;
        }

        //effects.ShowFrame();

        return 0;  //was 30

    }


};

#endif
