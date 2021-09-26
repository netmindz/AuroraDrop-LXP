#ifndef PatternEffectTestBlur2d_H
#define PatternEffectTestBlur2d_H

class PatternEffectTestBlur2d : public Drawable {
private:


public:
    PatternEffectTestBlur2d() {
      name = (char *)"Effect Test 2";
      id = "A";
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
