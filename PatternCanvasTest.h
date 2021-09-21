#ifndef PatternCanvasTest_H
#define PatternCanvasTest_H

class PatternCanvasTest : public Drawable {
  private:

    uint8_t color1 = 0;
    uint8_t color2 = 64;
    uint8_t color3 = 128;
    uint8_t color4 = 192;

    uint8_t canvasWidth;
    uint8_t canvasCentreX;
    uint8_t canvasCentreY;

    uint8_t colorEffect = 0;              // use palette or special (fire, ice, etc.)
    bool cycleColors;                     // todo
    bool insideOut = false;               // todo
    int8_t spin = 1;                      // 1/-1=rotate, 0=no rotate
    int spinVal = 0;                      // value to rotate by
    bool caleidoscope = false;            // drawn on quarter scrren and apply caleidoscope effect
    uint8_t caleidoscopeEffect = 0;       // which caleidoscope effect to apply



  public:


    PatternCanvasTest() {
      name = (char *)"Canvas Test";
    }

    // #------------- START -------------#
    void start(uint8_t _order) {
      // determine the colour effect to use. pallete or special (fire, ice, etc.)

      // randomise stuff
      cycleColors = random(0, 2);
      insideOut = random(0, 2);
      spin = random(0, 2); if (spin==0) spin = -1;      // 1=clockwise, -1 = anticlockwise, 0=no spin (disabled))
      if (spinVal==0) spinVal = random(0, 360);                   // start at random angle for the first time      ### USED TO BREAK STUFF? ###
      caleidoscope = random(0, 2);
      caleidoscopeEffect = random(0, 2);
      
      // override for testing
      cycleColors = false;        // does nothing just yet
      caleidoscope = false;
      //caleidoscopeEffect = 1;


      // setup parameters depending on random stuff
      if (caleidoscope) {
        // half size is commonly chosen, but sometimes others look good/weird
        // consider oscilating these once started
        float scale = 2;
        uint8_t rnd = random(0,12);   // half the time, use the standard, other half randomizes some scales
        switch (rnd) {
          case 0:
            scale = 1.25;
            break;
          case 1:
            scale = 1.5;
            break;
          case 2:
            scale = 1.75;
            break;
          case 3:
            scale = 2;
            break;
          case 4:
            scale = 2.5;
            break;
          case 5:
            scale = 3;
            break;
        }
        canvasWidth = MATRIX_WIDTH / scale;
        canvasCentreX = MATRIX_CENTER_X / scale;
        canvasCentreY = MATRIX_CENTER_Y / scale;
        //canvasWidth = canvasWidth / 2;                  // this was a weird bug but causes good effect!!!   now ocassionally scaling canvas randomly
        //canvasCentreX = canvasCentreX / 2;              // ditto
        //canvasCentreY = canvasCentreY / 2;              // ditto
      }
      else {
        canvasWidth = MATRIX_WIDTH;
        canvasCentreX = MATRIX_CENTER_X;
        canvasCentreY = MATRIX_CENTER_Y;
      }


    };



    // #------------- DRAW FRAME -------------#
    unsigned int drawFrame(uint8_t _order, uint8_t _total) {
      // _order indicates the order in which the effect is drawn, use the appropriate pre and post effects, if any

      // #---------------- NEW CANVAS TEST ---------------- //
      // clearcanvas, draw line in half width canvas, copy to random places on screen

      effects.ClearCanvas();        // clear all canvases

      /*
      int a1 = 0;
      int b1 = 0;
      int a2 = 31;
      int b2 = 31;
      effects.BresenhamLineCanvasH(a1, b1, a2, b2, effects.ColorFromCurrentPalette(128));
      a1 = 31;
      b1 = 0;
      a2 = 0;
      b2 = 31;
      effects.BresenhamLineCanvasH(a1, b1, a2, b2, effects.ColorFromCurrentPalette(128));


      effects.ApplyCanvasH(0, 10, 0.5);
      effects.ApplyCanvasH(32, 22, 0.5);
      effects.ApplyCanvasH(32, 32, 0.5);
      */


      //for (int x=0; x<MATRIX_WIDTH / 2; x++) {
      //  for (int y=0; y<MATRIX_WIDTH / 2; y++) {
      //    leds[XY16(MATRIX_WIDTH - 1 - x, y)] = leds[XY16(x, y)];
      //  }
      //}




      static const uint8_t BINS = BARS;
      byte audioData;
      uint8_t maxData = 127;

      // good pair of effects for non caleioscope mode
      //effects.SpiralStream(31, 31, 64, 128);        // for 64 pixel wide matrix!
      //effects.DimAll(220);


      float radius = canvasWidth / 2;
      float ratio = 360.0 / BINS;        // rotate around 360 degrees
      float rotation = 0.0;
      float angle = 0;
      int x,y;

      // add spin to rotation value (0-360)
      spinVal += spin;
      if (spinVal > 360) spinVal = 0;
      if (spinVal < 0) spinVal = 360;


      // # -------------- NEW CANVAS METHOD ------------------ #

      uint8_t canvasScale = 2;
      uint8_t audioScale = canvasScale;
      uint8_t centreX = (MATRIX_WIDTH  / 2) / canvasScale;
      uint8_t centreY = (MATRIX_HEIGHT  / 2) / canvasScale;
      for (int i=0; i<BINS; i++) {
        audioData = serialData.specData[i] / 6;
        if (audioData>maxData) audioData = maxData;
        audioData = audioData / audioScale;
        // the radius of the circle is the data value
        radius = audioData;
        rotation = i * ratio;
        rotation = rotation + spinVal;
        if (rotation > 360.0) rotation = rotation - 360.0;
        // calculate angle, then xy
        angle = rotation * 3.14 / 180;
        x = (int)(centreX + radius * cos(angle));
        y = (int)(centreY + radius * sin(angle));
        if (audioData > 0)
          effects.BresenhamLineCanvasH(centreX, centreY, x, y, effects.ColorFromCurrentPalette(i*2.65,255));
      }

      //effects.ApplyCanvasH(32, effects.p[2] / 2, 1);
      //effects.ApplyCanvasH(effects.p[2] / 2, 32, 1);

      effects.MoveOscillators();

      effects.ApplyCanvasH(-32, -32, 4.0);


      // # --------------------------------------------------- #


/*

      for (int i=0; i<BINS; i++) {
        audioData = serialData.specData[i] / 6;
        if (audioData>maxData) audioData = maxData;
        // scale data if in quarter screen
        if (caleidoscope) audioData = audioData /2;
        // the radius of the circle is the data value
        radius = audioData;
        rotation = i * ratio;
        rotation = rotation + spinVal;
        if (rotation > 360.0) rotation = rotation - 360.0;
        // calculate angle, then xy
        angle = rotation * 3.14 / 180;
        x = (int)(canvasCentreX + radius * cos(angle));
        y = (int)(canvasCentreY + radius * sin(angle));
        if (audioData > 0)
          effects.BresenhamLine(canvasCentreX, canvasCentreY, x, y, effects.ColorFromCurrentPalette(i*2.65,255));
        // brighter dots at end of lines this can look bad on quarter screen
        CRGB color = CRGB::White;
        if (!caleidoscope) effects.leds[XY(x, y)] = color;
      }
*/


      
      if (caleidoscope) {
        effects.Caleidoscope1Centre();          // default for now
        switch (caleidoscopeEffect) {
          case 0:
            break;
          case 1:
            break;
        }
      }



    // really good pair of effects
    //effects.SpiralStream(31, 31, 64, 128);        // for 64 pixel wide matrix!
    //effects.DimAll(220);


    effects.DimAll(220);


    return 0;
  }
};

#endif
