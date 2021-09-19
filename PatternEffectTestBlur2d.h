/*
 * Portions of this code are adapted from "Funky Clouds" by Stefan Petrick:
 * https://gist.github.com/anonymous/876f908333cd95315c35
 *
 * Copyright (c) 2014 Stefan Petrick
 * http://www.stefan-petrick.de/wordpress_beta
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

#ifndef PatternEffectTestBlur2d_H
#define PatternEffectTestBlur2d_H

class PatternEffectTestBlur2d : public Drawable {
private:


public:
    PatternEffectTestBlur2d() {
        name = (char *)"Effect Test 2";
    }


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern) {

        
    }

    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

        // manipulate the screen buffer
        // with fixed parameters (could be oszillators too)
        // Params: center x, y, radius, scale color down
        // --> NOTE: Affects always a SQUARE with an odd length
     //   effects.SpiralStream(15, 15, 10, 128);

        //blur2d(effects.leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, 192);

        blur2d(effects.leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, 255);
        //blur2d(effects.leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, serialData.spec255Data[32] * 2 > 255 ? 255 : serialData.spec255Data[32] * 2);

      //  effects.SpiralStream(47, 15, 10, 128);        // for 64 pixel wide matrix!

        // why not several times?!
       // effects.SpiralStream(16, 6, 6, 128);
       // effects.SpiralStream(10, 24, 10, 128);

        // increase the contrast
        effects.DimAll(250);
        //////effects.ShowFrame();

        return 0;
    }
};

#endif
