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

#ifndef PatternAudioRotatingSpectrum_H
#define PatternAudioRotatingSpectrum_H

class PatternAudioRotatingSpectrum : public Drawable {
  private:

    uint8_t dimVal;
    uint8_t dimStart;


    // randomise theses
    uint8_t dimEnd;
    bool hueCycle1 = false;
    byte hue1 = 0;
    uint32_t hue_ms1;
    uint8_t offset1 = 0;
    uint8_t counter1 = 0;
    uint8_t mirror1 = 0;
    uint8_t stagger1 = 1;

    bool useCurrentPalette;
    uint8_t colorSpread;
    uint8_t vertical;
    bool backdrop;
    bool caleidoscope;
    uint8_t caleidoscopeEffect;
    uint8_t audioScale;


    bool hueCycle2 = false;
    byte hue2 = 0;
    uint32_t hue_ms2;
    uint8_t offset2 = 0;
    uint8_t counter2 = 0;
    uint8_t mirror2 = 0;
    uint8_t stagger2 = 1;

public:
  PatternAudioRotatingSpectrum() {
    name = (char *)"Rotating Spectrum";
    id = "D";
    enabled = true;
  }


  uint8_t zzzz = 0;

  // ------------------ start -------------------
  void start(uint8_t _pattern) {

    dimStart = 255;
    dimVal = dimStart;

    // randomise stuff
    dimEnd = random8(128, 255);    
    
    hue1 = random8(0,255);
    hue_ms1 = millis();
    hueCycle1 = true; // random8(0,2);
    offset1 = 0;
    counter1 = 0;
    mirror1 = random8(0,4);
    stagger1 = random8(0,5); //if (stagger==3) stagger = 4;

    hue2 = random8(0,255);
    hue_ms2 = millis();
    hueCycle2 = true; // random8(0,2);
    offset2 = 0;
    counter2 = 0;
    mirror2 = random8(0,4);
    stagger2 = random8(0,5); //if (stagger==3) stagger = 4;



    useCurrentPalette = random(0,2);
    colorSpread = random(1,5);
    vertical = random8(0,3);
    backdrop = random8(0,2);
    caleidoscope = random8(0, 5);                             // 80% chance of caleidoscope
    caleidoscopeEffect = random8(1, CALEIDOSCOPE_COUNT + 1);
    audioScale = random8(3, 6);
    //useCurrentPalette = true;
    //colorSpread = 4;
    //caleidoscope = true;

    useCurrentPalette = true;   // fix this to use fire or ocean palette and not fixed colour

        
    effects.NoiseVariablesSetup();

    p1.x = 9;
    p1.y = 31;
    p2.x = 53;
    p2.y = 31;
            
  }

  int x0,y0,x1,y1,x2,y2,x3,y3;
  int diagonalOffset = 0;

  Point p1,p2;
  Point rp1,rp2;
  float cx=0;
  float cy=0;
  float angle = 0;

  
  // --------------------- DRAW FRAME -------------------------
  unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {


      // if we are going to dim all, then do it gradually
      effects.DimAll(dimVal);
      if (dimVal > dimEnd) dimVal--;
      //effects.DimAll(180);


      // draw to half width canvas
      effects.ClearCanvas(1);


    // draw on full screen
      uint8_t data;

/*      
      for (byte i = 0; i < 64; i=i+4) 
      {
        data = fftData.specData16[i/4] / 3;    // use the 16 bins for this!
        if (data > 63) data = 63;
        x1 = i;
        x2 = i+4;
        y1 = data;
        y2 = y1 + diagonalOffset;
        // only draw bars if there is non zero data
        if (data)
          effects.BresenhamLine(x1, y1, x2, y2, dma_display->color565(128, 128, 128));
      }
*/

      // centered spectrum analyser
    uint8_t space;

/*
    for (byte i = 0; i < MATRIX_WIDTH / 2; i++) 
    {
      data = fftData.specData32[i] / 3;
      x1 = i + (MATRIX_WIDTH/4);
      x2 = i + (MATRIX_WIDTH/4);
      y1 = MATRIX_HEIGHT - 1;
      y2 = MATRIX_HEIGHT - data;

      space = 64 - (y1 - y2);
      y1 = y1 - (space/2);
      y2 = y2 - (space/2);

      Point p1(x1,y1);
      Point p2(x2,y2);
      //float angle2 = 1;

      Point rp1 = rotate_point(31,31,angle,p1);
      Point rp2 = rotate_point(31,31,angle,p2);

      if (data)
      {
        if (useCurrentPalette) 
        {
          effects.BresenhamLine(rp1.x, rp1.y, rp2.x, rp2.y, i * colorSpread, 255);
          effects.BresenhamLineCanvas(effects.canvasH, rp1.x-16, rp1.y-16, rp2.x-16, rp2.y-16, i * colorSpread, 128);
        }
        else
        {
          effects.BresenhamLine(rp1.x, rp1.y, rp2.x, rp2.y, 16, 255);
          effects.BresenhamLineCanvas(effects.canvasH, rp1.x-16, rp1.y-16, rp2.x-16, rp2.y-16, 16, 128);
        }
      }

    }
*/


/*
    for (byte i = 0; i < MATRIX_WIDTH; i++) 
    {
      data = fftData.specData[i+32] / 3;
      x1 = i;
      x2 = i;
      y1 = MATRIX_HEIGHT - 1;
      y2 = MATRIX_HEIGHT - data;

      space = 64 - (y1 - y2);
      y1 = y1 - (space/2);
      y2 = y2 - (space/2);

      Point p1(x1,y1);
      Point p2(x2,y2);
      //float angle2 = 1;

      Point rp1 = rotate_point(31,31,angle,p1);
      Point rp2 = rotate_point(31,31,angle,p2);

      if (data)
      {
        if (useCurrentPalette) 
        {
          effects.BresenhamLine(rp1.x, rp1.y, rp2.x, rp2.y, i * colorSpread, 255);
          effects.BresenhamLineCanvas(effects.canvasH, rp1.x - 16, rp1.y - 16, rp2.x - 16, rp2.y - 16, i * colorSpread, 128);
        }
        else
        {
          effects.BresenhamLine(rp1.x, rp1.y, rp2.x, rp2.y, 16, 255);
          effects.BresenhamLineCanvas(effects.canvasH, rp1.x - 16, rp1.y - 16, rp2.x - 16, rp2.y - 16, 16, 128);
        }
      }

    }
*/

if (!fftData.noAudio) {

    for (byte i = 0; i < 96; i++) 
    {
      data = fftData.specData[i] / audioScale;   // random 3,4,5 or 6
      x1 = i - 16;
      x2 = i - 16;
      y1 = MATRIX_HEIGHT - 1;
      y2 = MATRIX_HEIGHT - data;

      space = 64 - (y1 - y2);
      y1 = y1 - (space/2);
      y2 = y2 - (space/2);

      Point p1(x1,y1);
      Point p2(x2,y2);
      //float angle2 = 1;

      Point rp1 = effects.RotatePoint(31,31,angle,p1);
      Point rp2 = effects.RotatePoint(31,31,angle,p2);

      if (data)
      {
        if (useCurrentPalette) 
        {
          effects.BresenhamLine(rp1.x, rp1.y, rp2.x, rp2.y, i * colorSpread, 255);
          effects.BresLineCanvasH(effects.canvasH, rp1.x - 16, rp1.y - 16, rp2.x - 16, rp2.y - 16, (i * colorSpread) + 128, 128);
          if (vertical) 
          {
            effects.BresenhamLine(rp1.y, rp1.x, rp2.y, rp2.x, i * colorSpread, 255);
            effects.BresLineCanvasH(effects.canvasH, rp1.y - 16, rp1.x - 16, rp2.y - 16, rp2.x - 16, (i * colorSpread) + 128, 128);
          }
        }
        else
        {
          effects.BresenhamLine(rp1.x, rp1.y, rp2.x, rp2.y, 16, 255);
          effects.BresLineCanvasH(effects.canvasH, rp1.x - 16, rp1.y - 16, rp2.x - 16, rp2.y - 16, 16, 128);
          if (vertical) 
          {
            effects.BresenhamLine(rp1.y, rp1.x, rp2.y, rp2.x, 16, 255);
            effects.BresLineCanvasH(effects.canvasH, rp1.y - 16, rp1.x - 16, rp2.y - 16, rp2.x - 16, 16 + 128, 128);
          }
        }
      }

    }


    //effects.ApplyCanvas(effects.canvasH, 16, 0, 1);
    //effects.ApplyCanvasMirror(effects.canvasH, 16, 16, 1);

    //effects.ApplyCanvas(effects.canvasH, 31, 0, 0.5);
    //effects.ApplyCanvas(effects.canvasH, 0, 0, 2.0);
    if (backdrop) 
    {
      effects.ApplyCanvasHMirror(effects.canvasH, 0, 0, 2.0);
    }


  }


    angle = angle + 0.01;
    if (angle >= 3.14*2) angle = 0;




     
    
    if (hueCycle1 && hue_ms1 + 200 < millis()) 
    {
      hue_ms1 = millis();
      hue1++;
    }

    if (hueCycle2 && hue_ms2 + 200 < millis()) 
    {
      hue_ms2 = millis();
      hue2++;
    }

    // do caleidoscope sometimes
    if (caleidoscope) {
      effects.RandomCaleidoscope(caleidoscopeEffect);
    }



    return 0;
  }
};

#endif
