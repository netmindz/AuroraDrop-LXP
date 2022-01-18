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

#ifndef PatternAudioAuroraDrop_H
#define PatternAudioAuroraDrop_H

class PatternAudioAuroraDrop : public Drawable {
private:

    // x,y, & time values
    uint32_t x,y,v_time,hue_time,hxy;

    // Play with the values of the variables below and see what kinds of effects they
    // have!  More octaves will make things slower.

    // how many octaves to use for the brightness and hue functions
    uint8_t octaves=1;
    uint8_t hue_octaves=3;

    // the 'distance' between points on the x and y axis
    int xscale=57771;
    int yscale=57771;

    // the 'distance' between x/y points for the hue noise
    int hue_scale=1;

    // how fast we move through time & hue noise
    int time_speed=1111;
    int hue_speed=31;

    // adjust these values to move along the x or y axis between frames
    int x_speed=331;
    int y_speed=1111;

    bool started = false;

public:
    PatternAudioAuroraDrop() {
        name = (char *)"AuroraDrop";
        id = "Z";
        enabled = false;
    }


    // ------------------ start -------------------
    void start(uint8_t _pattern) {

  if (!started) {
    started = true;

  // initialize the x/y and time values
  int x = random8(0,1);
  random16_set_seed(8934);
  random16_add_entropy(analogRead(3));

  hxy = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  x = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  y = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  v_time = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  hue_time = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();

  effects.NoiseVariablesSetup();


  }



    }


    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
      // fill the led array 2/16-bit noise values
      // turns out, there's lots of inefficient ways to do this

  

  //effects.ClearCanvas(3);
  fill_2dnoise16(effects.canvasQ, MATRIX_CENTER_X/2, MATRIX_CENTER_Y/2, false, octaves, x, xscale, y, yscale, v_time, hue_octaves, hxy, hue_scale, hxy, hue_scale, hue_time, false);
  // dim each led related to the voumem of the audio
  uint8_t bin = 0;
  for (uint8_t x=0; x<MATRIX_CENTER_X/2; x++) {
    for (uint8_t y=0; y<MATRIX_CENTER_Y/2; y++) {
      byte audio = fftData.specData[bin];
      if (audio > 127) audio = 127;
      audio = audio * 2;
      if (audio > 127) audio = 127;
      effects.DimPixel(effects.canvasQ, (x*(MATRIX_CENTER_X/2))+y, audio * 2);
      bin++;
      if (bin >127) bin = 0;
    }
  }


  effects.ApplyCanvasQ(effects.canvasQ, 0, 0);
  effects.ApplyCanvasQ(effects.canvasQ, MATRIX_CENTER_X/2, 0);        // need to mirror/flip these
  effects.ApplyCanvasQ(effects.canvasQ, 0, MATRIX_CENTER_Y/2);
  effects.ApplyCanvasQ(effects.canvasQ, MATRIX_CENTER_X/2, MATRIX_CENTER_Y/2);

  blur2d(effects.leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH/2, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT/2, 128);
  effects.Caleidoscope2();


  //effects.ApplyCanvasH(effects.canvasH, MATRIX_CENTER_X, 0);
  //effects.ApplyCanvasH(effects.canvasH, 0, MATRIX_CENTER_Y);
  //effects.ApplyCanvasH(effects.canvasH, MATRIX_CENTER_X, MATRIX_CENTER_Y);

  //effects.Caleidoscope2();
  //blur2d(effects.leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, 128);






  // adjust the intra-frame time values
  x += x_speed;
  y += y_speed;
  v_time += time_speed;
  hue_time += hue_speed;


    return 0;
    }


};

#endif
