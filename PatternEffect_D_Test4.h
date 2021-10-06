#ifndef PatternEffectTest4_H
#define PatternEffectTest4_H

class PatternEffectTest4 : public Drawable {
private:

  uint8_t direction;


public:
    PatternEffectTest4() {
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
