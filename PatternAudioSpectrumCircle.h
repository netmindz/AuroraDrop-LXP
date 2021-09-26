#ifndef PatternAudioSpectrumCircle_H
#define PatternAudioSpectrumCircle_H

class PatternAudioSpectrumCircle : public Drawable {
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
    bool spin = true;                     // rotate yes/no
    int spinVal = 0;                      // value to rotate by
    bool caleidoscope = false;            // drawn on quarter scrren and apply caleidoscope effect
    uint8_t caleidoscopeEffect = 0;       // which caleidoscope effect to apply



  public:


    PatternAudioSpectrumCircle() {
      name = (char *)"Audio Spectrum Circle";
      id = (char *)"f";
    }

    // #------------- START -------------#
    void start(uint8_t _order) {
      // determine the colour effect to use. pallete or special (fire, ice, etc.)

      // randomise stuff
      cycleColors = random(0, 2);
      insideOut = random(0, 2);
      spin = true;
      if (spinVal==0) spinVal = random(0, 360);                   // start at random angle for the first time      ### USED TO BREAK STUFF? ###
      caleidoscope = random(0, 2);
      caleidoscopeEffect = random(0, 2);
      
      // override for testing
      cycleColors = false;        // does nothing just yet
      //caleidoscope = true;
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

      //static const uint8_t BINS = BINS;
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
      spinVal++;
      if (spinVal > 360) spinVal = 0;

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
