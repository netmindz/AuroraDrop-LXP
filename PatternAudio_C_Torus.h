#ifndef PatternAudioTorus_H
#define PatternAudioTorus_H

class PatternAudioTorus : public Drawable {
  private:

    // used for internal colour cycling
    uint8_t color1 = 0;
    uint8_t color2 = 64;
    uint8_t color3 = 128;
    uint8_t color4 = 192;
    // used for internal audio calcs
    byte audioData;
    uint8_t maxData = 127;

    // pattern specific parameters used for randomizing, theses are mostly radomized at initial start and
    // determine the style of backdrop and main effects to render
    bool cycleColors = true;              // cylce through the color spectrum or not
    bool caleidoscope = false;
    uint8_t caleidoscopeEffect = 0;
    bool dimAll = false;
    uint8_t dimAmount = 0;

  public:

    PatternAudioTorus() {
      name = (char *)"Audio C - Rotating Torus (WIP)";
      id = (char *)"C";
    }

    // #------------- START -------------#
    void start(uint8_t _order) {

      // randomize the effects to use
      cycleColors = random8(0, 3);                              // 75% of the time, the color palette will be cycled
      caleidoscope = random8(0, 2);                             // 50% chance of caleidoscope
      caleidoscopeEffect = random8(1, CALEIDOSCOPE_COUNT + 1); 
      if (caleidoscopeEffect==5) caleidoscopeEffect = 3;
      dimAll = random8(0, 10);                                  // 90% chance of dimming the output render
      dimAmount = random8(200, 240); 

    };


      uint8_t theta1 = 0;
      uint8_t theta2 = 0;
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



    // #------------- DRAW FRAME -------------#
    unsigned int drawFrame(uint8_t _order, uint8_t _total) {
      // order - indicates the order in which the effect is being drawn, patterns can use the appropriate pre and post effects, if any, depending on order etc.
      // total - the total number of audio effect animations being played simultaineously

      // this patterns looks good with dimming so do it sometimes
      if (dimAll) {
        effects.DimAll(dimAmount);
      }


      // general cyclic stuff
      if (cycleColors) {
        // currently cycling through colours as fast as possible
        color1++;
        color2++;
      }

      // brightness = LO
      byte audio = fftData.specData8[6];
      if (audio > 63) audio = 63;
      audio = audio * 4;


      // scale audio data to floor at specDataMinVolume


      // draw to half width canvas
      effects.ClearCanvas(1);

      CRGB color;
      uint8_t x1,y1,x2,y2;

      for (int i = 0; i < 17; i++) {
        // determine brightness from 


        x1 = mapsin8(theta1 + i * spirooffset, minx, maxx);
        y1 = mapcos8(theta1 + i * spirooffset, miny, maxy);
        x2 = mapsin8(theta2 + i * spirooffset, x1 - radiusx, x1 + radiusx);
        y2 = mapcos8(theta2 + i * spirooffset, y1 - radiusy, y1 + radiusy);
        color = effects.ColorFromCurrentPalette(hueoffset + i * spirooffset, audio);

        // scale audio data to floor at specDataMinVolume
        audio = fftData.specData[i*5];
        if (audio > 127) audio - 127;
        audio = (audio - fftData.specDataMinVolume) * 2;

        if (i > 0) effects.BresenhamLineCanvas(effects.canvasH, lastx/2, lasty/2, x2/2, y2/2, effects.ColorFromCurrentPalette(hueoffset + i * spirooffset, audio));

        effects.ApplyCanvas(effects.canvasH, (x1/2)-8, (y1/2)-8, 1.5);

        lastx = x2;
        lasty = y2;
      }

      theta1 = theta1 + 2;
      theta2++;

      hueoffset++;

      // do caleidoscope sometimes
      if (caleidoscope) {
        effects.RandomCaleidoscope(caleidoscopeEffect);
      }

      return 0;

  }
};

#endif
