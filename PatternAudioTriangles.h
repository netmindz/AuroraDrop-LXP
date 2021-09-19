

#ifndef PatternAudioTriangles_H
#define PatternAudioTriangles_H

class PatternAudioTriangles : public Drawable {
  public:
    PatternAudioTriangles() {
      name = (char *)"Audio Triangles";
    }

    bool cycleColors;
    uint8_t color1 = 0;
    uint8_t color2 = 64;
    uint8_t color3 = 128;
    uint8_t color4 = 192;

    uint8_t caleidoscopeMode = 0;


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){
      cycleColors = random(0, 2);
      caleidoscopeMode = random(0, 2);
      Serial.print("caleidoscopeMode="); Serial.println(caleidoscopeMode);
    };



    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

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

    // triangle
    int x1,y1,x2,y2,x3,y3;
    x1 = serialData.specData[0] / 3;
    y1 = serialData.specData[20] / 3;
    x2 = serialData.specData[40] / 3;
    y2 = serialData.specData[60] / 3;
    x3 = serialData.specData[80] / 3;
    y3 = serialData.specData[90] / 3;
    effects.BresenhamLine(x1, y1, x2, y2, effects.ColorFromCurrentPalette(color1,200)); //  dma_display->color444(15, 0, 0)); // red
    effects.BresenhamLine(x2, y2, x3, y3, effects.ColorFromCurrentPalette(color1,200)); // green
    effects.BresenhamLine(x3, y3, x1, y1, effects.ColorFromCurrentPalette(color1,200)); // green

    // second triangle
    x1 = serialData.specData[5] / 3;
    y1 = serialData.specData[25] / 3;
    x2 = serialData.specData[45] / 3;
    y2 = serialData.specData[65] / 3;
    x3 = serialData.specData[85] / 3;
    y3 = serialData.specData[95] / 3;
    effects.BresenhamLine(x1, y1, x2, y2, effects.ColorFromCurrentPalette(color2,200)); //  dma_display->color444(15, 0, 0)); // red
    effects.BresenhamLine(x2, y2, x3, y3, effects.ColorFromCurrentPalette(color2,200)); // green
    effects.BresenhamLine(x3, y3, x1, y1, effects.ColorFromCurrentPalette(color2,200)); // green


    switch (caleidoscopeMode) {
      case 0:
        effects.Caleidoscope1();
        break;
      case 1:
        effects.Caleidoscope2();  // 2 = good
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
