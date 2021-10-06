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

        effects.DimAll(250);

        blur2d(effects.leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, 255);

        return 0;
    }
};

#endif
