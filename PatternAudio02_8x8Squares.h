#ifndef PatternAudio8x8Squares_H
#define PatternAudio8x8Squares_H

class PatternAudio8x8Squares : public Drawable {
  private:

    uint8_t color1 = 0;
    uint8_t color2 = 64;
    uint8_t color3 = 128;
    uint8_t color4 = 192;

    uint8_t canvasWidth;
    uint8_t canvasCentreX;
    uint8_t canvasCentreY;

    // theses are mostly radomised at initial start
    uint8_t colorEffect = 0;              // TODO: use standard palette or special (fire, ice, etc.)
    bool cycleColors;                     // todo: cylce through the color spectrum
    bool insideOut = false;               // todo: instead of starting lines at centre and go out, start at fixed radiusand draw line inwards towards the centre
    int8_t spin = 1;                      // 1/-1=rotate clockwise/counter, 0=no rotate
    int spinVal = 0;                      // value to rotate by
    uint8_t backdrop = 1;                 // draw a scaled backdrop
    bool caleidoscope = false;            // drawn on quarter screen and apply caleidoscope effect
    uint8_t caleidoscopeEffect = 0;       // which caleidoscope effect to apply
    uint8_t sprites = 0;                  // will we used half width sprites, if so how many....
    uint8_t spriteBehaviour = 0;          // 0=stationary, 1=revolving around screen
    
    uint8_t audioRange = 0;               // 0=full range, 1=bass centric, 2=mids, 3=treble

  public:


    PatternAudio8x8Squares() {
      name = (char *)"Audio 01 - Big Spark";
    }

    // #------------- START -------------#
    void start(uint8_t _order) {

      // randomly determine the colour effect to use, etc.
      cycleColors = random(0, 2);                                 // TODO: 
      insideOut = random(0, 2);                                   // TODO: 
      spin = random(0, 2); if (spin==0) spin = -1;                // 1=clockwise, -1 = anticlockwise, 0=no spin (disabled))
      if (spinVal==0) spinVal = random(0, 360);                   // choose random angle at initial start      ### USED TO BREAK STUFF? ###
      caleidoscope = random(0, 2);
      caleidoscopeEffect = random(0, 2);
      backdrop = random(1, 5);
      audioRange = random8(0,4);
      
      // override for testing
      cycleColors = false;        // does nothing just yet
      caleidoscope = false;
      caleidoscopeEffect = 0;
      //backdrop = 1;

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

      color1++;
      color2++;






      // determine if we want to first draw to a blank half width canvas first, or directly to the existing screen
      if (backdrop || sprites > 0) {
        effects.ClearCanvas(1);        // clear half width canvas

        uint8_t canvasScale = 4;
        uint8_t canvasWidth = MATRIX_WIDTH / canvasScale;
        uint8_t canvasHeight = MATRIX_HEIGHT / canvasScale;
        uint8_t centreX = (MATRIX_WIDTH  / 2) / canvasScale;
        uint8_t centreY = (MATRIX_HEIGHT  / 2) / canvasScale;
        uint8_t audioScale = canvasScale * 2;

        // draw simple quarter width square to scale up for a background
        for (int i = 0; i < canvasWidth; i++) {
          // randomise the audio range to sample
          switch(audioRange) {
            case 0:
              // full range
              audioData = serialData.specData16[i] / audioScale;
              break;
            case 1:
              // bass centric
              audioData = serialData.specData32[i] / audioScale;
              break;
            case 2:
              // mids centric
              audioData = serialData.specData32[i+8] / audioScale;
              break;
            case 3:
              // treble centric
              audioData = serialData.specData32[i+16] / audioScale;
              break;
          }
          if (audioData >= canvasHeight) audioData = canvasHeight -1;
          

          if (audioData > 0) {
            effects.BresenhamLineCanvasH(i, canvasHeight - audioData, i, 0, effects.ColorFromCurrentPalette((i*16) + color1, 32));
            effects.BresenhamLineCanvasH(i, canvasHeight - 1, i, canvasHeight - audioData, effects.ColorFromCurrentPalette((i*16) + color2, 255));
          }

        }

        if (backdrop==1) {
          for (int i = 0; i < canvasScale; i++) {
            effects.ApplyCanvasH(i*canvasWidth, 0, 1);
            effects.ApplyCanvasH(i*canvasWidth, 16, 1);
            effects.ApplyCanvasH(i*canvasWidth, 32, 1);
            effects.ApplyCanvasH(i*canvasWidth, 48, 1);
          }
          //effects.ApplyCanvasH(0, 0, 4.0);
          effects.StreamRight(128);
        }

        if (backdrop==2) {
          effects.ApplyCanvasH(16, 16, 2.0, 128);     // 64 = light blur
          effects.StreamRight(128);
        }


        if (backdrop==3) {
          effects.ApplyCanvasH(0, 0, 4.0, 64);     // 64 = light blur
          effects.StreamRight(128);
        }

        if (backdrop==4) {
          effects.ApplyCanvasH(16, 16, 2.0, 128);     // 64 = light blur
          effects.ApplyCanvasH(0, 0, 4.0, 64);     // 64 = light blur
          effects.StreamRight(128);
        }



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

    effects.DimAll(150);

    return 0;
  }
};

#endif
