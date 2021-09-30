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

#ifndef PatternStaticSimpleStars_H
#define PatternStaticSimpleStars_H

class PatternStaticSimpleStars : public Drawable {
  private:
    Boid starBoids[AVAILABLE_BOID_COUNT];

    uint32_t hue_ms;


  public:
    PatternStaticSimpleStars() {
      name = (char *)"Static X - Simple Stars";
      id = (char *)"X";
    }

    uint16_t x;
    uint16_t y;
    uint16_t z;

    uint16_t speed = 1;
    uint16_t scale = 26;

    static const int count = 40;

    byte hue = 0;

    // ------------------ START PATTERN -------------------
    void start(uint8_t _pattern) {

      x = random16();
      y = random16();
      z = random16();
      hue_ms = millis();

      for (int i = 0; i < count; i++) {
        starBoids[i] = Boid(random(MATRIX_WIDTH), random(MATRIX_HEIGHT));
      }
    }

    // ------------------ DRAW FRAME --------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {


      //effects.DimAll(240); 

      // CRGB color = effects.ColorFromCurrentPalette(hue);

      for (int i = 0; i < count; i++) {
        Boid * boid = &starBoids[i];

        int ioffset = scale * boid->location.x;
        int joffset = scale * boid->location.y;

        byte angle = inoise8(x + ioffset, y + joffset, z);

        boid->velocity.x = (float) sin8(angle) * 0.0078125 - 1.0;
        //boid->velocity.y = -((float)cos8(angle) * 0.0078125 - 1.0);
        boid->update();

        effects.drawBackgroundFastLEDPixelCRGB(boid->location.x, boid->location.y, effects.ColorFromCurrentPalette(angle + hue)); // color

        if (boid->location.x < 0 || boid->location.x >= MATRIX_WIDTH ||
            boid->location.y < 0 || boid->location.y >= MATRIX_HEIGHT) {
          boid->location.x = random(MATRIX_WIDTH);
          boid->location.y = random(MATRIX_HEIGHT);
        }
      }

      if (hue_ms + 200 < millis()) 
      {
        hue_ms = millis();
        hue++;
      }

      //EVERY_N_MILLIS(200) {
      //  hue++;
      //}

      x += speed;
      //y += speed;
      z += speed;

      //////effects.ShowFrame();

      return 50;
    }
};

#endif
