

#ifndef PatternTestCircles_H
#define PatternTestCircles_H

class PatternTestCircles : public Drawable {
  public:
    PatternTestCircles() {
      name = (char *)"Test Cirlces";
      id2 = 1;
    }

    uint8_t xxxx1 = 0;
    uint8_t xxxx2 = 64;
    uint8_t xxxx3 = 128;
    uint8_t xxxx4 = 192;

    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern){
    //effects.ClearFrame();
    };



    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
    //blur2d(effects.leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, 192);
    //effects.DimAll(250);


    //dma_display->drawCircle(31, 31, serialData.specData[10] / 6, dma_display->color444(0, 0, 15));;

/*
    xxxx1++;
    xxxx2++;
    xxxx3++;
    xxxx4++;
*/

/*
      dma_display->setCursor(1, 1);
      dma_display->setTextColor(dma_display->color444(15,0,0));
      dma_display->print('T');
      dma_display->print('h');
      dma_display->print('e');
      dma_display->print(' ');
      dma_display->setTextColor(dma_display->color444(0,15,0));
      dma_display->print('P');
      dma_display->print('i');
      dma_display->print('x');
      dma_display->setTextColor(dma_display->color444(0,0,15));
      dma_display->print("i");
      dma_display->print("e");
      dma_display->print("s");
*/

    int x1,y1,x2,y2,x3,y3;
    x1 = serialData.specData[05] / 3;
    y1 = serialData.specData[10] / 3;
    x2 = serialData.specData[15] / 3;
    y2 = serialData.specData[20] / 3;
    x3 = serialData.specData[25] / 3;
    y3 = serialData.specData[30] / 3;
    effects.BresenhamLine(x1, y1, x2, y2, effects.ColorFromCurrentPalette(xxxx1,255)); //  dma_display->color444(15, 0, 0)); // red

    //effects.RainbowTriangle();

    //effects.BresenhamLine(MATRIX_WIDTH - x1, y1, x2, y2, dma_display->color444(0, 0, 15)); // color
    //effects.BresenhamLine(x1, MATRIX_WIDTH - y1, x2, y2, dma_display->color444(0, 15, 0)); // color
    //effects.BresenhamLine(x1, y1, MATRIX_WIDTH - x2, y2, dma_display->color444(15, 0, 0)); // color
    //effects.BresenhamLine(x1, y1, x2, MATRIX_WIDTH - y2, dma_display->color444(15, 0, 15)); // color

    x1 = serialData.specData[25] / 3;
    y1 = serialData.specData[30] / 3;
    x2 = serialData.specData[35] / 3;
    y2 = serialData.specData[40] / 3;
    effects.BresenhamLine(x1, y1, x2, y2, effects.ColorFromCurrentPalette(xxxx2,255)); // green

    x1 = serialData.specData[45] / 3;
    y1 = serialData.specData[50] / 3;
    x2 = serialData.specData[55] / 3;
    y2 = serialData.specData[60] / 3;
    effects.BresenhamLine(x1, y1, x2, y2, effects.ColorFromCurrentPalette(xxxx3,255)); // green

    effects.Caleidoscope1();

    effects.SpiralStream(31, 15, 64, 128);        // for 64 pixel wide matrix!



    effects.DimAll(250);
    //////effects.ShowFrame();





/*
      uint8_t dim = beatsin8(2, 230, 250);
      //effects.DimAll(dim);   // TONY TEST COMMENT
      effects.DimAll(250);
      
      effects.ShowFrame();

      for (int i = 2; i <= MATRIX_WIDTH / 2; i++)
      {
        CRGB color = effects.ColorFromCurrentPalette((i - 2) * (240 / (MATRIX_WIDTH / 2)));

        uint8_t x = beatcos8((17 - i) * 2, MATRIX_CENTER_X - i, MATRIX_CENTER_X + i);
        uint8_t y = beatsin8((17 - i) * 2, MATRIX_CENTER_Y - i, MATRIX_CENTER_Y + i);

        effects.drawBackgroundFastLEDPixelCRGB(x, y, color);
      }
*/



      return 0;
    }
};

#endif
