/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from "Funky Clouds" by Stefan Petrick: https://gist.github.com/anonymous/876f908333cd95315c35
 * Portions of this code are adapted from "NoiseSmearing" by Stefan Petrick: https://gist.github.com/StefanPetrick/9ee2f677dbff64e3ba7a
 * Copyright (c) 2014 Stefan Petrick
 * http://www.stefan-petrick.de/wordpress_beta
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

#ifndef Effects_H
#define Effects_H

/* ---------------------------- GLOBAL CONSTANTS ----------------------------- */

const uint16_t NUM_LEDS = (MATRIX_WIDTH * MATRIX_HEIGHT) + 1; // one led spare to capture out of bounds

// forward declaration
//
uint16_t XY16( uint16_t x, uint16_t y);

// AuroraDrop Additions:  canvas forward declarations
//
//uint16_t CANVAS_FULL( uint16_t x, uint16_t y);      // full width canvas
uint16_t CANVAS_HALF( uint16_t x, uint16_t y);        // half width
//uint16_t CANVAS_QUARTER( uint16_t x, uint16_t y);   // quarter


/* Convert x,y co-ordinate to flat array index. 
 * x and y positions start from 0, so must not be >= 'real' panel width or height 
 * (i.e. 64 pixels or 32 pixels.).  Max value: MATRIX_WIDTH-1 etc.
 * Ugh... uint8_t - really??? this weak method can't cope with 256+ pixel matrixes :(
 */
uint16_t XY( uint8_t x, uint8_t y) {

    return XY16(x, y);

}

/**
 *  The one for 256+ matrixes
 *  otherwise this:
 *    for (uint8_t i = 0; i < MATRIX_WIDTH; i++) {}
 *  turns into an infinite loop
 */
uint16_t XY16( uint16_t x, uint16_t y) {
    
    if( x >= MATRIX_WIDTH) {
        
        return 0;

    }

    if (y >= MATRIX_HEIGHT) {
        
        return 0;

    }

    return (y * MATRIX_WIDTH) + x + 1; // everything offset by one to capute out of bounds stuff - never displayed by ShowFrame()

}

uint8_t beatcos8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timebase = 0, uint8_t phase_offset = 0) {

    uint8_t beat = beat8(beats_per_minute, timebase);
    uint8_t beatcos = cos8(beat + phase_offset);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatcos, rangewidth);
    uint8_t result = lowest + scaledbeat;

    return result;

}

uint8_t mapsin8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255) {

    uint8_t beatsin = sin8(theta);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatsin, rangewidth);
    uint8_t result = lowest + scaledbeat;

    return result;

}

uint8_t mapcos8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255) {

    uint8_t beatcos = cos8(theta);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatcos, rangewidth);
    uint8_t result = lowest + scaledbeat;

    return result;
  
}

// Array of temperature readings at each simulation cell
//byte heat[NUM_LEDS];    // none of the currently enabled effects uses this

uint32_t noise_x;
uint32_t noise_y;
uint32_t noise_z;
uint32_t noise_scale_x;
uint32_t noise_scale_y;

//uint8_t noise[MATRIX_WIDTH][MATRIX_HEIGHT];
uint8_t **noise = nullptr;  // we will allocate mem later
uint8_t noisesmoothing;

class Effects {

    public:

    CRGB *leds;
    //CRGB leds[NUM_LEDS];
    //CRGB leds2[NUM_LEDS]; // Faptastic: getting rid of this and any dependant effects or algos. to save memory 24*64*32 bytes of ram (50k).

    // AuroraDrop: adding new for canvases
    //CRGB *canvasF;    // full size
    CRGB *canvasH;    // half width canvas no.1
    CRGB *canvasH2;    // half width canvas no.2
    CRGB *canvasQ;    // quarter

    Effects(){

        // we do dynamic allocation for leds buffer, otherwise esp32 toolchain can't link static arrays of such a big size for 256+ matrixes
        //
        leds = (CRGB *)malloc(NUM_LEDS * sizeof(CRGB));
        //canvasF = (CRGB *)malloc(NUM_LEDS * sizeof(CRGB));
        canvasH = (CRGB *)malloc(NUM_LEDS * sizeof(CRGB) / 4);
        canvasH2 = (CRGB *)malloc(NUM_LEDS * sizeof(CRGB) / 4);
        canvasQ = (CRGB *)malloc(NUM_LEDS * sizeof(CRGB) / 16);

        // allocate mem for noise effect
        // (there should be some guards for malloc errors eventually)
        //
        noise = (uint8_t **)malloc(MATRIX_WIDTH * sizeof(uint8_t *));

        for (int i = 0; i < MATRIX_WIDTH; ++i) {

            noise[i] = (uint8_t *)malloc(MATRIX_HEIGHT * sizeof(uint8_t));

        }

        ClearFrame();

    }
  
    ~Effects(){

        free(leds);
        //free(canvasF);
        free(canvasH);
        free(canvasH2);
        free(canvasQ);

        for (int i = 0; i < MATRIX_WIDTH; ++i) {

            free(noise[i]);

        }

        free(noise);

    }

    /* The only 'framebuffer' we have is what is contained in the leds and leds2 variables.
    * We don't store what the color a particular pixel might be, other than when it's turned
    * into raw electrical signal output gobbly-gook (i.e. the DMA matrix buffer), but this * is not reversible.
    * 
    * As such, any time these effects want to write a pixel color, we first have to update
    * the leds or leds2 array, and THEN write it to the RGB panel. This enables us to 'look up' the array to see what a pixel color was previously, each drawFrame().
    */
    void drawBackgroundFastLEDPixelCRGB(int16_t x, int16_t y, CRGB color) {

        leds[XY(x, y)] = color;

    }

    // write one pixel with the specified color from the current palette to coordinates
    //
    void Pixel(int x, int y, uint8_t colorIndex, uint8_t brightness = 255) {
        
        leds[XY(x, y)] = ColorFromCurrentPalette(colorIndex, brightness);

    }
  
    void PrepareFrame() {

        // leds = (CRGB*) backgroundLayer.backBuffer();

    }

    void ShowFrame() {

        #if (FASTLED_VERSION >= 3001000)
        nblendPaletteTowardPalette(currentPalette, targetPalette, 24);   // was commented out
        #else
        currentPalette = targetPalette;
        #endif

        for (int y=0; y<MATRIX_HEIGHT; ++y) {

            for (int x=0; x<MATRIX_WIDTH; ++x) {

                //Serial.printf("Flushing x, y coord %d, %d\n", x, y);

                uint16_t _pixel = XY16(x,y);

                dma_display->drawPixelRGB888( x, y, leds[_pixel].r, leds[_pixel].g, leds[_pixel].b);
                
            } // end loop to copy fast led to the dma matrix
            
        }

    }

    // scale the brightness of the screenbuffer down
    //
    void DimAll(byte value) {

        for (int i = 0; i < NUM_LEDS; i++) {

            leds[i].nscale8(value);

        }

    }  

    // scale the brightness of the screenbuffer down
    //
    void DimPixel(CRGB *canvas, int led, byte value) {

        canvas[led].nscale8(value);

    } 

    void ClearFrame() {

        memset(leds, 0x00, NUM_LEDS * sizeof(CRGB)); // flush

    }
  
    
    /*
    void CircleStream(uint8_t value) {
        DimAll(value); ShowFrame();

        for (uint8_t offset = 0; offset < MATRIX_CENTER_X; offset++) {
        boolean hasprev = false;
        uint16_t prevxy = 0;

        for (uint8_t theta = 0; theta < 255; theta++) {
            uint8_t x = mapcos8(theta, offset, (MATRIX_WIDTH - 1) - offset);
            uint8_t y = mapsin8(theta, offset, (MATRIX_HEIGHT - 1) - offset);

            uint16_t xy = XY(x, y);

            if (hasprev) {
            leds[prevxy] += leds[xy];
            }

            prevxy = xy;
            hasprev = true;
        }
        }

        for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
            uint16_t xy = XY(x, y);
            leds[xy] = leds2[xy];
            leds[xy].nscale8(value);
            leds2[xy].nscale8(value);
        }
        }
    }
    */

    // palettes
    //
    static const int paletteCount = 10;
    int paletteIndex = -1;
    TBlendType currentBlendType = LINEARBLEND;
    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    char* currentPaletteName;

    static const int HeatColorsPaletteIndex = 6;
    static const int RandomPaletteIndex = 9;

    CRGBPalette16 AllRed_p = {
        CRGB::Red,
        CRGB::DarkRed,
        CRGB::DarkRed,
        CRGB::Red,
        CRGB::DarkRed,
        CRGB::DarkRed,
        CRGB::Red,
        CRGB::DarkRed,
        CRGB::Red,
        CRGB::DarkRed,
        CRGB::Red,
        CRGB::DarkRed,
        CRGB::Red,
        CRGB::DarkRed,
        CRGB::Red,
        CRGB::Gray
    };

    void Setup() {

        currentPalette = RainbowColors_p;
        currentPalette = AllRed_p;
        
        loadPalette(0);
        NoiseVariablesSetup();

    }

    void CyclePalette(int offset = 1) {

        loadPalette(paletteIndex + offset);
        
    }

    void RandomPalette() {

        loadPalette(RandomPaletteIndex);
        
    }

    void loadPalette(int index) {

        // override for testing
        //
        //setupYellowBluePalette();
        //currentPaletteName = (char *)"YellowBlue";
        //return;

        paletteIndex = index;

        if (paletteIndex >= paletteCount) {

            paletteIndex = 0;
        
        } else if (paletteIndex < 0) {

            paletteIndex = paletteCount - 1;

        }
        
        switch (paletteIndex) {

            case 0:
                targetPalette = RainbowColors_p;
                currentPaletteName = (char *)"Rainbow";
            break;

            case 1:
                targetPalette = OceanColors_p;
                currentPaletteName = (char *)"Ocean";
            break;

            case 2:
                targetPalette = CloudColors_p;
                currentPaletteName = (char *)"Cloud";
            break;

            case 3:
                targetPalette = ForestColors_p;
                currentPaletteName = (char *)"Forest";
            break;
            
            case 4:
                targetPalette = PartyColors_p;
                currentPaletteName = (char *)"Party";
            break;

            case 5:
                setupYellowBluePalette();
                currentPaletteName = (char *)"YellowBlue";
                //setupGrayscalePalette();
                //currentPaletteName = (char *)"Greyscale";
            break;

            case HeatColorsPaletteIndex:
                targetPalette = HeatColors_p;
                currentPaletteName = (char *)"Heat";
            break;

            case 7:
                targetPalette = LavaColors_p;
                currentPaletteName = (char *)"Lava";
            break;

            case 8:
                setupIcePalette();
                currentPaletteName = (char *)"Ice";
            break;

            case RandomPaletteIndex:
                loadPalette(random(0, paletteCount - 1));
                paletteIndex = RandomPaletteIndex;
                currentPaletteName = (char *)"Random";
            break;

        }

    }

    void setPalette(String paletteName) {

        if (paletteName == "Rainbow") {
        
            loadPalette(0);

        } else if (paletteName == "Ocean") {
            
            loadPalette(1);

        } else if (paletteName == "Cloud") {
            
            loadPalette(2);
        
        } else if (paletteName == "Forest") {
        
            loadPalette(3);
        
        } else if (paletteName == "Party") {

            loadPalette(4);

        } else if (paletteName == "YellowBlue") {

            loadPalette(5);

        } else if (paletteName == "Heat") {

            loadPalette(6);

        } else if (paletteName == "Lava") {

            loadPalette(7);

        } else if (paletteName == "Ice") {
            
            loadPalette(8);

        } else if (paletteName == "Random") {
            
            RandomPalette();

        }

    }

    void listPalettes() {

        Serial.println(F("{"));
        Serial.print(F("  \"count\": "));
        Serial.print(paletteCount);
        Serial.println(",");
        Serial.println(F("  \"results\": ["));

        String paletteNames [] = {
            "Rainbow",
            // "RainbowStripe",
            "Ocean",
            "Cloud",
            "Forest",
            "Party",
            "YellowBlue", //"Grayscale",
            "Heat",
            "Lava",
            "Ice",
            "Random"
        };

        for (int i = 0; i < paletteCount; i++) {

            Serial.print(F("    \""));
            Serial.print(paletteNames[i]);
            
            if (i == paletteCount - 1) {

                Serial.println(F("\""));

            } else {

                Serial.println(F("\","));

            }

        }

        Serial.println("  ]");
        Serial.println("}");

    }

    void setupYellowBluePalette() {

        targetPalette = CRGBPalette16(CRGB::Purple, CRGB::Blue, CRGB::Purple, CRGB::Blue);
        //targetPalette = {CRGB::Yellow, CRGB::Blue, CRGB::Yellow, CRGB::Blue, CRGB::Yellow, CRGB::Blue, CRGB::Yellow, CRGB::Blue};
        //targetPalette = {CRGB::Purple, CRGB::MediumPurple, CRGB::Yellow, CRGB::Purple, CRGB::MediumPurple, CRGB::Yellow};
        
    }


    void setupGrayscalePalette() {

        // looks poor sometimes, don't use

        targetPalette = CRGBPalette16(CRGB::Black, CRGB::White);

    }

    void setupIcePalette() {

        targetPalette = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

    }

    // Oscillators and Emitters

    // the oscillators: linear ramps 0-255
    //
    byte osci[6];

    // sin8(osci) swinging between 0 to MATRIX_WIDTH - 1
    //
    byte p[6];

    // set the speeds (and by that ratios) of the oscillators here
    //
    void MoveOscillators() {

        osci[0] = osci[0] + 5;
        osci[1] = osci[1] + 2;
        osci[2] = osci[2] + 3;
        osci[3] = osci[3] + 4;
        osci[4] = osci[4] + 1;
        
        if (osci[4] % 2 == 0) {

            osci[5] = osci[5] + 1; // .5

        }
        
        for (int i = 0; i < 4; i++) {
            
            p[i] = map8(sin8(osci[i]), 0, MATRIX_WIDTH - 1); //why? to keep the result in the range of 0-MATRIX_WIDTH (matrix size)
        
        }

    }
 
    // All the caleidoscope functions work directly within the screenbuffer (leds array).
    // Draw whatever you like in the area x(0-15) and y (0-15) and then copy it arround.

    // rotates the first 16x16 quadrant 3 times onto a 32x32 (+90 degrees rotation for each one)
    //
    void Caleidoscope1() {

        for (int x = 0; x < MATRIX_CENTER_X; x++) {

            for (int y = 0; y < MATRIX_CENTER_Y; y++) {

                leds[XY16(MATRIX_WIDTH - 1 - x, y)] = leds[XY16(x, y)];
                leds[XY16(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y)] = leds[XY16(x, y)];                
                leds[XY16(x, MATRIX_HEIGHT - 1 - y)] = leds[XY16(x, y)];

            }

        }

    }

    // mirror the first 16x16 quadrant 3 times onto a 32x32
    //
    void Caleidoscope2() {

        if (MATRIX_WIDTH == MATRIX_HEIGHT) {

            for (int x = 0; x < MATRIX_CENTER_X; x++) {

                for (int y = 0; y < MATRIX_CENTER_Y; y++) {

                    leds[XY16(MATRIX_WIDTH - 1 - x, y)] = leds[XY16(y, x)];
                    leds[XY16(x, MATRIX_HEIGHT - 1 - y)] = leds[XY16(y, x)];
                    leds[XY16(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y)] = leds[XY16(x, y)];

                }

            }

        } else {
            
            // TODO : fix this 

            for (int x = 0; x < MATRIX_CENTER_X; x++) {

                for (int y = 0; y < MATRIX_CENTER_Y; y++) {

                    leds[XY16(MATRIX_WIDTH - 1 - x, y)] = leds[XY16(x, y)];
                    leds[XY16(x, MATRIX_HEIGHT - 1 - y)] = leds[XY16(x, y)];
                    leds[XY16(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y)] = leds[XY16(x, y)];
                    
                }

            }

        }

    }

    // copy one diagonal triangle into the other one within a 16x16
    //
    void Caleidoscope3() {

        for (int x = 0; x <= MATRIX_CENTER_X && x < MATRIX_HEIGHT; x++) {

            for (int y = 0; y <= x && y<MATRIX_HEIGHT; y++) {

                leds[XY16(x, y)] = leds[XY16(y, x)];

            }

        }

    }

    // copy one diagonal triangle into the other one within a 16x16 (90 degrees rotated compared to Caleidoscope3)
    //
    void Caleidoscope4() {
    
        for (int x = 0; x <= MATRIX_CENTER_X; x++) {
    
            for (int y = 0; y <= MATRIX_CENTER_Y - x; y++) {
    
                leds[XY16(MATRIX_CENTER_Y - y, MATRIX_CENTER_X - x)] = leds[XY16(x, y)];
    
            }
    
        }
    
    }

    // copy one diagonal triangle into the other one within a 8x8
    //
    void Caleidoscope5() {

        for (int x = 0; x < MATRIX_WIDTH / 4; x++) {

            for (int y = 0; y <= x && y<=MATRIX_HEIGHT; y++) {

                leds[XY16(x, y)] = leds[XY16(y, x)];

            }
        
        }

        for (int x = MATRIX_WIDTH / 4; x < MATRIX_WIDTH / 2; x++) {

            for (int y = MATRIX_HEIGHT / 4; y >= 0; y--) {

                leds[XY16(x, y)] = leds[XY16(y, x)];

            }

        }

    }

    void Caleidoscope6() {

        for (int x = 1; x < MATRIX_CENTER_X; x++) {

            leds[XY16(7 - x, 7)] = leds[XY16(x, 0)];

        } //a

        for (int x = 2; x < MATRIX_CENTER_X; x++) {

            leds[XY16(7 - x, 6)] = leds[XY16(x, 1)];

        } //b

        for (int x = 3; x < MATRIX_CENTER_X; x++) {

            leds[XY16(7 - x, 5)] = leds[XY16(x, 2)];
            
        } //c

        for (int x = 4; x < MATRIX_CENTER_X; x++) {

            leds[XY16(7 - x, 4)] = leds[XY16(x, 3)];

        } //d

        for (int x = 5; x < MATRIX_CENTER_X; x++) {

            leds[XY16(7 - x, 3)] = leds[XY16(x, 4)];

        } //e

        for (int x = 6; x < MATRIX_CENTER_X; x++) {

            leds[XY16(7 - x, 2)] = leds[XY16(x, 5)];

        } //f

        for (int x = 7; x < MATRIX_CENTER_X; x++) {

            leds[XY16(7 - x, 1)] = leds[XY16(x, 6)];

        } //g

    }

    // create a square twister to the left or counter-clockwise
    // x and y for center, r for radius
    // AuroraDrop extension: CanvasId, 0=default/main canvas, 1=temporary 32x32 canvas1, 2=1=temporary 32x32 canvas1
    //
    void SpiralStream(int x, int y, int r, byte dimm, uint8_t CanvasId = 0) {

        switch (CanvasId) {

            case 1:
                // apply effect to temporary quarter sized array 1
            break;
            
            case 2:
                // apply effect to temporary quarter sized array 1
            break;

            default:
                
                // standard, apply effect to main effects array
                //
                for (int d = r; d >= 0; d--) { // from the outside to the inside

                    for (int i = x - d; i <= x + d; i++) {
                        leds[XY16(i, y - d)] += leds[XY16(i + 1, y - d)]; // lowest row to the right
                        leds[XY16(i, y - d)].nscale8(dimm);
                    }

                    for (int i = y - d; i <= y + d; i++) {

                        leds[XY16(x + d, i)] += leds[XY16(x + d, i + 1)]; // right colum up
                        leds[XY16(x + d, i)].nscale8(dimm);

                    }

                    for (int i = x + d; i >= x - d; i--) {

                        leds[XY16(i, y + d)] += leds[XY16(i - 1, y + d)]; // upper row to the left
                        leds[XY16(i, y + d)].nscale8(dimm);

                    }

                    for (int i = y + d; i >= y - d; i--) {

                        leds[XY16(x - d, i)] += leds[XY16(x - d, i - 1)]; // left colum down
                        leds[XY16(x - d, i)].nscale8(dimm);

                    }

                }

            break;
    
        }
    
    }

    // TONY create a square twister to the right or clockwise
    // x and y for center, r for radius
    //
    void SpiralStreamVer2(int x, int y, int r, byte dimm) {

        for (int d = 0; d < r; d++) { // from the outside to the inside

            for (int i = x + d; i >= x - d; i--) {

                leds[XY16(i, y - d)] += leds[XY16(i + 1, y - d)]; // lowest row to the right
                leds[XY16(i, y - d)].nscale8(dimm);

            }

            for (int i = y + d; i >= y - d; i--) {

                leds[XY16(x + d, i)] += leds[XY16(x + d, i + 1)]; // right colum up
                leds[XY16(x + d, i)].nscale8(dimm);

            }

            for (int i = x - d; i <= x + d; i++) {

                leds[XY16(i, y + d)] += leds[XY16(i - 1, y + d)]; // upper row to the left
                leds[XY16(i, y + d)].nscale8(dimm);

            }

            for (int i = y - d; i <= y + d; i++) {

                leds[XY16(x - d, i)] += leds[XY16(x - d, i - 1)]; // left colum down
                leds[XY16(x - d, i)].nscale8(dimm);

            }

        }

    }

    // expand everything within a circle
    //
    void Expand(int centerX, int centerY, int radius, byte dimm) {
            
        if (radius == 0)
        return;

        int currentRadius = radius;

        while (currentRadius > 0) {

            int a = radius, b = 0;
            int radiusError = 1 - a;

            int nextRadius = currentRadius - 1;
            int nextA = nextRadius - 1, nextB = 0;
            int nextRadiusError = 1 - nextA;

            while (a >= b) {

                // move them out one pixel on the radius
                //
                leds[XY16(a + centerX, b + centerY)] = leds[XY16(nextA + centerX, nextB + centerY)];
                leds[XY16(b + centerX, a + centerY)] = leds[XY16(nextB + centerX, nextA + centerY)];
                leds[XY16(-a + centerX, b + centerY)] = leds[XY16(-nextA + centerX, nextB + centerY)];
                leds[XY16(-b + centerX, a + centerY)] = leds[XY16(-nextB + centerX, nextA + centerY)];
                leds[XY16(-a + centerX, -b + centerY)] = leds[XY16(-nextA + centerX, -nextB + centerY)];
                leds[XY16(-b + centerX, -a + centerY)] = leds[XY16(-nextB + centerX, -nextA + centerY)];
                leds[XY16(a + centerX, -b + centerY)] = leds[XY16(nextA + centerX, -nextB + centerY)];
                leds[XY16(b + centerX, -a + centerY)] = leds[XY16(nextB + centerX, -nextA + centerY)];

                // dim them
                //
                leds[XY16(a + centerX, b + centerY)].nscale8(dimm);
                leds[XY16(b + centerX, a + centerY)].nscale8(dimm);
                leds[XY16(-a + centerX, b + centerY)].nscale8(dimm);
                leds[XY16(-b + centerX, a + centerY)].nscale8(dimm);
                leds[XY16(-a + centerX, -b + centerY)].nscale8(dimm);
                leds[XY16(-b + centerX, -a + centerY)].nscale8(dimm);
                leds[XY16(a + centerX, -b + centerY)].nscale8(dimm);
                leds[XY16(b + centerX, -a + centerY)].nscale8(dimm);

                b++;

                if (radiusError < 0) {
                    
                    radiusError += 2 * b + 1;
                
                } else {

                    a--;
                    radiusError += 2 * (b - a + 1);

                }

                nextB++;

                if (nextRadiusError < 0) {
                    
                    nextRadiusError += 2 * nextB + 1;
                
                } else {

                    nextA--;
                    nextRadiusError += 2 * (nextB - nextA + 1);

                }

            }

            currentRadius--;

        }

    }

    // give it a linear tail to the right
    //
    void StreamRight(byte scale, int fromX = 0, int toX = MATRIX_WIDTH, int fromY = 0, int toY = MATRIX_HEIGHT) {

        for (int x = fromX + 1; x < toX; x++) {

            for (int y = fromY; y < toY; y++) {

                leds[XY16(x, y)] += leds[XY16(x - 1, y)];
                leds[XY16(x, y)].nscale8(scale);

            }

        }

        for (int y = fromY; y < toY; y++) {

            leds[XY16(0, y)].nscale8(scale);
        
        }

    }

    // give it a linear tail to the left
    //
    void StreamLeft(byte scale, int fromX = MATRIX_WIDTH, int toX = 0, int fromY = 0, int toY = MATRIX_HEIGHT) {

        for (int x = toX; x < fromX; x++) {
        
            for (int y = fromY; y < toY; y++) {
            
                leds[XY16(x, y)] += leds[XY16(x + 1, y)];
                leds[XY16(x, y)].nscale8(scale);
            
            }
            
        }
        
        for (int y = fromY; y < toY; y++) {
        
            leds[XY16(0, y)].nscale8(scale);
        
        }

    }

    // TODO: CLEANUP BELOW
    
  // give it a linear tail downwards
  void StreamDown(byte scale)
  {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      for (int y = 1; y < MATRIX_HEIGHT; y++) {
        leds[XY16(x, y)] += leds[XY16(x, y - 1)];
        leds[XY16(x, y)].nscale8(scale);
      }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
      leds[XY16(x, 0)].nscale8(scale);
  }

  // give it a linear tail upwards
  void StreamUp(byte scale)
  {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      for (int y = MATRIX_HEIGHT - 2; y >= 0; y--) {
        leds[XY16(x, y)] += leds[XY16(x, y + 1)];
        leds[XY16(x, y)].nscale8(scale);
      }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
      leds[XY16(x, MATRIX_HEIGHT - 1)].nscale8(scale);
  }

  // give it a linear tail up and to the left
  void StreamUpAndLeft(byte scale)
  {
    for (int x = 0; x < MATRIX_WIDTH - 1; x++) {
      for (int y = MATRIX_HEIGHT - 2; y >= 0; y--) {
        leds[XY16(x, y)] += leds[XY16(x + 1, y + 1)];
        leds[XY16(x, y)].nscale8(scale);
      }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
      leds[XY16(x, MATRIX_HEIGHT - 1)].nscale8(scale);
    for (int y = 0; y < MATRIX_HEIGHT; y++)
      leds[XY16(MATRIX_WIDTH - 1, y)].nscale8(scale);
  }

  // give it a linear tail up and to the right
  void StreamUpAndRight(byte scale)
  {
    for (int x = 0; x < MATRIX_WIDTH - 1; x++) {
      for (int y = MATRIX_HEIGHT - 2; y >= 0; y--) {
        leds[XY16(x + 1, y)] += leds[XY16(x, y + 1)];
        leds[XY16(x, y)].nscale8(scale);
      }
    }
    // fade the bottom row
    for (int x = 0; x < MATRIX_WIDTH; x++)
      leds[XY16(x, MATRIX_HEIGHT - 1)].nscale8(scale);

    // fade the right column
    for (int y = 0; y < MATRIX_HEIGHT; y++)
      leds[XY16(MATRIX_WIDTH - 1, y)].nscale8(scale);
  }

  // just move everything one line down
  void MoveDown() {
    for (int y = MATRIX_HEIGHT - 1; y > 0; y--) {
      for (int x = 0; x < MATRIX_WIDTH; x++) {
        leds[XY16(x, y)] = leds[XY16(x, y - 1)];
      }
    }
  }

  // just move everything one line down
  void VerticalMoveFrom(int start, int end) {
    for (int y = end; y > start; y--) {
      for (int x = 0; x < MATRIX_WIDTH; x++) {
        leds[XY16(x, y)] = leds[XY16(x, y - 1)];
      }
    }
  }

  // copy the rectangle defined with 2 points x0, y0, x1, y1
  // to the rectangle beginning at x2, x3
  void Copy(byte x0, byte y0, byte x1, byte y1, byte x2, byte y2) {
    for (int y = y0; y < y1 + 1; y++) {
      for (int x = x0; x < x1 + 1; x++) {
        leds[XY16(x + x2 - x0, y + y2 - y0)] = leds[XY16(x, y)];
      }
    }
  }

  // rotate + copy triangle (MATRIX_CENTER_X*MATRIX_CENTER_X)
  void RotateTriangle() {
    for (int x = 1; x < MATRIX_CENTER_X; x++) {
      for (int y = 0; y < x; y++) {
        leds[XY16(x, 7 - y)] = leds[XY16(7 - x, y)];
      }
    }
  }

  // mirror + copy triangle (MATRIX_CENTER_X*MATRIX_CENTER_X)
  void MirrorTriangle() {
    for (int x = 1; x < MATRIX_CENTER_X; x++) {
      for (int y = 0; y < x; y++) {
        leds[XY16(7 - y, x)] = leds[XY16(7 - x, y)];
      }
    }
  }

  // draw static rainbow triangle pattern (MATRIX_CENTER_XxWIDTH / 2)
  // (just for debugging)
  void RainbowTriangle() {
    for (int i = 0; i < MATRIX_CENTER_X; i++) {
      for (int j = 0; j <= i; j++) {
        Pixel(7 - i, j, i * j * 4);
      }
    }
  }

  //void BresenhamLine(int x0, int y0, int x1, int y1, byte colorIndex, uint8_t brightness = 255)
  void BresenhamLine(int x0, int y0, int x1, int y1, byte colorIndex, uint8_t brightness)
  {
    BresenhamLine(x0, y0, x1, y1, ColorFromCurrentPalette(colorIndex, brightness));
  }

  void BresenhamLine(int x0, int y0, int x1, int y1, CRGB color)
  {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    for (;;) {
      leds[XY16(x0, y0)] += color;
      if (x0 == x1 && y0 == y1) break;
      e2 = 2 * err;
      if (e2 > dy) {
        err += dy;
        x0 += sx;
      }
      if (e2 < dx) {
        err += dx;
        y0 += sy;
      }
    }
  }




  CRGB ColorFromCurrentPalette(uint8_t index = 0, uint8_t brightness = 255, TBlendType blendType = LINEARBLEND) {
    return ColorFromPalette(currentPalette, index, brightness, currentBlendType);
  }

  CRGB HsvToRgb(uint8_t h, uint8_t s, uint8_t v) {
    CHSV hsv = CHSV(h, s, v);
    CRGB rgb;
    hsv2rgb_spectrum(hsv, rgb);
    return rgb;
  }

  void NoiseVariablesSetup() {
    noisesmoothing = 200;

    noise_x = random16();
    noise_y = random16();
    noise_z = random16();
    noise_scale_x = 6000;
    noise_scale_y = 6000;
  }

  void FillNoise() {
    for (uint16_t i = 0; i < MATRIX_WIDTH; i++) {
      uint32_t ioffset = noise_scale_x * (i - MATRIX_CENTER_Y);

      for (uint16_t j = 0; j < MATRIX_HEIGHT; j++) {
        uint32_t joffset = noise_scale_y * (j - MATRIX_CENTER_Y);

        byte data = inoise16(noise_x + ioffset, noise_y + joffset, noise_z) >> 8;

        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8(olddata, noisesmoothing) + scale8(data, 256 - noisesmoothing);
        data = newdata;

        noise[i][j] = data;
      }
    }
  }

  // non leds2 memory version.
  void MoveX(byte delta) 
  {

    CRGB tmp = 0;

    for (int y = 0; y < MATRIX_HEIGHT; y++) 
    {
 
      // Shift Left: https://codedost.com/c/arraypointers-in-c/c-program-shift-elements-array-left-direction/
      // Computationally heavier but doesn't need an entire leds2 array

      tmp = leds[XY16(0, y)];      
      for (int m = 0; m < delta; m++)
      {
        // Do this delta time for each row... computationally expensive potentially.
        for(int x = 0; x < MATRIX_WIDTH; x++)
        {
            leds[XY16(x, y)] = leds [XY16(x+1, y)];
        }

        leds[XY16(MATRIX_WIDTH-1, y)] = tmp;
      }
   

      /*
      // Shift
      for (int x = 0; x < MATRIX_WIDTH - delta; x++) {
        leds2[XY(x, y)] = leds[XY(x + delta, y)];
      }

      // Wrap around
      for (int x = MATRIX_WIDTH - delta; x < MATRIX_WIDTH; x++) {
        leds2[XY(x, y)] = leds[XY(x + delta - MATRIX_WIDTH, y)];
      }
      */
    } // end row loop

    /*
    // write back to leds
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
      for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
        leds[XY(x, y)] = leds2[XY(x, y)];
      }
    }
    */
  }

  void MoveY(byte delta)
  {
    
    CRGB tmp = 0; 
    for (int x = 0; x < MATRIX_WIDTH; x++) 
    {
      tmp = leds[XY16(x, 0)];      
      for (int m = 0; m < delta; m++) // moves
      {
        // Do this delta time for each row... computationally expensive potentially.
        for(int y = 0; y < MATRIX_HEIGHT; y++)
        {
            leds[XY16(x, y)] = leds [XY16(x, y+1)];
        }

        leds[XY16(x, MATRIX_HEIGHT-1)] = tmp;
      }
    } // end column loop
  } /// MoveY

  // ------- AuroraDrop Additions: ----------------------------------------------------------------------------------------------

  #define CALEIDOSCOPE_COUNT 9

  uint16_t beatSineOsci[6];            // full 0-65535
  uint8_t beatSineOsci8[6];            // byte sized 0-255
  uint8_t beatSineOsciWidth[6];        // matrix width, 0-63 or whatever...
  uint8_t beatCosineOsciWidth[6];      // matrix width, 0-63 or whatever...
  uint8_t beatSawOsci8[6];
  uint8_t beatSawOsciWidth[6];
  uint8_t beatSquareOsci8[6];
  uint8_t beatSquareOsciWidth[6];


  // general system bpm oscillators, patterns can set up their own if they need to be more specific
  //
  void updateBpmOscillators() {
    // oscillators for sine wave forms at variuos rates proportional to the tempo
    // do we need all these? used anywhere?
    // scaled 0-65535
    beatSineOsci[0] = beatsin16(fftData.bpm, 0, 65535);
    beatSineOsci[1] = beatsin16(fftData.bpm / 2, 0, 65535);
    beatSineOsci[2] = beatsin16(fftData.bpm / 4, 0, 65535);
    beatSineOsci[3] = beatsin16(fftData.bpm / 8, 0, 65535);
    beatSineOsci[4] = beatsin16(fftData.bpm / 16, 0, 65535);
    beatSineOsci[5] = beatsin16(fftData.bpm / 32, 0, 65535);
    // scaled 0-255
    beatSineOsci8[0] = beatsin16(fftData.bpm, 0, 255);
    beatSineOsci8[1] = beatsin16(fftData.bpm / 2, 0, 255);
    beatSineOsci8[2] = beatsin16(fftData.bpm / 4, 0, 255);
    beatSineOsci8[3] = beatsin16(fftData.bpm / 8, 0, 255);
    beatSineOsci8[4] = beatsin16(fftData.bpm / 16, 0, 255);
    beatSineOsci8[5] = beatsin16(fftData.bpm / 32, 0, 255);
    // scaled for matrix width (e.g. 0-63)
    beatSineOsciWidth[0] = beatsin16(fftData.bpm, 0, MATRIX_HEIGHT-1);
    beatSineOsciWidth[1] = beatsin16(fftData.bpm / 2, 0, MATRIX_HEIGHT-1);
    beatSineOsciWidth[2] = beatsin16(fftData.bpm / 4, 0, MATRIX_HEIGHT-1);
    beatSineOsciWidth[3] = beatsin16(fftData.bpm / 8, 0, MATRIX_HEIGHT-1);
    beatSineOsciWidth[4] = beatsin16(fftData.bpm / 16, 0, MATRIX_HEIGHT-1);
    beatSineOsciWidth[5] = beatsin16(fftData.bpm / 32, 0, MATRIX_HEIGHT-1);
    beatCosineOsciWidth[0] = beatsin16(fftData.bpm, 0, MATRIX_HEIGHT-1, 0 , 16384);
    beatCosineOsciWidth[1] = beatsin16(fftData.bpm / 2, 0, MATRIX_HEIGHT-1, 0 , 16384);
    beatCosineOsciWidth[2] = beatsin16(fftData.bpm / 4, 0, MATRIX_HEIGHT-1, 0 , 16384);
    beatCosineOsciWidth[3] = beatsin16(fftData.bpm / 8, 0, MATRIX_HEIGHT-1, 0 , 16384);
    beatCosineOsciWidth[4] = beatsin16(fftData.bpm / 16, 0, MATRIX_HEIGHT-1, 0 , 16384);
    beatCosineOsciWidth[5] = beatsin16(fftData.bpm / 32, 0, MATRIX_HEIGHT-1, 0 , 16384);
    // oscillators for saw tooth wave forms, scaled 0-255
    beatSawOsci8[0] = beat8(fftData.bpm);
    beatSawOsci8[1] = beat8(fftData.bpm / 2);
    beatSawOsci8[2] = beat8(fftData.bpm / 4);
    beatSawOsci8[3] = beat8(fftData.bpm / 8);
    beatSawOsci8[4] = beat8(fftData.bpm / 16);
    beatSawOsci8[5] = beat8(fftData.bpm / 32);
    // scaled for matrix width
    beatSawOsciWidth[0] = map8(beatSawOsci8[0], 0, MATRIX_HEIGHT - 1);
    beatSawOsciWidth[1] = map8(beatSawOsci8[1], 0, MATRIX_HEIGHT - 1);
    beatSawOsciWidth[2] = map8(beatSawOsci8[2], 0, MATRIX_HEIGHT - 1);
    beatSawOsciWidth[3] = map8(beatSawOsci8[3], 0, MATRIX_HEIGHT - 1);
    beatSawOsciWidth[4] = map8(beatSawOsci8[4], 0, MATRIX_HEIGHT - 1);
    beatSawOsciWidth[5] = map8(beatSawOsci8[5], 0, MATRIX_HEIGHT - 1);
    // oscillators for square wave forms, scaled 0-255
    beatSquareOsci8[0] = squarewave8(beat8(fftData.bpm), 128);
    beatSquareOsci8[1] = squarewave8(beat8(fftData.bpm / 2), 128);
    beatSquareOsci8[2] = squarewave8(beat8(fftData.bpm / 4), 128);
    beatSquareOsci8[3] = squarewave8(beat8(fftData.bpm / 8), 128);
    beatSquareOsci8[4] = squarewave8(beat8(fftData.bpm / 16), 128);
    beatSquareOsci8[5] = squarewave8(beat8(fftData.bpm / 32), 128);

  }


  // AuroraDrop: copied from XY16( uint16_t x, uint16_t y)
  uint16_t XYCH( uint16_t x, uint16_t y) 
  {
    // ensure given half scaled matrix positions are within limits
    if( x >= MATRIX_WIDTH / 2) return 0;
    if( y >= MATRIX_HEIGHT / 2) return 0;
    return (y * (MATRIX_WIDTH / 2)) + x;
    //return (y * (MATRIX_WIDTH_TOTAL / 2)) + x + 1; // everything offset by one to capute out of bounds stuff - never displayed by ShowFrame()
  }
  uint16_t XYCQ( uint16_t x, uint16_t y) 
  {
    // ensure given quarter scaled matrix positions are within limits
    if( x >= MATRIX_WIDTH / 4) return 0;
    if( y >= MATRIX_HEIGHT / 4) return 0;
    return (y * (MATRIX_WIDTH / 4)) + x;
    //return (y * (MATRIX_WIDTH_TOTAL / 2)) + x + 1; // everything offset by one to capute out of bounds stuff - never displayed by ShowFrame()
  }



  // AuroraDrop: modifed from ClearFrame()

  // 0=full canvas, 1/2=half widths, 3=quarter, empty/255 = clear all
  void ClearCanvas(uint8_t id = 255)
  {
    switch (id) {
      // 0=full canvas, 1/2=half widths, 3=quarter, empty/255 = clear all
      //case 0:
      //  memset(canvasF, 0x00, NUM_LEDS * sizeof(CRGB)); // flush
      //  break;
      case 1:
        memset(canvasH, 0x00, NUM_LEDS * sizeof(CRGB) / 4);
        break;
      case 2:
        memset(canvasH2, 0x00, NUM_LEDS * sizeof(CRGB) / 4);
        break;
      case 3:
      //  memset(canvasQ, 0x00, NUM_LEDS * sizeof(CRGB) / 16);
      //  break;
      case 255:
      //  memset(canvasF, 0x00, NUM_LEDS * sizeof(CRGB));
        memset(canvasH, 0x00, NUM_LEDS * sizeof(CRGB) / 4);
        memset(canvasH2, 0x00, NUM_LEDS * sizeof(CRGB) / 4);
        memset(canvasQ, 0x00, NUM_LEDS * sizeof(CRGB) / 16);
        break;
      default:
        Serial.println("No Canvas?");
    }
  }


 void BresLine(int x0, int y0, int x1, int y1, byte colorIndex, uint8_t brightness, TBlendType blendType = LINEARBLEND)
  {
    CRGB color = ColorFromCurrentPalette(colorIndex, brightness);
    BresLine(x0, y0, x1, y1, color, blendType);
  }

  void BresLine(int x0, int y0, int x1, int y1, CRGB color, TBlendType blendType = LINEARBLEND)
  {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    for (;;) {
      if (blendType == LINEARBLEND) {
        leds[XY16(x0, y0)] += color;
      }
      else
      {
        leds[XY16(x0, y0)] = color;
      }
      if (x0 == x1 && y0 == y1) break;
      e2 = 2 * err;
      if (e2 > dy) {
        err += dy;
        x0 += sx;
      }
      if (e2 < dx) {
        err += dx;
        y0 += sy;
      }
    }
  }


  void BresLineCanvasH(CRGB *canvas, int x0, int y0, int x1, int y1, byte colorIndex, uint8_t brightness)
  {
    BresLineCanvasH(canvas, x0, y0, x1, y1, ColorFromCurrentPalette(colorIndex, brightness));
  }

  // AuroraDrop: draw line on canvas
  void BresLineCanvasH(CRGB *canvas, int x0, int y0, int x1, int y1, CRGB color)
  {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    for (;;) {
      canvas[XYCH(x0, y0)] += color;
      if (x0 == x1 && y0 == y1) break;
      e2 = 2 * err;
      if (e2 > dy) {
        err += dy;
        x0 += sx;
      }
      if (e2 < dx) {
        err += dx;
        y0 += sy;
      }
    }
  }

  // AuroraDrop: apply the canvas to the frame/screen
  void ApplyCanvasH(CRGB *canvas, int16_t x_offset, int16_t y_offset, float scale = 1.0, uint8_t blur = 0) {
    // use integer maths if we're not scaling, allow signed x/y for better scaling up options
    if (scale == 0.0 || scale == 1.0) {
      for (int x=0; x < MATRIX_WIDTH / 2; x++) {
        for (int y=0; y < MATRIX_HEIGHT / 2; y++) {
          leds[XY16(x + x_offset, y + y_offset)] += canvas[XYCH(x, y)];
        }
      }
    }
    else {
      for (int x=0; x < MATRIX_WIDTH / 2; x++) {
        for (int y=0; y < MATRIX_HEIGHT / 2; y++) {
          //if ((x * scale) + x_offset < MATRIX_WIDTH_TOTAL / 2 && (y * scale) + y_offset < MATRIX_HEIGHT / 2)
          
          // bug here? without this line pixel is always set to white, is clear function working properly?
          //if (x == 31 && x == 31) canvas[CANVAS_HALF(x, y)] = 0;

          leds[XY16((x * scale) + x_offset, (y * scale) + y_offset)] += canvas[XYCH(x, y)];
        }
      }

    }
    // 2d blur if we are scaling up
    if (blur > 0) {

      blur2d(leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, blur);   //  255=heavy blurring

      // effects.blur2d(canvas)
    }
  }

  // AuroraDrop: apply the canvas to the frame/screen
  void ApplyCanvasQ(CRGB *canvas, int16_t x_offset, int16_t y_offset, float scale = 1.0, uint8_t blur = 0) {
    // use integer maths if we're not scaling, allow signed x/y for better scaling up options
    if (scale == 0.0 || scale == 1.0) {
      for (int x=0; x < MATRIX_WIDTH / 4; x++) {
        for (int y=0; y < MATRIX_HEIGHT / 4; y++) {
          leds[XY16(x + x_offset, y + y_offset)] += canvas[XYCQ(x, y)];
        }
      }
    }
    else {
      for (int x=0; x < MATRIX_WIDTH / 4; x++) {
        for (int y=0; y < MATRIX_HEIGHT / 4; y++) {
          //if ((x * scale) + x_offset < MATRIX_WIDTH_TOTAL / 2 && (y * scale) + y_offset < MATRIX_HEIGHT / 2)
          
          // bug here? without this line pixel is always set to white, is clear function working properly?
          //if (x == 31 && x == 31) canvas[CANVAS_HALF(x, y)] = 0;

          leds[XY16((x * scale) + x_offset, (y * scale) + y_offset)] += canvas[XYCH(x, y)];
        }
      }

    }
    // 2d blur if we are scaling up
    if (blur > 0) {

      blur2d(leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, blur);   //  255=heavy blurring

      // effects.blur2d(canvas)
    }
  }


  void ApplyCanvasHMirror(CRGB *canvas, int16_t x_offset, int16_t y_offset, float scale = 1.0, uint8_t blur = 0) {
    // use integer maths if we're not scaling, allow signed x/y for better scaling up options
    if (scale == 0.0 || scale == 1.0) {
      for (int x=0; x < MATRIX_WIDTH / 2; x++) {
        for (int y=0; y < MATRIX_HEIGHT / 2; y++) {
          leds[XY16(((MATRIX_WIDTH/2) - x) + x_offset, y + y_offset)] += canvas[XYCH(x, y)];
        }
      }
    }
    else {
      for (int x=0; x < MATRIX_WIDTH / 2; x++) {
        for (int y=0; y < MATRIX_HEIGHT / 2; y++) {
          //if ((x * scale) + x_offset < MATRIX_WIDTH_TOTAL / 2 && (y * scale) + y_offset < MATRIX_HEIGHT / 2)
          
          // bug here? without this line pixel is always set to white, is clear function working properly?
          //if (x == 31 && x == 31) canvas[CANVAS_HALF(x, y)] = 0;

          leds[XY16((((MATRIX_WIDTH/2) - x) * scale) + x_offset, (y * scale) + y_offset)] += canvas[XYCH(x, y)];
        }
      }

    }
    // 2d blur if we are scaling up
    if (blur > 0) {

      blur2d(leds, MATRIX_WIDTH > 255 ? 255 : MATRIX_WIDTH, MATRIX_HEIGHT > 255 ? 255 : MATRIX_HEIGHT, blur);   //  255=heavy blurring

      // effects.blur2d(canvas)
    }
  }



  // rotates the bottom right 16x16 quadrant 3 times onto a 32x32 (+90 degrees rotation for each one)
  void Caleidoscope1_BottomRight() {
    for (int x = MATRIX_CENTER_X; x < MATRIX_WIDTH; x++) {
      for (int y = MATRIX_CENTER_Y; y < MATRIX_HEIGHT; y++) {
        leds[XY16(x - MATRIX_CENTER_X, y - MATRIX_CENTER_Y)] = leds[XY16(x, y)];
        leds[XY16(x - MATRIX_CENTER_X, y)] = leds[XY16(x, y)];
        leds[XY16(x, y - MATRIX_CENTER_Y)] = leds[XY16(x, y)];
      }
    }
  }


  // AuroraDrop: same but try to do middle too
  void Caleidoscope1Centre() {
    // copy original

    for (int x = 0; x < MATRIX_CENTER_X; x++) {
      for (int y = 0; y < MATRIX_CENTER_Y; y++) {
        leds[XY16(MATRIX_WIDTH - 1 - x, y)] = leds[XY16(x, y)];
        leds[XY16(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y)] = leds[XY16(x, y)];
        leds[XY16(x, MATRIX_HEIGHT - 1 - y)] = leds[XY16(x, y)];
      }
    }
    int x2,y2;
    // copy centre in reverse tp prevent overwrire
    for (int x = MATRIX_CENTER_X - 1; x >= 0 ; x--) {
      for (int y = MATRIX_CENTER_Y - 1; y >= 0; y--) {
        //x2 = MATRIX_CENTER_X - 1 - x;
        //y2 = MATRIX_CENTER_Y - 1 - y;
        leds[XY16(x + (MATRIX_WIDTH / 4), y + (MATRIX_HEIGHT / 4))] += leds[XY16(x, y)];
      }
    }
  }


  void CaleidoscopeA1() {
    // copy centre left quarter around

    // 1. copy bottom half to above areas
    for (int x = 0; x < MATRIX_CENTER_X; x++) {
      for (int y = MATRIX_CENTER_Y; y < MATRIX_HEIGHT - (MATRIX_HEIGHT / 4); y++) {
        leds[XY16(x, y - MATRIX_CENTER_Y)] = leds[XY16(x, y)];
        leds[XY16(x + MATRIX_CENTER_X, y - MATRIX_CENTER_Y)] = leds[XY16(x, y)];
      }
    }
    // b. copy top half to below areas
    for (int x = 0; x < MATRIX_CENTER_X; x++) {
      for (int y = MATRIX_CENTER_Y - (MATRIX_HEIGHT / 4); y < MATRIX_CENTER_Y; y++) {
        leds[XY16(x, y + MATRIX_CENTER_Y)] = leds[XY16(x, y)];
        leds[XY16(x + MATRIX_CENTER_X, y + MATRIX_CENTER_Y)] = leds[XY16(x, y)];
      }
    }

    // iii. copy whole area to the right
    for (int x = 0; x < MATRIX_CENTER_X; x++) {
      for (int y = MATRIX_CENTER_Y - (MATRIX_HEIGHT / 4); y < MATRIX_HEIGHT - (MATRIX_HEIGHT / 4); y++) {
        leds[XY16(x + MATRIX_CENTER_X, y)] = leds[XY16(x, y)];
      }
    }

  }


  void CaleidoscopeA2() {
    // simlilar but mirrors parts on right

    // 1. copy bottom half to above areas
    for (int x = 0; x < MATRIX_CENTER_X; x++) {
      for (int y = MATRIX_CENTER_Y; y < MATRIX_HEIGHT - (MATRIX_HEIGHT / 4); y++) {
        leds[XY16(x, y - MATRIX_CENTER_Y)] = leds[XY16(x, y)];
        leds[XY16(x + MATRIX_CENTER_X, y - MATRIX_CENTER_Y)] = leds[XY16(MATRIX_CENTER_X - x, y)];
      }
    }
    // b. copy top half to below areas
    for (int x = 0; x < MATRIX_CENTER_X; x++) {
      for (int y = MATRIX_CENTER_Y - (MATRIX_HEIGHT / 4); y < MATRIX_CENTER_Y; y++) {
        leds[XY16(x, y + MATRIX_CENTER_Y)] = leds[XY16(x, y)];
        leds[XY16(x + MATRIX_CENTER_X, y + MATRIX_CENTER_Y)] = leds[XY16(MATRIX_CENTER_X - x, y)];
      }
    }

    // iii. copy whole area to the right
    for (int x = 0; x < MATRIX_CENTER_X; x++) {
      for (int y = MATRIX_CENTER_Y - (MATRIX_HEIGHT / 4); y < MATRIX_HEIGHT - (MATRIX_HEIGHT / 4); y++) {
        leds[XY16(x + MATRIX_CENTER_X, y)] = leds[XY16(MATRIX_CENTER_X - x, y)];
      }
    }


  }

  void CaleidoscopeB1() {
    // copy the quarters of the centre to corners
    uint8_t matrixQW = MATRIX_WIDTH / 4;
    uint8_t matrixQH = MATRIX_HEIGHT / 4;

    // 1. copy top left quadrant to top left corner
    for (int x = matrixQW; x < MATRIX_CENTER_X; x++) {
      for (int y = MATRIX_HEIGHT / 4; y < MATRIX_CENTER_Y; y++) {
        leds[XY16(x - matrixQW, y - matrixQH)] = leds[XY16(x, y)];
      }
    }
    // 2. copy top right quadrant to top right corner
    for (int x = MATRIX_CENTER_X; x < MATRIX_WIDTH - matrixQW; x++) {
      for (int y = MATRIX_HEIGHT / 4; y < MATRIX_CENTER_Y; y++) {
        leds[XY16(x + matrixQW, y - matrixQH)] = leds[XY16(x, y)];
      }
    }
    // 3. copy bottom left quadrant to bottom left corner
    for (int x = matrixQW; x < MATRIX_CENTER_X; x++) {
      for (int y = MATRIX_CENTER_Y; y < MATRIX_HEIGHT - matrixQH; y++) {
        leds[XY16(x - matrixQW, y + matrixQH)] = leds[XY16(x, y)];
      }
    }
    // 4. copy bottom right quadrant to bottom right corner
    for (int x = MATRIX_CENTER_X; x < MATRIX_WIDTH - matrixQW; x++) {
      for (int y = MATRIX_CENTER_Y; y < MATRIX_HEIGHT - matrixQH; y++) {
        leds[XY16(x + matrixQW, y + matrixQH)] = leds[XY16(x, y)];
      }
    }

  }

  void CaleidoscopeB2() {
    // copy the halves of the centre to corners
    uint8_t matrixQW = MATRIX_WIDTH / 4;
    uint8_t matrixQH = MATRIX_HEIGHT / 4;

    // 1. copy left half to top left corner
    for (int x = matrixQW; x < MATRIX_CENTER_X; x++) {
      for (int y = MATRIX_HEIGHT / 4; y < MATRIX_HEIGHT - matrixQH; y++) {
        leds[XY16(x - matrixQW, y - matrixQH)] = leds[XY16(x, y)];
      }
    }
    // 2. copy right half to top right corner
    for (int x = MATRIX_CENTER_X; x < MATRIX_WIDTH - matrixQW; x++) {
      for (int y = MATRIX_HEIGHT / 4; y < MATRIX_HEIGHT - matrixQH; y++) {
        leds[XY16(x + matrixQW, y - matrixQH)] = leds[XY16(x, y)];
      }
    }
    // 3. copy left half to bottom left corner
    for (int x = matrixQW; x < MATRIX_CENTER_X; x++) {
      for (int y = MATRIX_HEIGHT / 4; y < MATRIX_HEIGHT - matrixQH; y++) {
        leds[XY16(x - matrixQW, y + matrixQH)] = leds[XY16(x, y)];
      }
    }
    // 4. copy right half to bottom right corner
    for (int x = MATRIX_CENTER_X; x < MATRIX_WIDTH - matrixQW; x++) {
      for (int y = MATRIX_HEIGHT / 4; y < MATRIX_HEIGHT - matrixQH; y++) {
        leds[XY16(x + matrixQW, y + matrixQH)] = leds[XY16(x, y)];
      }
    }

  }



  void RandomCaleidoscope(uint8_t id = 0) 
  {
    if (option10DisableCaleidoEffects) 
      return;

    //CaleidoscopeB2();
    //return;


    //Serial.print("Caleidoscope Pattern: ");
    //Serial.print(id);
    //Serial.print("\n");

    uint8_t randId = id;
    if (id==0) randId = random8(1,CALEIDOSCOPE_COUNT + 1);

    switch (randId) 
    {
      case 1:
        Caleidoscope1();
        break;
      case 2:
        Caleidoscope2();
        break;
      case 3:
        CaleidoscopeA1();
        break;
      case 4:
        CaleidoscopeA2();
        break;
      case 5:
        CaleidoscopeB1();
        break;
      case 6:
        CaleidoscopeB2();
        break;
      case 7:
        // rework Caleidoscope4 to mirror bottom right!
        Caleidoscope4Rework();
        Caleidoscope1();
        break;
      case 8:
        // rework Caleidoscope4 to mirror bottom right!
        Caleidoscope4Rework();
        Caleidoscope2();
        break;
      case 9:
        Caleidoscope4();
        Caleidoscope1();
        break;
      default:
        Caleidoscope1();
        break;
    }

  }

  // copy one diagonal triangle into the other one within a 16x16 (90 degrees rotated compared to Caleidoscope3)
  void Caleidoscope4Rework() {
    for (int x = 0; x <= MATRIX_CENTER_X; x++) {
      for (int y = 0; y <= MATRIX_CENTER_Y - x; y++) {
        leds[XY16(x, y)] = leds[XY16(MATRIX_CENTER_Y - y, MATRIX_CENTER_X - x)];
      }
    }
  }


  Point RotatePoint(float cx,float cy,float angle,Point p)
  {
    float s = sin(angle);
    float c = cos(angle);

    // translate point back to origin:
    p.x -= cx;
    p.y -= cy;

    // rotate point
    float xnew = p.x * c - p.y * s;
    float ynew = p.x * s + p.y * c;

    // translate point back:
    p.x = xnew + cx;
    p.y = ynew + cy;
    return p;
  }


  void ShowLianLi120() 
  {
    static uint8_t hue;

    const CRGBPalette16 newPalette_p = CRGBPalette16(CRGB::Cyan, CRGB::Yellow, CRGB::Magenta);

    uint8_t signHeight;
    uint8_t signCenter;

    //CRGB color  = 0xF800 ;
    CRGB color  = 0xEF0000;
    CRGB colorBlack  = 0x0;
    CRGB colorCenter  = 0x0A0A0A;

    signHeight = MATRIX_HEIGHT;
    if (fftData.noAudio) signHeight = signHeight - 7;
    signCenter = signHeight / 2;

    int ver = 1;
    int x0,yy0;
    switch (ver) {
      case 1:

        for (uint8_t i=0; i<signHeight; i++) {
          //color = ColorFromCurrentPalette(i*4,233,NOBLEND);
          color = ColorFromPalette(newPalette_p, i*4 + hue, 255, LINEARBLEND);

          if (i < 11) {
            x0 = 14 - i;
            BresLine(x0, i, x0 + 1, i, color, NOBLEND);
            x0 = 48 + i;
            BresLine(x0, i, x0 + 1, i, color, NOBLEND);
          }
          else
          {
            if (i > signHeight - 11) {
              x0 = (i - (signHeight - 11) + 3);
              BresLine(x0, i, x0 + 1, i, color, NOBLEND);
              x0 = 59 - (i - (signHeight - 11));
              BresLine(x0, i, x0 + 1, i, color, NOBLEND);
            }
            else
            {
              BresLine(3, i, 5, i, color, NOBLEND);
              BresLine(58, i, 60, i, color, NOBLEND);
            }
          }

        }
        // change color slowly
        hue++;

        break;
      case 0:
        // original method
    //left
    BresLine(14, 0, 3, 11, color, NOBLEND);
    BresLine(15, 0, 4, 11, color, NOBLEND);

    BresLine(3, 11, 3, signHeight - 11, color, NOBLEND);
    BresLine(4, 11, 4, signHeight - 11, color, NOBLEND);
    BresLine(5, 10, 5, signHeight - 10, color, NOBLEND);

    BresLine(3, signHeight - 11, 14, signHeight, color, NOBLEND);
    BresLine(4, signHeight - 11, 15, signHeight, color, NOBLEND);

    //right
    BresLine(49, 0, 60, 11, color, NOBLEND);
    BresLine(48, 0, 59, 11, color, NOBLEND);

    BresLine(60, 11, 60, signHeight - 11, color, NOBLEND);
    BresLine(59, 11, 59, signHeight - 11, color, NOBLEND);
    BresLine(58, 10, 58, signHeight - 10, color, NOBLEND);

    BresLine(60, signHeight - 11, 49, signHeight, color, NOBLEND);
    BresLine(59, signHeight - 11, 48, signHeight, color, NOBLEND);

    // centre circle
    BresLine(29, signCenter - 6, 34, signCenter - 6, colorCenter, LINEARBLEND);
    BresLine(27, signCenter - 5, 36, signCenter - 5, colorCenter, LINEARBLEND);
    BresLine(26, signCenter - 4, 37, signCenter - 4, colorCenter, LINEARBLEND);
    BresLine(26, signCenter - 3, 37, signCenter - 3, colorCenter, LINEARBLEND);
    BresLine(25, signCenter - 2, 38, signCenter - 2, colorCenter, LINEARBLEND);
    BresLine(25, signCenter - 1, 38, signCenter - 1, colorCenter, LINEARBLEND);
    BresLine(25, signCenter , 38, signCenter, colorCenter, LINEARBLEND);
    BresLine(25, signCenter + 1, 38, signCenter + 1, colorCenter, LINEARBLEND);
    BresLine(25, signCenter + 2, 38, signCenter + 2, colorCenter, LINEARBLEND);
    BresLine(25, signCenter + 3, 38, signCenter + 3, colorCenter, LINEARBLEND);
    BresLine(26, signCenter + 4, 37, signCenter + 4, colorCenter, LINEARBLEND);
    BresLine(26, signCenter + 5, 37, signCenter + 5, colorCenter, LINEARBLEND);
    BresLine(27, signCenter + 6, 36, signCenter + 6, colorCenter, LINEARBLEND);
    BresLine(29, signCenter + 7, 34, signCenter + 7, colorCenter, LINEARBLEND);

    // draw black lines at bottom if silent
    if (signHeight != MATRIX_HEIGHT) {
      BresLine(0, 63, 63, 63, colorBlack, NOBLEND);
      BresLine(0, 62, 63, 62, colorBlack, NOBLEND);
      BresLine(0, 61, 63, 61, colorBlack, NOBLEND);
      BresLine(0, 60, 63, 60, colorBlack, NOBLEND);
      BresLine(0, 59, 63, 59, colorBlack, NOBLEND);
      BresLine(0, 58, 63, 58, colorBlack, NOBLEND);

    }

        break;
    }




  }







  void GenerateBitmap()
  {

    unsigned char *img = NULL;
    int r,g,b;
    int x,y;
    int w = MATRIX_WIDTH ;
    int h = MATRIX_HEIGHT;
    int filesize = 54 + 3 * w * h;

    img = (unsigned char *)malloc(3*w*h);
    memset(img,0,3*w*h);

    // encode data from panel to img
    for(int i=0; i<w; i++)
    {
      for(int j=0; j<h; j++)
      {
          x=i; y=(h-1)-j;
          r = leds[XY16(i, j)].r;
          g = leds[XY16(i, j)].g;
          b = leds[XY16(i, j)].b;
          img[(x+y*w)*3+2] = (unsigned char)(r);
          img[(x+y*w)*3+1] = (unsigned char)(g);
          img[(x+y*w)*3+0] = (unsigned char)(b);
      }
    }

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmppad[3] = {0,0,0};

    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);

    bmpinfoheader[ 4] = (unsigned char)(       w    );
    bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
    bmpinfoheader[ 6] = (unsigned char)(       w>>16);
    bmpinfoheader[ 7] = (unsigned char)(       w>>24);
    bmpinfoheader[ 8] = (unsigned char)(       h    );
    bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
    bmpinfoheader[10] = (unsigned char)(       h>>16);
    bmpinfoheader[11] = (unsigned char)(       h>>24);

    File file = SPIFFS.open("/test.bmp", FILE_WRITE);
    if (!file)
    {
      Serial.println("There was an error opening the file for writing");
      return;
    }
    file.write(bmpfileheader,14);
    file.write(bmpinfoheader,40);
    for(int i=0; i<h; i++)
    {
      file.write(img+(w*(h-i-1)*3),w*3);
      file.write(bmppad,(4-(w*3)%4)%4);
    }
    file.close();
    free(img);
  } 
  
};

#endif