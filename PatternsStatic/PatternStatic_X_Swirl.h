
#ifndef PatternStaticSwirl_H
#define PatternStaticSwirl_H

class PatternStaticSwirl : public Drawable {
    private:
        uint8_t counter = 0 ;

    public:
        PatternStaticSwirl() {
            name = (char *)"Swirl";
            id = "W";
            enabled = true;
        }


        // ------------------ start -------------------
        void start(uint8_t _pattern) {
            counter = 0 ;

        }


        // --------------------- draw frame -------------------------
        unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

            // Use two out-of-sync sine waves
            uint8_t  i = beatsin8( 7, 0, MATRIX_WIDTH - 1);
            uint8_t  j = beatsin8( 9, 0, MATRIX_HEIGHT - 1);

            // Also calculate some reflections
            uint8_t ni = (MATRIX_WIDTH - 1) - i;
            uint8_t nj = (MATRIX_HEIGHT - 1) - j;

            // The color of each point shifts over time, each at a different speed.
            uint16_t ms = millis();
            effects.leds[XY( i, j)] += CHSV( ms / 11, 255, 255);
            effects.leds[XY( j, i)] += CHSV( ms / 13, 255, 255);
            effects.leds[XY(ni, nj)] += CHSV( ms / 17, 255, 255);
            effects.leds[XY(nj, ni)] += CHSV( ms / 29, 255, 255);
            effects.leds[XY( i, nj)] += CHSV( ms / 37, 255, 255);
            effects.leds[XY(ni, j)] += CHSV( ms / 41, 255, 255);

            effects.BresLine(i, j, j, i, 16, 128, LINEARBLEND);
            effects.BresLine(ni, nj, nj, ni, 32, 255, LINEARBLEND);

            effects.BresLine(i, nj, j, ni, 64, 128, LINEARBLEND);
            effects.BresLine(ni, j, nj, i, 80, 128, LINEARBLEND);

            effects.BresLine(ni, j, j, ni, 64, 128, LINEARBLEND);
            effects.BresLine(ni, j, j, ni, 64, 128, LINEARBLEND);

            effects.BresLine(i, nj, nj, i, 64, 128, LINEARBLEND);
            effects.BresLine(i, nj, nj, i, 64, 128, LINEARBLEND);

            return 0;
        }

};

#endif
