#ifndef PatternEffectStream1_H
#define PatternEffectStream1_H

class PatternEffectStream1 : public Drawable {
private:

  uint8_t streamDirection;
  uint8_t scaleColorDown;



public:
  PatternEffectStream1() {
    name = (char *)"Effect Stream 1";
    id = "C";
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
