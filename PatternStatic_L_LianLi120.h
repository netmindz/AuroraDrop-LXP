#ifndef PatternStaticLianLi120_H
#define PatternStaticLianLi120_H

class PatternStaticLianLi120 : public Drawable {
  private:

/*
// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF
*/

  public:
    PatternStaticLianLi120() {
      name = (char *)"Lian-Li SL120";
      id = "L";
      enabled = true;
    }


    // ------------------ start -------------------
    void start(uint8_t _pattern) {
      

    }


    // --------------------- draw frame -------------------------
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

        uint8_t signHeight;
        uint8_t signCenter;

        //CRGB color  = 0xF800 ;
        CRGB color  = 0xEF0000;
        CRGB colorCenter  = 0x0A0A0A;

        signHeight = MATRIX_HEIGHT;
        if (fftData.noAudio) signHeight = signHeight - 7;
        signCenter = signHeight / 2;

        //left
        effects.BresLine(14, 0, 3, 11, color, NOBLEND);
        effects.BresLine(15, 0, 4, 11, color, NOBLEND);

        effects.BresLine(3, 11, 3, signHeight - 11, color, NOBLEND);
        effects.BresLine(4, 11, 4, signHeight - 11, color, NOBLEND);
        effects.BresLine(5, 10, 5, signHeight - 10, color, NOBLEND);

        effects.BresLine(3, signHeight - 11, 14, signHeight, color, NOBLEND);
        effects.BresLine(4, signHeight - 11, 15, signHeight, color, NOBLEND);

        //right
        effects.BresLine(49, 0, 60, 11, color, NOBLEND);
        effects.BresLine(48, 0, 59, 11, color, NOBLEND);

        effects.BresLine(60, 11, 60, signHeight - 11, color, NOBLEND);
        effects.BresLine(59, 11, 59, signHeight - 11, color, NOBLEND);
        effects.BresLine(58, 10, 58, signHeight - 10, color, NOBLEND);

        effects.BresLine(60, signHeight - 11, 49, signHeight, color, NOBLEND);
        effects.BresLine(59, signHeight - 11, 48, signHeight, color, NOBLEND);



        // centre circle
        effects.BresLine(29, signCenter - 6, 34, signCenter - 6, colorCenter, LINEARBLEND);
        effects.BresLine(27, signCenter - 5, 36, signCenter - 5, colorCenter, LINEARBLEND);
        effects.BresLine(26, signCenter - 4, 37, signCenter - 4, colorCenter, LINEARBLEND);
        effects.BresLine(26, signCenter - 3, 37, signCenter - 3, colorCenter, LINEARBLEND);
        effects.BresLine(25, signCenter - 2, 38, signCenter - 2, colorCenter, LINEARBLEND);
        effects.BresLine(25, signCenter - 1, 38, signCenter - 1, colorCenter, LINEARBLEND);
        effects.BresLine(25, signCenter , 38, signCenter, colorCenter, LINEARBLEND);
        effects.BresLine(25, signCenter + 1, 38, signCenter + 1, colorCenter, LINEARBLEND);
        effects.BresLine(25, signCenter + 2, 38, signCenter + 2, colorCenter, LINEARBLEND);
        effects.BresLine(25, signCenter + 3, 38, signCenter + 3, colorCenter, LINEARBLEND);
        effects.BresLine(26, signCenter + 4, 37, signCenter + 4, colorCenter, LINEARBLEND);
        effects.BresLine(26, signCenter + 5, 37, signCenter + 5, colorCenter, LINEARBLEND);
        effects.BresLine(27, signCenter + 6, 36, signCenter + 6, colorCenter, LINEARBLEND);
        effects.BresLine(29, signCenter + 7, 34, signCenter + 7, colorCenter, LINEARBLEND);
        

      return 0;
    }


};

#endif
