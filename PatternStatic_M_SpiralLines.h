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

#ifndef PatternSpiralLines_H
#define PatternSpiralLines_H

class PatternSpiralLines : public Drawable {
private:
    // Timer stuff (Oszillators)
    struct timer {
        unsigned long takt;
        unsigned long lastMillis;
        unsigned long count;
        int delta;
        byte up;
        byte down;
    };
    timer multiTimer[5];

    int timers = sizeof(multiTimer) / sizeof(multiTimer[0]);

    // counts all variables with different speeds linear up and down
    void UpdateTimers()
    {
        unsigned long now = millis();
        for (int i = 0; i < timers; i++)
        {
            while (now - multiTimer[i].lastMillis >= multiTimer[i].takt)
            {
                multiTimer[i].lastMillis += multiTimer[i].takt;
                multiTimer[i].count = multiTimer[i].count + multiTimer[i].delta;
                if ((multiTimer[i].count == multiTimer[i].up) || (multiTimer[i].count == multiTimer[i].down))
                {
                    multiTimer[i].delta = -multiTimer[i].delta;
                }
            }
        }
    }

public:
    PatternSpiralLines() {
      name = (char *)"Spiral";
      id = "P";
    }


    // #############
    // ### START ###
    // #############
    void start(uint8_t _pattern) {
        // set all counting directions positive for the beginning
        for (int i = 0; i < timers; i++) multiTimer[i].delta = 1;

        // set range (up/down), speed (takt=ms between steps) and starting point of all oszillators

        unsigned long now = millis();

        multiTimer[0].lastMillis = now;
        multiTimer[0].takt = 42;     //x1
        multiTimer[0].up = MATRIX_WIDTH - 1;
        multiTimer[0].down = 0;
        multiTimer[0].count = 0;

        multiTimer[1].lastMillis = now;
        multiTimer[1].takt = 55;     //y1
        multiTimer[1].up = MATRIX_HEIGHT - 1;
        multiTimer[1].down = 0;
        multiTimer[1].count = 0;

        multiTimer[2].lastMillis = now;
        multiTimer[2].takt = 3;      //color
        multiTimer[2].up = 255;
        multiTimer[2].down = 0;
        multiTimer[2].count = 0;

        multiTimer[3].lastMillis = now;
        multiTimer[3].takt = 71;     //x2  
        multiTimer[3].up = MATRIX_WIDTH - 1;
        multiTimer[3].down = 0;
        multiTimer[3].count = 0;

        multiTimer[4].lastMillis = now;
        multiTimer[4].takt = 89;     //y2
        multiTimer[4].up = MATRIX_HEIGHT - 1;
        multiTimer[4].down = 0;
        multiTimer[4].count = 0;
    }

    // ##################
    // ### DRAW FRAME ###
    // ##################
    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {
        // manage the Oszillators
        UpdateTimers();

        // only show the lines if there is audio present
        if (!fftData.noAudio) {
            // draw just a line defined by 5 oszillators
            byte audio;
            audio = fftData.specData[40];
            if (audio > 127) audio = 127;
            audio = audio * 2;
            effects.BresenhamLine(
                multiTimer[3].count,  // x1
                multiTimer[4].count,  // y1
                multiTimer[0].count,  // x2
                multiTimer[1].count,  // y2
                multiTimer[2].count, audio); // color, brightness

            // AuroraDrop DUPLICATE draw just a line defined by 5 oszillators
            audio = fftData.specData[44];
            if (audio > 127) audio = 127;
            audio = audio * 2;
            effects.BresenhamLine(
                multiTimer[3].count / 2,  // x1
                multiTimer[4].count / 2,  // y1
                multiTimer[0].count / 2,  // x2
                multiTimer[1].count / 2,  // y2
                multiTimer[2].count, audio); // color

            // AuroraDrop DUPLICATE draw just a line defined by 5 oszillators
            audio = fftData.specData[48];
            if (audio > 127) audio = 127;
            audio = audio * 2;
            effects.BresenhamLine(
                MATRIX_WIDTH - multiTimer[3].count,  // x1
                MATRIX_HEIGHT - multiTimer[4].count,  // y1
                MATRIX_WIDTH - multiTimer[0].count,  // x2
                MATRIX_HEIGHT - multiTimer[1].count,  // y2
                multiTimer[2].count, audio); // color, brightness

            // AuroraDrop DUPLICATE draw just a line defined by 5 oszillators
            audio = fftData.specData[52];
            if (audio > 127) audio = 127;
            audio = audio * 2;
            effects.BresenhamLine(
                MATRIX_WIDTH - multiTimer[3].count / 2,  // x1
                MATRIX_HEIGHT - multiTimer[4].count / 2,  // y1
                MATRIX_WIDTH - multiTimer[0].count / 2,  // x2
                MATRIX_HEIGHT - multiTimer[1].count / 2,  // y2
                multiTimer[2].count, audio); // color, brightness
        }


        // manipulate the screen buffer
        // with fixed parameters (could be oszillators too)
        // Params: center x, y, radius, scale color down
        // --> NOTE: Affects always a SQUARE with an odd length
     //   effects.SpiralStream(15, 15, 10, 128);

        effects.SpiralStream(31, 15, 64, 128);        // for 64 pixel wide matrix!
      //  effects.SpiralStream(47, 15, 10, 128);        // for 64 pixel wide matrix!

        // why not several times?!
       // effects.SpiralStream(16, 6, 6, 128);
       // effects.SpiralStream(10, 24, 10, 128);

        // increase the contrast
        effects.DimAll(250);
        //////effects.ShowFrame();

        return 0;   // 4 
    }
};

#endif