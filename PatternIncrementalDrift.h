

#ifndef PatternIncrementalDrift_H
#define PatternIncrementalDrift_H

class PatternIncrementalDrift : public Drawable {
  public:
    PatternIncrementalDrift() {
      name = (char *)"Incremental Drift";
      id2 = 1;
    }

    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
      uint8_t dim = beatsin8(2, 230, 250);
      
      //effects.DimAll(dim);   // TONY TEST COMMENT
      effects.DimAll(250);
      
      effects.ShowFrame();

      for (int i = 2; i <= MATRIX_WIDTH / 2; i++)
      {
        CRGB color = effects.ColorFromCurrentPalette((i - 2) * (240 / (MATRIX_WIDTH / 2)));

        uint8_t x = beatcos8((17 - i) * 2, MATRIX_CENTER_X - i, MATRIX_CENTER_X + i);
        uint8_t y = beatsin8((17 - i) * 2, MATRIX_CENTER_Y - i, MATRIX_CENTER_Y + i);

        effects.drawBackgroundFastLEDPixelCRGB(x, y, color);
      }

      return 0;
    }
};

#endif
