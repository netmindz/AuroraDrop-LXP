

#ifndef PatternAudioCircles_H
#define PatternAudioCircles_H

class TestCircle{
public:
    char* name;

    byte theta1 = 0;
    byte theta2 = 0;
    byte hueoffset = 0;

    uint8_t radiusx = MATRIX_WIDTH / 4;
    uint8_t radiusy = MATRIX_HEIGHT / 4;
    uint8_t minx = MATRIX_CENTER_X - radiusx;
    uint8_t maxx = MATRIX_CENTER_X + radiusx + 1;
    uint8_t miny = MATRIX_CENTER_Y - radiusy;
    uint8_t maxy = MATRIX_CENTER_Y + radiusy + 1;

    uint8_t spirocount = 32;
    uint8_t spirooffset = 256 / spirocount;
    boolean spiroincrement = true;

    boolean change = false;

};


class PatternAudioCircles : public Drawable {
  private:

    static const uint8_t CIRCLE_COUNT = 3;
    TestCircle testCircle[CIRCLE_COUNT];


    //byte theta1 = 0;
    //byte theta2 = 0;
    //byte hueoffset = 0;

    bool cycleColors;
    uint8_t color1 = 0;
    uint8_t color2 = 64;
    uint8_t color3 = 128;
    uint8_t color4 = 192;


  public:
    PatternAudioCircles() {
      name = (char *)"Audio Circles";
    }



    //uint8_t radiusx = MATRIX_WIDTH / 4;
    //uint8_t radiusy = MATRIX_HEIGHT / 4;
    //uint8_t minx = (MATRIX_CENTER_X - radiusx) / 2;
    //uint8_t maxx = MATRIX_CENTER_X + radiusx + 1;
    //uint8_t miny = (MATRIX_CENTER_Y - radiusy) / 2;
    //uint8_t maxy = MATRIX_CENTER_Y + radiusy + 1;

    //uint8_t spirocount = 32;
    //uint8_t spirooffset = 256 / spirocount;
    //boolean spiroincrement = true;


    bool insideOut = false;
    uint8_t caleidoscopeMode = 0;


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){
      cycleColors = random(0, 2);
      caleidoscopeMode = random(0, 2);
      insideOut = random(0, 2);

      
      // override for testing
      cycleColors = false;
      caleidoscopeMode = 1;

      for (int c = 0; c < CIRCLE_COUNT; c++){

      if (caleidoscopeMode == 1) {
        testCircle[c].radiusx = (MATRIX_WIDTH / 4) / 2;
        testCircle[c].radiusy = (MATRIX_HEIGHT / 4) /2;
        testCircle[c].minx = (MATRIX_CENTER_X /2) - testCircle[c].radiusx;
        testCircle[c].maxx = (MATRIX_CENTER_X /2) + testCircle[c].radiusx + 1;
        testCircle[c].miny = (MATRIX_CENTER_Y /2) - testCircle[c].radiusy;
        testCircle[c].maxy = (MATRIX_CENTER_Y /2) + testCircle[c].radiusy + 1;

        testCircle[c].spirocount = 32;
        testCircle[c].spirooffset = 256 / testCircle[c].spirocount;
        testCircle[c].spiroincrement = true;        
      }
      else {
        testCircle[c].radiusx = MATRIX_WIDTH / 4;
        testCircle[c].radiusy = MATRIX_HEIGHT / 4;
        testCircle[c].minx = (MATRIX_CENTER_X - testCircle[c].radiusx) / 2;
        testCircle[c].maxx = MATRIX_CENTER_X + testCircle[c].radiusx + 1;
        testCircle[c].miny = (MATRIX_CENTER_Y - testCircle[c].radiusy) / 2;
        testCircle[c].maxy = MATRIX_CENTER_Y + testCircle[c].radiusy + 1;

        testCircle[c].spirocount = 32;
        testCircle[c].spirooffset = 256 / testCircle[c].spirocount;
        testCircle[c].spiroincrement = true;        
      }

      }


    };



    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      boolean change = false;


      byte data;

      for (int c = 0; c < CIRCLE_COUNT; c++){

        //testCircle[c].theta1++;
        //testCircle[c].theta1 = serialData.specData8[c+1];
        data = serialData.specData8[c+1];
        if (data > 127) data = 127;
        data = data * 2;
        if (insideOut) data = 255 - data;
        testCircle[c].theta1 = data;

        for (int i = 0; i < testCircle[c].spirocount; i++) {
          uint8_t x0 = mapsin8(testCircle[c].theta1 + i * testCircle[c].spirooffset, testCircle[c].minx, testCircle[c].maxx);
          uint8_t y0 = mapcos8(testCircle[c].theta1 + i * testCircle[c].spirooffset, testCircle[c].miny, testCircle[c].maxy);

          uint8_t x1 = mapsin8(testCircle[c].theta2 + i * testCircle[c].spirooffset, x0 - testCircle[c].radiusx, x0 + testCircle[c].radiusx);
          uint8_t y1 = mapcos8(testCircle[c].theta2 + i * testCircle[c].spirooffset, y0 - testCircle[c].radiusy, y0 + testCircle[c].radiusy);

          CRGB color = effects.ColorFromCurrentPalette(testCircle[c].hueoffset + i * testCircle[c].spirooffset, 128);
          // test colors
          //color = dma_display->color565(128+(i*32), 128+(i*32), 128+(i*32));
          color = effects.ColorFromCurrentPalette(data * 2);; // dma_display->color565(16*i, 32*i, 64*i);

          if (serialData.specData8[c+1] > 0)
            effects.leds[XY(x1, y1)] += color;
        
          if((x1 == MATRIX_CENTER_X && y1 == MATRIX_CENTER_Y) ||
             (x1 == MATRIX_CENTER_X && y1 == MATRIX_CENTER_Y)) testCircle[c].change = true;

          // randomise this?
          //testCircle[c].theta2++;


        }

      }







      /* old method

      theta1++;
      theta1 = serialData.specData8[3];

      if (cycleColors) {
        color1++;
        color2++;
        color3++;
        color4++;
      }
      else {
        color1 = 0;
        color2 = 64;
        color3 = 128;
        color4 = 192;
      }

      for (int i = 0; i < spirocount; i++) {
        uint8_t x0 = mapsin8(theta1 + i * spirooffset, minx, maxx);
        uint8_t y0 = mapcos8(theta1 + i * spirooffset, miny, maxy);

        uint8_t x1 = mapsin8(theta2 + i * spirooffset, x0 - radiusx, x0 + radiusx);
        uint8_t y1 = mapcos8(theta2 + i * spirooffset, y0 - radiusy, y0 + radiusy);

        CRGB color = effects.ColorFromCurrentPalette(hueoffset + i * spirooffset, 128);
        effects.leds[XY(x1, y1)] += color;
        
        if((x1 == MATRIX_CENTER_X && y1 == MATRIX_CENTER_Y) ||
           (x1 == MATRIX_CENTRE_X && y1 == MATRIX_CENTRE_Y)) change = true;
      }

      */


    switch (caleidoscopeMode) {
      case 0:
        effects.Caleidoscope1();
        break;
      case 1:
        effects.Caleidoscope2();  // 2 = good
        break;
      case 2:
        effects.Expand(31,31,16,250);  // 1 = good
        break;
      default:
        break;
    }



    //effects.SpiralStream(31, 15, 64, 128);        // for 64 pixel wide matrix!

    effects.DimAll(250);

    return 0;
  }
};

#endif
