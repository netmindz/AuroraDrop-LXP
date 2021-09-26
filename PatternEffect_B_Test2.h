#ifndef PatternEffectTest2_H
#define PatternEffectTest2_H

class PatternEffectTest2 : public Drawable {
private:


public:
    PatternEffectTest2() {
      name = (char *)"Effect Test 2";
      id = "B";
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

        effects.SpiralStream(31, 15, 64, 128);        // for 64 pixel wide matrix!
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
