#ifndef PatternEffectSpiralStream1_H
#define PatternEffectSpiralStream1_H

class PatternEffectSpiralStream1 : public Drawable {
private:

  bool addBlur;
  bool dimAll;
  bool scrollStream;
  uint8_t dimAmount;
  uint8_t scaleColorDown;
  uint8_t streamRadius;
  uint8_t centerX = MATRIX_CENTER_X - 1;
  uint8_t centerY = MATRIX_CENTER_X - 1;


public:
    PatternEffectSpiralStream1() {
      name = (char *)"Effect Spiral Stream 1";
      id = "B";
    }


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern) {

      addBlur = random8(0,1);                             // don't need blur here
      dimAll = random8(0, 10);                            // 90% chance of dimming the output render
      scrollStream = random8(1,2);
      dimAmount = random8(250, 251); 
      scaleColorDown = random8(128, 129); 
      streamRadius = random8(1, 6);
      switch (streamRadius)
      {
        case 0:
        case 1:
        case 2:
        case 3:
          streamRadius = MATRIX_WIDTH / 2;
          break;
        case 4:
          streamRadius = MATRIX_WIDTH / 3;
          break;
        case 5:
          streamRadius = MATRIX_WIDTH / 4;
          break;
      }

      // reset centre if not scrolling the stream
      if (!scrollStream)
      {
        centerX = MATRIX_CENTER_X - 1;
        centerY = MATRIX_CENTER_Y - 1;   
      }
        
    }

    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

        if (dimAll)
        {
          effects.DimAll(dimAmount);
        }

        // Params: center x, y, radius, scale color down
        // --> NOTE: Affects always a SQUARE with an odd length
        //effects.SpiralStream(31, 31, 32, 128);        // for 64 pixel wide matrix!

        // dim some extra to account for any unstreamed areas
        if (streamRadius != MATRIX_WIDTH / 2) {
          effects.DimAll(dimAmount);
        }
        effects.SpiralStream(centerX, centerY, streamRadius, scaleColorDown);        // for 64 pixel wide matrix!

        // don't use blur for the moment
        if (addBlur) 
        {
          blur2d(effects.leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, 255);
        }


      if (scrollStream)
      {
        centerX++;
        if (centerX > MATRIX_WIDTH - 1) centerX = 0;
      }


        return 0;
    }
};

#endif
