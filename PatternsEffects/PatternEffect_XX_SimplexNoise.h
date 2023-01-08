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

#ifndef PatternEffectSimplexNoise_H
#define PatternEffectSimplexNoise_H

class PatternEffectSimplexNoise : public Drawable {
private:


  public:
    PatternEffectSimplexNoise() {
      name = (char *)"Simplex Noise";
      id = "T";
      enabled = false;
    }


    // ------------------ start -------------------
    void start(uint8_t _pattern) {
            // Initialize our coordinates to some random values
      noise_x = random16();
      noise_y = random16();
      noise_z = random16();

    }


    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      #if FASTLED_VERSION >= 3001000

        // a new parameter set every 15 seconds

        EVERY_N_SECONDS(15) {

          noise_x = random16();
          noise_y = random16();
          noise_z = random16();

        }

      #endif

      uint32_t speed = 100;

      effects.FillNoise();
      ShowNoiseLayer(0, 1, 0);

      // noise_x += speed;
      noise_y += speed;
      noise_z += speed;

      //effects.ShowFrame();

      //return 30;


      return 0;
      
    }

    // show just one layer
  void ShowNoiseLayer(byte layer, byte colorrepeat, byte colorshift) {

    for (uint16_t i = 0; i < MATRIX_WIDTH; i++) {

      for (uint16_t j = 0; j < MATRIX_HEIGHT; j++) {


        uint8_t color = noise[i][j];

        uint8_t bri = color;

        // assign a color depending on the actual palette
        CRGB pixel = ColorFromPalette(effects.currentPalette, colorrepeat * (color + colorshift), bri);

        effects.leds[XY16(i, j)] = pixel;

      }

    }

  }

};

#endif
