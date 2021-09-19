

#ifndef PatternAudioCirclesReal_H
#define PatternAudioCirclesReal_H

class TestSimpleCircle{
public:
    char* name;

    byte theta1 = 0;
    byte theta2 = 0;
    byte hueoffset = 0;

    uint8_t radius = MATRIX_WIDTH / 4;

};


class PatternAudioCirclesReal : public Drawable {
  private:

    static const uint8_t CIRCLE_COUNT = 3;
    TestSimpleCircle testCircle[CIRCLE_COUNT];


    //byte theta1 = 0;
    //byte theta2 = 0;
    //byte hueoffset = 0;

    bool cycleColors;
    uint8_t color1 = 0;
    uint8_t color2 = 64;
    uint8_t color3 = 128;
    uint8_t color4 = 192;


  public:
    PatternAudioCirclesReal() {
      name = (char *)"Audio Circles Real";
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
    uint8_t caleidoscopeModeB = 0;


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){
      cycleColors = random(0, 2);
      caleidoscopeMode = random(0, 2);
      caleidoscopeModeB = random(0, 2);
      insideOut = random(0, 2);

      
      // override for testing
      cycleColors = false;
      caleidoscopeMode = 1;
      caleidoscopeModeB = 1;
      insideOut = false;
    };



    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

      boolean change = false;

      float radius = MATRIX_WIDTH / 2;

      byte data;

      for (int c = 0; c < CIRCLE_COUNT; c++){

        //testCircle[c].theta1++;
        //testCircle[c].theta1 = serialData.specData8[c+1];
        data = serialData.specData8[c+1] / 4;
        if (data > MATRIX_WIDTH - 1) data = MATRIX_WIDTH - 1;
        if (insideOut) data = (MATRIX_WIDTH - 1) - data;
        //testCircle[c].theta1 = data;





      for (int c = 0; c < CIRCLE_COUNT; c++) {

        radius = data;
        
        for (float i = 0.0; i < 360.0; i += ((MATRIX_WIDTH - data) / 8)) {   // 0.1
            float angle = i * 3.14 / 180;
            int x = (int)(MATRIX_CENTRE_X + radius * cos(angle));
            int y = (int)(MATRIX_CENTRE_Y + radius * sin(angle));
            if (caleidoscopeMode) {
              /*
              if (caleidoscopeModeB) {
                x = (x / 2) + (MATRIX_WIDTH / 4);
                y = (y / 2);
              }
              else {
                x = x / 2;
                y = y / 2;
              }
              */
              x = x / 2;
              y = y / 2;
            }

            CRGB color = CRGB::Purple;
            /*
            if (c==0)
              color = CRGB::Orange; // dma_display->color565(c*255, c*32, 64);
            if (c==1)
              color = CRGB::Green; // dma_display->color565(c*255, c*32, 64);
            if (c==2)
              color = CRGB::Cyan; // dma_display->color565(c*255, c*32, 64);
            */

            effects.leds[XY(x, y)] = color;

            }


      }




      }




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

    effects.DimAll(150);

    return 0;
  }
};

#endif
