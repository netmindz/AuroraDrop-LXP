

// FREEZES


#ifndef PatternSpin_H

class PatternSpin : public Drawable {
public:
    PatternSpin() {
      name = (char *)"Spin";
      id2 = 1;
    }

    float degrees = 0;
    float radius = 16;

    float speedStart = 10;
    float velocityStart = 0.6;

    float maxSpeed = 20;

    float speed = speedStart;
    float velocity = velocityStart;

    void start(uint8_t _pattern) {
        speed = speedStart;
        velocity = velocityStart;
        degrees = 0;
    }

    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
        effects.DimAll(250);

        radius = serialData.specData8[1] / 6; // bass
        if (radius > MATRIX_CENTER_X/2 - 1) radius = MATRIX_CENTER_X/2 - 1;


        CRGB color1 = effects.ColorFromCurrentPalette(speed * 8);
        CRGB color2 = effects.ColorFromCurrentPalette((speed * 8) + 128);

        // start position
        int x;
        int y;

        // target position
        float targetDegrees = degrees + speed;
        float targetRadians = radians(targetDegrees);
        int targetX = (int) (MATRIX_CENTER_X + radius * cos(targetRadians));
        int targetY = (int) (MATRIX_CENTER_Y - radius * sin(targetRadians));

        float tempDegrees = degrees;

        do{
            float radians = radians(tempDegrees);
            x = (int) (MATRIX_CENTER_X + radius * cos(radians));
            y = (int) (MATRIX_CENTER_Y - radius * sin(radians));

            //backgroundLayer.drawPixel(x, y, color);
            //backgroundLayer.drawPixel(y, x, color);
            effects.drawBackgroundFastLEDPixelCRGB(x, y, color1);
            effects.drawBackgroundFastLEDPixelCRGB(y, x, color1);

            //effects.drawBackgroundFastLEDPixelCRGB(MATRIX_WIDTH - x, y, color2);
            //effects.drawBackgroundFastLEDPixelCRGB(MATRIX_WIDTH - y, x, color2);


            tempDegrees += 1;
            if (tempDegrees >= 360)
                tempDegrees = 0;
        } while (x != targetX || y != targetY);

        degrees += speed;

        // add velocity to the particle each pass around the accelerator
        if (degrees >= 360) {
            degrees = 0;
            speed += velocity;
            if (speed <= speedStart) {
                speed = speedStart;
                velocity *= -1;
            }
            else if (speed > maxSpeed){
                speed = maxSpeed - velocity;
                velocity *= -1;
            }
        }

        return 0;
    }
};

#endif