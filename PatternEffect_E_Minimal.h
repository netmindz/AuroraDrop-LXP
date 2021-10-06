#ifndef PatternEffectMinimal_H
#define PatternEffectMinimal_H

class PatternEffectMinimal : public Drawable {
private:


public:
    PatternEffectMinimal() {
      name = (char *)"Effect Minimal";
      id = "E";
    }


    uint8_t zzzz = 0;

    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern) {
        
        effects.NoiseVariablesSetup();
        
    }

    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
        zzzz++;
        if (zzzz > MATRIX_CENTER_X) zzzz = 0;

        // test 1 pretty shitty
        //effects.Expand(MATRIX_CENTRE_X, MATRIX_CENTRE_Y, zzzz, 128);

        // test 2 - OK
        //effects.StreamRight(128);  // 128 = 128

        // test 3 - wonky?
        //effects.StreamLeft(128);

        // test 4 - OK
        //effects.StreamDown(128);

        // test 5 - OK
        //effects.StreamUp(128);

        // test 6 - OK
        //effects.StreamUpAndLeft(128);

        // test 7 - OK
        //effects.StreamUpAndRight(128);

        // test 8
        effects.MoveDown();

        // test 9 - buggered
        //effects.RotateTriangle();

        // test 10 - nothing happens?
        //effects.FillNoise();



        // manipulate the screen buffer
        // with fixed parameters (could be oszillators too)
        // Params: center x, y, radius, scale color down
        // --> NOTE: Affects always a SQUARE with an odd length
     //   effects.SpiralStream(15, 15, 10, 128);

        //effects.SpiralStream(31, 15, 64, 128);        // for 64 pixel wide matrix!
        //effects.SpiralStreamVer2(31, 15, 64, 128);        // for 64 pixel wide matrix!
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
