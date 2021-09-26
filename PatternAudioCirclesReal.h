

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
      id = (char *)"R";
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

      float radius1 = MATRIX_WIDTH / 2;
      float radius2 = MATRIX_WIDTH / 2;
      float radius3 = MATRIX_WIDTH / 2;

      byte data1;
      byte data2;
      byte data3;

      for (int c = 0; c < CIRCLE_COUNT; c++){

        //testCircle[c].theta1++;
        //testCircle[c].theta1 = serialData.specData8[c+1];
        data1 = serialData.specData8[c+1] / 4;
        if (data1 > MATRIX_WIDTH - 1) data1 = MATRIX_WIDTH - 1;
        if (insideOut) data1 = (MATRIX_WIDTH - 1) - data1;

        data2 = serialData.specData8[c+2] / 4;
        if (data2 > MATRIX_WIDTH - 1) data2 = MATRIX_WIDTH - 1;
        if (insideOut) data2 = (MATRIX_WIDTH - 1) - data2;

        data3 = serialData.specData8[c+3] / 4;
        if (data3 > MATRIX_WIDTH - 1) data3 = MATRIX_WIDTH - 1;
        if (insideOut) data3 = (MATRIX_WIDTH - 1) - data3;




      for (int c = 0; c < CIRCLE_COUNT; c++) {

        radius1 = data1;
        radius2 = data2;
        radius3 = data3;
        
        for (float i = 0.0; i < 360.0; i += 6) { // ((MATRIX_WIDTH - data1) / 8)) {   // 0.1
            float angle = i * 3.14 / 180;
            int x = (int)(MATRIX_CENTER_X + radius1 * cos(angle));
            int y = (int)(MATRIX_CENTER_Y + radius1 * sin(angle));
            if (caleidoscopeMode) {
              x = x / 2;
              y = y / 2;
            }
            CRGB color = effects.ColorFromCurrentPalette(data1 * 1);
            effects.leds[XY(x, y)] = color;
          }

        for (float i = 0.0; i < 360.0; i += 6) { // ((MATRIX_WIDTH - data2) / 8)) {   // 0.1
            float angle = i * 3.14 / 180;
            int x = (int)(MATRIX_CENTER_X + radius2 * cos(angle));
            int y = (int)(MATRIX_CENTER_Y + radius2 * sin(angle));
            if (caleidoscopeMode) {
              x = x / 2;
              y = y / 2;
            }
            CRGB color = effects.ColorFromCurrentPalette(data2 * 2);
            effects.leds[XY(x, y)] = color;
          }

        for (float i = 0.0; i < 360.0; i += 6) { // ((MATRIX_WIDTH - data3) / 8)) {   // 0.1
            float angle = i * 3.14 / 180;
            int x = (int)(MATRIX_CENTER_X + radius3 * cos(angle));
            int y = (int)(MATRIX_CENTER_Y + radius3 * sin(angle));
            if (caleidoscopeMode) {
              x = x / 2;
              y = y / 2;
            }
            CRGB color = effects.ColorFromCurrentPalette(data3 * 3);
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

    effects.DimAll(230);

    return 0;
  }
};

#endif
