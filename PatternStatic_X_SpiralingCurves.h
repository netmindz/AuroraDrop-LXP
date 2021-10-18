/*
 * https://wokwi.com/arduino/projects/290624755962216973
 * 
 */

#ifndef PatternStaticSpiralingCurves_H
#define PatternStaticSpiralingCurves_H

class PatternStaticSpiralingCurves : public Drawable {
private:

    #define speed -12  //   was -6

    byte hue;
    //uint8_t  speed  = fftData.bpm;


  public:
    PatternStaticSpiralingCurves() {
      name = (char *)" Spiraling Curves";
      id = "T";
      enabled = true;
    }


//Idea from Ldir's F_lying
void drawPixelXYF(float x, float y, const CRGB & color) {

  if (x < 0 || y < 0 || x > ((float) MATRIX_WIDTH - 1) || y > ((float) MATRIX_HEIGHT - 1)) return;

  // extract the fractional parts and derive their inverses
  uint16_t xx = (x - (uint16_t) x) * 255, yy = (y - (uint16_t) y) * 255, ix = 255 - xx, iy = 255 - yy;
  // calculate the intensities for each affected pixel
#define WU_WEIGHT(a, b)((uint16_t)(((a) * (b) + (a) + (b)) >> 8))
  uint16_t wu[4] = {
    WU_WEIGHT(ix, iy),
    WU_WEIGHT(xx, iy),
    WU_WEIGHT(ix, yy),
    WU_WEIGHT(xx, yy)
  };
  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (uint16_t i = 0; i < 4; i++) {
    uint16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = effects.leds[XY(xn, yn)];
    if (xn < (uint16_t) MATRIX_WIDTH - 1 && yn < (uint16_t) MATRIX_HEIGHT - 1 && yn > 0 && xn > 0) {
      clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
      clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
      clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    } else {
      clr.r = qadd8(clr.r, (color.r * 85) >> 8);
      clr.g = qadd8(clr.g, (color.g * 85) >> 8);
      clr.b = qadd8(clr.b, (color.b * 85) >> 8);
    }


    effects.leds[XY(xn, yn)] = clr;
    // repeat for anothe 3 lines
    effects.leds[XY(MATRIX_WIDTH - xn, MATRIX_HEIGHT - yn)] = clr;
    // join them up?

/*    
    effects.leds[XY(xn, MATRIX_HEIGHT - yn)] = clr;
    effects.leds[XY(MATRIX_WIDTH - xn, yn)] = clr;


    // skinny ones too?
    effects.leds[XY(xn/2, yn)] = clr;
    effects.leds[XY((xn/2) + MATRIX_CENTER_X, yn)] = clr;
    effects.leds[XY(xn, yn/2)] = clr;
    effects.leds[XY(xn, (yn/2) + MATRIX_CENTER_Y)] = clr;

    //small ones
    effects.leds[XY(xn/2, yn/2)] = clr;
    effects.leds[XY(xn/2, (yn/2) + MATRIX_CENTER_Y)] = clr;
    effects.leds[XY((xn/2) + MATRIX_CENTER_X, (yn/2) + MATRIX_CENTER_Y)] = clr;
    effects.leds[XY((xn/2) + MATRIX_CENTER_X, (yn/2) + MATRIX_CENTER_Y)] = clr;
*/

  }
#undef WU_WEIGHT
}


void drawCurve(float x,float y, float x2,float y2,float x3,float y3,CRGB col) 
{ 
    float xu = 0.0 , yu = 0.0 , u = 0.0 ; 
    uint16_t i = 0 ; 
    for(u = 0.0 ; u <= 1.0 ; u += 0.03)   // was += 0.01   (overly cpu intensive)
    { 
        xu = pow(1-u,3)*x+3*u*pow(1-u,2)*x2+3*pow(u,2)*(1-u)*x3 
             +pow(u,3)*x3; 
        yu = pow(1-u,3)*y+3*u*pow(1-u,2)*y2+3*pow(u,2)*(1-u)*y3
            +pow(u,3)*y3;


        drawPixelXYF(xu,yu,col);   // was

        //effects.leds[XY(xu, yu)] = col;

    } 
}

    // ------------------ start -------------------
    void start(uint8_t _pattern) {
      

    }


    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

        effects.DimAll(240); //fadeToBlackBy(leds, NUM_LEDS, 30);
        uint16_t x1 = beatsin8 (18+speed, 0, (MATRIX_WIDTH-2));
        uint16_t x2 = beatsin8 (23+speed, 0, (MATRIX_WIDTH-2)); 
        uint16_t x3 = beatsin8 (27+speed, 0, (MATRIX_WIDTH-2)); 

        uint16_t y1 = beatsin8 (20+speed, 0, (MATRIX_HEIGHT-2)); 
        uint16_t y2 = beatsin8 (26+speed, 0, (MATRIX_HEIGHT-2));
        uint16_t y3 = beatsin8 (15+speed, 0, (MATRIX_HEIGHT-2));

        drawCurve(x1,y1,x2,y2,x3,y3,CHSV(hue,255,255));
        hue++;


      return 0;
    }


};

#endif
