#ifndef PatternRadar_H

class PatternRadar : public Drawable {
  private:
    byte theta = 0;
    byte hueoffset = 0;

  public:
    PatternRadar() {
      name = (char *)"Radar";
      id2 = 1;
    }

    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
      effects.DimAll(254);

      //uint8_t data = MATRIX_CENTER_X;
      //data = serialData.spec8Data[3] / 3;
      //if (data > MATRIX_CENTER_X) data = MATRIX_CENTER_X;

      for (int offset = 0; offset < MATRIX_CENTER_X; offset++) {
        byte hue = 255 - (offset * 16 + hueoffset);
        CRGB color = effects.ColorFromCurrentPalette(hue);
        uint8_t x = mapcos8(theta, offset, (MATRIX_WIDTH - 1) - offset);
        uint8_t y = mapsin8(theta, offset, (MATRIX_HEIGHT - 1) - offset);
        uint16_t xy = XY(x, y);
        effects.leds[xy] = color;

        EVERY_N_MILLIS(25) {
          theta += 2;
          hueoffset += 1;
        }
      }

      return 0;
    }
};

#endif