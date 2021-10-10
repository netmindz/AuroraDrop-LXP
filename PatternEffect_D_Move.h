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

#ifndef PatternEffectMove_H
#define PatternEffectMove_H

class PatternEffectMove : public Drawable {
private:

  uint8_t direction;


public:
    PatternEffectMove() {
      name = (char *)"Effect Test 4";
      id = "D";
    }


    uint8_t zzzz = 0;

    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern) {
        
        effects.NoiseVariablesSetup();

        direction = random8(0,2);
        
    }

    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {


        effects.DimAll(200);

        switch (direction) 
        {
          case 0:
            effects.MoveY(1);
            effects.MoveX(1);
            break;
          case 1:
            effects.MoveDown();
            break;
          case 2:
            // not used
            effects.MoveY(2);
            effects.MoveX(1);
            break;
          case 3:
            // not used
            effects.MoveY(1);
            effects.MoveX(2);
            break;
            
        }

        // test 9 - buggered
        //effects.RotateTriangle();

        // test 10 - nothing happens?
        //effects.FillNoise();



       




        //////effects.ShowFrame();

        return 0;
    }
};

#endif
