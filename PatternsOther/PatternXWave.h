#ifndef PatternWave_H
#define PatternWave_H

class PatternWave : public Drawable {
  private:
    byte thetaUpdate = 0;
    byte thetaUpdateFrequency = 0;
    byte theta = 0;

    byte hueUpdate = 0;
    byte hueUpdateFrequency = 0;
    byte hue = 0;

    byte rotation = 0;

    uint8_t scale = 256 / MATRIX_WIDTH;

    uint8_t maxX = MATRIX_WIDTH - 1;
    uint8_t maxY = MATRIX_HEIGHT - 1;

    uint8_t waveCount = 1;

  public:
    PatternWave() {
      name = (char *)"Wave";
      id = "W";
      enabled = false;
    }


    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
     int n = 0;

        switch (rotation) {
            case 0:
                for (int x = 0; x < MATRIX_WIDTH; x++) {
                    n = quadwave8(x * 2 + theta) / scale;
                    effects.Pixel(x, n, x + hue, 255);
                    //backgroundLayer.drawPixel(x, n, effects.ColorFromCurrentPalette(x + hue));
                    if (waveCount == 2)
                        effects.Pixel(x, maxY - n, x + hue, 255);
                        //backgroundLayer.drawPixel(x, maxY - n, effects.ColorFromCurrentPalette(x + hue));
                }
                break;

            case 1:
                for (int y = 0; y < MATRIX_HEIGHT; y++) {
                    n = quadwave8(y * 2 + theta) / scale;
                    effects.Pixel(n, y, y + hue, 255);
                    //backgroundLayer.drawPixel(n, y, effects.ColorFromCurrentPalette(y + hue));
                    if (waveCount == 2)
                        effects.Pixel(maxX - n, y, y + hue, 255);
                        //backgroundLayer.drawPixel(maxX - n, y, effects.ColorFromCurrentPalette(y + hue));
                }
                break;

            case 2:
                for (int x = 0; x < MATRIX_WIDTH; x++) {
                    n = quadwave8(x * 2 - theta) / scale;
                    effects.Pixel(x, n, x + hue, 255);
                    //backgroundLayer.drawPixel(x, n, effects.ColorFromCurrentPalette(x + hue));
                    if (waveCount == 2)
                        effects.Pixel(x, maxY - n, x + hue, 255);
                        //backgroundLayer.drawPixel(x, maxY - n, effects.ColorFromCurrentPalette(x + hue));
                }
                break;

            case 3:
                for (int y = 0; y < MATRIX_HEIGHT; y++) {
                    n = quadwave8(y * 2 - theta) / scale;
                    effects.Pixel(n, y, y + hue, 255);
                    //backgroundLayer.drawPixel(n, y, effects.ColorFromCurrentPalette(y + hue));
                    if (waveCount == 2)
                        effects.Pixel(maxX - n, y, y + hue, 255);
                        //backgroundLayer.drawPixel(maxX - n, y, effects.ColorFromCurrentPalette(y + hue));
                }
                break;
        }

        effects.DimAll(254);

        if (thetaUpdate >= thetaUpdateFrequency) {
            thetaUpdate = 0;
            theta++;
        }
        else {
            thetaUpdate++;
        }

        if (hueUpdate >= hueUpdateFrequency) {
            hueUpdate = 0;
            hue++;
        }
        else {
            hueUpdate++;
        }

        return 0;
    }
};

#endif