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

#ifndef PatternEffectStream1_H
#define PatternEffectStream1_H

class PatternEffectStream1 : public Drawable {
private:

  uint8_t streamDirection;
  uint8_t scaleColorDown;



public:
  PatternEffectStream1() {
    name = (char *)"Directional Stream";
    id = "C";
    enabled = true;
  }

  // ---------------- START ----------------
  void start(uint8_t _pattern) {
        
    effects.NoiseVariablesSetup();

    streamDirection = random8(5,6);
    scaleColorDown = random8(128, 129); 
  }

    // ------------- DRAW FRAME -------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

        //effects.DimAll(250);

        switch (streamDirection)
        {
          case 0:
            effects.StreamUp(scaleColorDown);
            break;
          case 1:
            effects.StreamDown(scaleColorDown);
            break;
          case 2:
            effects.StreamLeft(scaleColorDown);
            break;
          case 3:
            effects.StreamRight(scaleColorDown);
            break;
          case 4:
            effects.StreamUpAndRight(scaleColorDown);
            break;
          case 5:
            effects.StreamUpAndLeft(scaleColorDown);
            break;
          default:
            effects.StreamDown(scaleColorDown);
            effects.StreamLeft(scaleColorDown);
            break;
        }


        return 0;
    }
};

#endif
