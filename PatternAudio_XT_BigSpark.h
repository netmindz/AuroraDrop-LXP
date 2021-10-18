/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef PatternAudioBigSpark_H
#define PatternAudioBigSpark_H

class PatternAudioBigSpark : public Drawable {
  private:

    // not using these at the moment
    uint8_t color1 = 0;
    uint8_t color2 = 64;
    uint8_t color3 = 128;
    uint8_t color4 = 192;

    uint8_t canvasWidth;
    uint8_t canvasCentreX;
    uint8_t canvasCentreY;

    // theses are mostly radomised at initial start
    uint8_t paletteType = 0;              // TODO: use standard random palette or special (fire, ice, etc.)
    uint8_t colorEffect = 0;              // TODO: 
    bool cycleColors;                     // todo: cylce through the color spectrum
    bool insideOut = false;               // todo: instead of starting lines at centre and go out, start at fixed radiusand draw line inwards towards the centre
    int8_t spin = 1;                      // 1/-1=rotate clockwise/counter, 0=no rotate
    int spinVal = 0;                      // value to rotate by
    bool backdrop = true;                 // draw a scaled backdrop
    bool caleidoscope = false;            // drawn on quarter screen and apply caleidoscope effect
    uint8_t caleidoscopeEffect = 0;       // which caleidoscope effect to apply
    uint8_t sprites = 0;                  // will we used half width sprites, if so how many....
    uint8_t spriteBehaviour = 0;          // 0=stationary, 1=revolving around screen
    bool dimPreRender = false;            // dim prior to frame render 
    uint8_t dimAmount = 0;

  public:


    PatternAudioBigSpark() {
      name = (char *)"Big Spark";
      id = (char *)"T";;
      enabled = false;
    }

    // #------------- START -------------#
    void start(uint8_t _order) {

      id2 = 9;

      // randomly determine the colour effect to use, etc.
      cycleColors = random(0, 2);                                 // TODO: 
      insideOut = random(0, 2);                                   // TODO: 
      spin = random(0, 2); if (spin==0) spin = -1;                // 1=clockwise, -1 = anticlockwise, 0=no spin (disabled))
      if (spinVal==0) spinVal = random(0, 360);                   // choose random angle at initial start      ### USED TO BREAK STUFF? ###
      caleidoscope = random(0, 2);
      caleidoscopeEffect = random(0, 2);
      dimPreRender = random8(0, 2);
      dimAmount = random8(150, 250);
      
      // override for testing
      cycleColors = false;        // does nothing just yet
      //caleidoscope = true;
      //caleidoscopeEffect = 0;
      backdrop = true;
      dimAmount = 150;

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
      
      // dimming is useful on this pattern and can clean things up
      if (dimPreRender) {
        effects.DimAll(dimAmount);
      }


      float radius = canvasWidth / 2;
      float ratio = 360.0 / BINS;        // rotate around 360 degrees
      float rotation = 0.0;
      float angle = 0;
      int x,y;

      // add spin to rotation value (0-360)
      spinVal += spin;
      if (spinVal > 360) spinVal = 0;
      if (spinVal < 0) spinVal = 360;


      // --------- draw any canvas requirements --------------- #


      // determine if we want to first draw to a blank half width canvas first, or directly to the existing screen
      if (backdrop || sprites > 0) {
        effects.ClearCanvas(1);        // clear half width canvas

        uint8_t canvasScale = 2;
        uint8_t audioScale = canvasScale;
        uint8_t centreX = (MATRIX_WIDTH  / 2) / canvasScale;
        uint8_t centreY = (MATRIX_HEIGHT  / 2) / canvasScale;
        for (int i=0; i<BINS; i++) {
          audioData = fftData.specData[i] / 6;
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
            effects.BresLineCanvasH(effects.canvasH, centreX, centreY, x, y, effects.ColorFromCurrentPalette(i*2.65,255));
        }

        // two sprite moving up and down
        effects.ApplyCanvasH(effects.canvasH, 0, MATRIX_HEIGHT - (effects.beatSineOsciWidth[3] + 16), 1);
        effects.ApplyCanvasH(effects.canvasH, 32, effects.beatSineOsciWidth[3] - 16, 1);

        // two orbiting sprites
        effects.ApplyCanvasH(effects.canvasH, effects.beatSineOsciWidth[3] - 8, effects.beatCosineOsciWidth[3] - 8, 0.5);
        effects.ApplyCanvasH(effects.canvasH, MATRIX_WIDTH - (effects.beatSineOsciWidth[3] + 8), MATRIX_HEIGHT - (effects.beatCosineOsciWidth[3] + 8), 0.5);

        // another two orbiting other way
        effects.ApplyCanvasH(effects.canvasH, effects.beatSineOsciWidth[3] - 8, MATRIX_HEIGHT - (effects.beatCosineOsciWidth[3] + 8), 0.5);
        effects.ApplyCanvasH(effects.canvasH, MATRIX_WIDTH - (effects.beatSineOsciWidth[3] + 8), effects.beatCosineOsciWidth[3] - 8, 0.5);



        if (backdrop) {
          effects.ApplyCanvasH(effects.canvasH, -32, -32, 4.0, 128);   // 128 = medium blur
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



    // really good pair of effects
    //effects.SpiralStream(31, 31, 64, 128);        // for 64 pixel wide matrix!
    //effects.DimAll(220);


    // don't dim at end, removes clarity
    //effects.DimAll(220);


    return 0;
  }
};

#endif
