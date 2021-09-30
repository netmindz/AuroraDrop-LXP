#ifndef PatternStaticAtom_H
#define PatternStaticAtom_H

class PatternStaticAtom : public Drawable {
  private:

    // used for internal colour cycling
    uint8_t color1 = 0;
    uint8_t color2 = 64;
    uint8_t color3 = 128;
    uint8_t color4 = 192;
    // used for internal scaling functions
    uint8_t canvasWidth;
    uint8_t canvasCentreX;
    uint8_t canvasCentreY;
    // used for internal audio calcs
    byte audioData;
    uint8_t maxData = 127;

    // pattern specific parameters used for randomizing, theses are mostly radomized at initial start and
    // determine the style of backdrop and main effects to render
    // general/common parameters
    bool generalRand1 = false;            // general use, either true/false
    bool generalRand2 = false;            // general use
    uint8_t colorEffect = 0;              // TODO: use standard palette or special (fire, ice, etc.)
    bool cycleColors = true;              // cylce through the color spectrum or not
    uint8_t audioRange = 0;               // 0=render full audio range, 1=bass centric, 2=mids, 3=treble
    // backdrop only random stuff
    uint8_t backdrop = 0;                 // 0=don't draw backdrop, 1-4=draw various scaled backdrops (override if there are alreadt too many backdrops?)
    int8_t mirrorVert = 0;                // todo: 0=don't mirror, 1=straight mirror, 2=rotate 180 degrees
    // main effects random stuff
    uint8_t mainEffect = 0;               // 0=todo main effects
    bool caleidoscope = false;            // main effect drawn on quarter screen and apply caleidoscope effect
    uint8_t caleidoscopeEffect = 0;       // which caleidoscope effect to apply
    uint8_t sprites = 0;                  // will we used half width sprites, if so how many....
    uint8_t spriteBehaviour = 0;          // 0=stationary, 1=revolving around screen
    

  public:

    PatternStaticAtom() {
      name = (char *)"Audio C - Tunnel (WIP)";
      id = (char *)"X";
    }

    // #------------- START -------------#
    void start(uint8_t _order) {

      // randomize the effects to use
      generalRand1 = random(0, 2);                      // for stream effect directions
      cycleColors = random(0, 3);                       // 75% of the time, the color palette will be cycled
      mirrorVert = random(0, 3);                        // 33% not mirror, 66% one of the two mirror options
      mainEffect = random(0, 2);
      caleidoscope = random(0, 2);
      caleidoscopeEffect = random(0, 2);
      backdrop = random(0, 5);                          // 1 in 5 chance of no background being rendered, otherwise averge chance to any of the 4
      audioRange = random8(0,4);                        // same chance for getting any of the 5 audio ranges
      
      // override some randomizations if previous patterns are over active, or for any other reason
      //if (PatternsAudioBackdropCount > 2 && random8(0,5)) backdrop = 0;              // if too many previous backdrops, then only a 1 in 5 chance of rendering another backdrop
      //if (PatternsAudioMainEffectCount > 2 && random8(0,5)) mainEffect = 0;          // if too many previous main effects, then only a 1 in 5 chance of rendering another effect

      
      // ## ----- override random stuff for testing ----- ##
      caleidoscope = false;
      caleidoscopeEffect = 1;
      //backdrop = 3;
      if (backdrop==0) backdrop = 4;
      // ## ----- override random stuff for testing ----- ##


      // generate random canvas, sprites etc. to use in animations

      // setup parameters depending on random stuff
      if (!caleidoscope) {
        canvasWidth = MATRIX_WIDTH;
        canvasCentreX = MATRIX_CENTER_X;
        canvasCentreY = MATRIX_CENTER_Y;
      }
      else {
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


    };


      uint8_t theta1 = 0;
      uint8_t theta2 = 0;
      uint8_t theta3 = 0;
      uint8_t minx = 16;
      uint8_t miny = 16;
      uint8_t maxx = 48;
      uint8_t maxy = 48;
      uint8_t spirooffset = 16;   // 32= hexagon,  16=
      uint8_t radiusx = 8;
      uint8_t radiusy = 12;
      uint8_t hueoffset = 0;

      uint8_t lastx = 0;
      uint8_t lasty = 0;

      bool useCanvas = true;


    // #------------- DRAW FRAME -------------#
    unsigned int drawFrame(uint8_t _order, uint8_t _total) {
      // order - indicates the order in which the effect is being drawn, patterns can use the appropriate pre and post effects, if any, depending on order etc.
      // total - the total number of audio effect animations being played simultaineously




      // brightness = LO
      byte audio = fftData.specData8[6];
      if (audio > 63) audio = 63;
      audio = audio * 4;

      // draw to half width canvas
      if (useCanvas) {
        effects.ClearCanvas(1);        // clear half width canvas 1
      }

      effects.ClearCanvas(1);

      CRGB color;
      uint8_t x,y,x2,y2;
      // always draws hexagon?
      for (int i = 0; i < 17; i++) {
        x = mapsin8(theta1 + i * spirooffset, minx, maxx);
        y = mapcos8(theta1 + i * spirooffset, miny, maxy);
        x2 = mapsin8(theta2 + i * spirooffset, x - radiusx, x + radiusx);
        y2 = mapcos8(theta2 + i * spirooffset, y - radiusy, y + radiusy);
        color = effects.ColorFromCurrentPalette(hueoffset + i * spirooffset, audio);
        
        
        //effects.leds[XY(x2, y2)] += color;
        //effects.canvasH[XY(x2/2, y2/2)] += color;



/*
        x = mapsin8(128 + theta1 + i * spirooffset, minx, maxx);
        y = mapcos8(128 + theta1 + i * spirooffset, miny, maxy);
        x2 = mapsin8(128 + theta2 + i * spirooffset, x - (radiusx/2), x + radiusx);
        y2 = mapcos8(128 + theta2 + i * spirooffset, y - (radiusy/2), y + radiusy);
        color = effects.ColorFromCurrentPalette(hueoffset + i * spirooffset, audio);
        effects.leds[XY(x2, y2)] += color;
*/


        if (i>0) {
        //effects.BresenhamLine(lastx, lasty, x2, y2, effects.ColorFromCurrentPalette(hueoffset + i * spirooffset, audio / 4));
        effects.BresenhamLineCanvas(effects.canvasH, lastx/2, lasty/2, x2/2, y2/2, effects.ColorFromCurrentPalette(hueoffset + i * spirooffset, fftData.specData[i*6]));
        }


        lastx = x2;
        lasty = y2;
      }


      theta1 = theta1 + 2;
      theta2++;
      theta3++;

      hueoffset++;

      //effects.DimAll(230);

      return 0;


      // todo
      float radius = canvasWidth / 2;
      float ratio = 360.0 / BINS;        // rotate around 360 degrees
      float rotation = 0.0;
      float angle = 0;
      //int x,y;

      // general cyclic stuff
      if (cycleColors) {
        // currently cycling through colours as fast as possible
        color1++;
        color2++;
      }

        // ---------------------- generate canvas/sprites ----------------------------

      // single canvas for use in the backdrop or main animations
      if (backdrop || sprites > 0) {
        effects.ClearCanvas(1);        // clear half width canvas 1
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
              audioData = fftData.specData16[i] / audioScale;
              break;
            case 1:
              // bass centric
              audioData = fftData.specData32[i] / audioScale;
              break;
            case 2:
              // mids centric
              audioData = fftData.specData32[i+8] / audioScale;
              break;
            case 3:
              // treble centric
              audioData = fftData.specData32[i+16] / audioScale;
              break;
          }
          if (audioData >= canvasHeight) audioData = canvasHeight -1;
          

          if (audioData > 0) {
            //effects.BresenhamLineCanvasH(i, canvasHeight - audioData, i, 0, effects.ColorFromCurrentPalette((i*16) + color1, 32));
            //effects.BresenhamLineCanvasH(i, canvasHeight - 1, i, canvasHeight - audioData, effects.ColorFromCurrentPalette((i*16) + color2, 255));
            effects.BresenhamLineCanvas(effects.canvasH, i, canvasHeight - audioData, i, 0, effects.ColorFromCurrentPalette((i*16) + color1, 32));
            effects.BresenhamLineCanvas(effects.canvasH, i, canvasHeight - 1, i, canvasHeight - audioData, effects.ColorFromCurrentPalette((i*16) + color2, 255));
          }

        }


        // ---------------------- generate chosen backdrop ----------------------------

        if (backdrop==1) {
          for (int i = 0; i < canvasScale; i++) {
            effects.ApplyCanvas(effects.canvasH, i*canvasWidth, 0, 1);
            effects.ApplyCanvas(effects.canvasH, i*canvasWidth, 16, 1);
            effects.ApplyCanvas(effects.canvasH, i*canvasWidth, 32, 1);
            effects.ApplyCanvas(effects.canvasH, i*canvasWidth, 48, 1);
          }
          //effects.ApplyCanvas(0, 0, 4.0);
          effects.DimAll(180);
        }

        if (backdrop==2) {
          effects.ApplyCanvas(effects.canvasH, 16, 16, 2.0, 128);     // 64 = light blur
        }

        // overscaled half width canvas centered on frame/screen
        if (backdrop==3) {
          effects.ApplyCanvas(effects.canvasH, 0, 0, 4.0, 64);     // 64 = light blur
        }

        if (backdrop==4) {   
          effects.ApplyCanvas(effects.canvasH, 16, 16, 2.0, 128);     // 64 = light blur
          effects.ApplyCanvas(effects.canvasH, 0, 0, 4.0, 64);     // 64 = light blur
        }

        // apply test effects
        if (backdrop == 1) {   
          if (generalRand1) {
            if (generalRand2) {
              effects.StreamRight(128);
            }
            else {
              effects.StreamUpAndRight(128);
            }
          } 
          else {
            effects.StreamUp(128);
          }
        }


      }


      // ---------------------- generate main foreground animation ------------------------

      // if we are in caleidoscope mode, focus the rendering on the top left corner

      // todo: caleidoscope options
      if (caleidoscope) {
        effects.Caleidoscope1Centre();          // default for now
        switch (caleidoscopeEffect) {
          case 0:
            break;
          case 1:
            break;
        }
      }

      // ----------------- if so, apply randomish caleidoscope effect ---------------------

      // TODO:


    effects.DimAll(250);   // 150 is good for clean bars

    return 0;
  }
};

#endif
