// Color definitions
//
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF

uint32_t actual_render_ms = 0;
uint32_t start_render_ms = millis();
uint32_t total_render_ms = 0;
uint32_t actual_fps = 0;

void UpdateDiagnosticsData() {

    #ifdef ONBOARD_RGB_LED

        if (actual_render_ms > 1000/20) {
            
            pixels.setPixelColor(0,255,0,0);
            pixels.show();

        } else if (actual_render_ms > 1000/30) {
            
            pixels.setPixelColor(0,255,191,0);
            pixels.show();

        } else {

            pixels.setPixelColor(0,0,255,0);
            pixels.show();

        }

    #endif

    // ------------------------ INFO --------------------------

    // ---------------- show total render time ----------------

    // show actual render time if over limit, 50 = 20fps
    //
    if (option3ShowRenderTime || option1Diagnostics) {

        dma_display->setTextSize(1);

        // display total render time
        //
        dma_display->setTextColor(WHITE);
        
        if (actual_render_ms > 50) {
            
            dma_display->setTextColor(YELLOW);

        }

        dma_display->setCursor(40,55);
        dma_display->print(actual_render_ms);
        dma_display->print("ms");
        
    }

    // if WiFi has just connected, show IP address and version notice
    //
    #ifdef USE_WIFI 

        if (wifiMessage > 0) {

            wifiMessage--;

            dma_display->setTextSize(1);
            dma_display->setTextColor(WHITE, BLUE);
            dma_display->setCursor(0,8);
            dma_display->print(WiFi.localIP());

        }

    #endif

    if (option5ShowEffectsStack) {

        int startx = 2;
        int starty = 2;
        int yinc = 8;

        dma_display->setTextSize(1);
        dma_display->setTextColor(WHITE);

        for (uint8_t i=0; i < CountPlaylistsBackground; i++) {

            dma_display->setCursor(startx,starty);
            dma_display->print(playlistBackground[i].getCurrentPatternName());

            starty += yinc;

        }

        for (uint8_t i=0; i < CountPlaylistsAudio; i++) {

            dma_display->setCursor(startx,starty);
            dma_display->print(playlistAudio[i].getCurrentPatternName());

            starty += yinc;
            
        }

        for (uint8_t i=0; i < CountPlaylistsStatic; i++) {

            dma_display->setCursor(startx,starty);
            dma_display->print(playlistStatic[i].getCurrentPatternName());

            starty += yinc;
            
        }

        for (uint8_t i=0; i < CountPlaylistsForeground; i++) {

            dma_display->setCursor(startx,starty);
            dma_display->print(playlistForeground[i].getCurrentPatternName());

            starty += yinc;
            
        }

    }
    
    // ---------------------- DIAGNOSTICS -------------------------

    if (option1Diagnostics) {

        for (uint8_t i=0; i < 16; i++) {

            uint8_t height = map8(fftResult[i],0,32);
            
            dma_display->drawFastVLine(0+(i*4)+0, 0, height, BLUE);
            dma_display->drawFastVLine(0+(i*4)+1, 0, height, BLUE);
            dma_display->drawFastVLine(0+(i*4)+2, 0, height, BLUE);
            dma_display->drawFastVLine(0+(i*4)+3, 0, height, BLUE);

        }

        for (uint8_t i=0; i < MATRIX_WIDTH; i++) {

            uint8_t height = map8(fftData.specData[i],0,32);
            
            dma_display->drawFastVLine(i, MATRIX_HEIGHT-32, height, GREEN);

        }

        for (uint8_t i=0; i < 16; i++) {

            uint8_t height = map8(fftData.specData16[i],0,32);
            
            dma_display->drawFastVLine(64+(i*4)+0, 0, height, GREEN);
            dma_display->drawFastVLine(64+(i*4)+1, 0, height, GREEN);
            dma_display->drawFastVLine(64+(i*4)+2, 0, height, GREEN);
            dma_display->drawFastVLine(64+(i*4)+3, 0, height, GREEN);

        }

        dma_display->setTextSize(1);
        dma_display->setTextColor(WHITE);
        dma_display->setCursor(40,46);
        dma_display->print(total_render_ms);
        dma_display->print("ms");

        // actual fps
        dma_display->setCursor(2,46);
        dma_display->print("G ");
        dma_display->print(multAgc);

        dma_display->setCursor(2,55);
        dma_display->print(fftData.bpm);
        dma_display->print("bpm");

        for (uint8_t i=0; i < CountPlaylistsBackground; i++) {

            dma_display->setTextColor(WHITE);
            dma_display->setCursor(2+(i*20),1);
            dma_display->print(playlistBackground[i].getCurrentPatternId());

            if (playlistBackground[i].render_ms > 10) {
                
                dma_display->setTextColor(YELLOW);

            }

            if (playlistBackground[i].render_ms > 15) {
                
                dma_display->setTextColor(MAGENTA);

            }

            dma_display->print(playlistBackground[i].render_ms);

        }

        for (uint8_t i=0; i < CountPlaylistsAudio; i++) {
            
            dma_display->setTextColor(WHITE);
            dma_display->setCursor(2+(i*20),1+8);
            dma_display->print(playlistAudio[i].getCurrentPatternId());      

            if (playlistAudio[i].render_ms > 10) {
                
                dma_display->setTextColor(YELLOW);

            }
        
            if (playlistAudio[i].render_ms > 15) {
                
                dma_display->setTextColor(MAGENTA);

            }

            dma_display->print(playlistAudio[i].render_ms);

        }

        for (uint8_t i=0; i < CountPlaylistsStatic; i++) {

            dma_display->setTextColor(WHITE);
            dma_display->setCursor(2+(i*20),1+8+8);
            dma_display->print(playlistStatic[i].getCurrentPatternId());      

            if (playlistStatic[i].render_ms > 10) {
                
                dma_display->setTextColor(YELLOW);
            }
            
            if (playlistStatic[i].render_ms > 15) {
                
                dma_display->setTextColor(MAGENTA);
            }

            dma_display->print(playlistStatic[i].render_ms);

        }
        
        for (uint8_t i=0; i < CountPlaylistsForeground; i++) {

            dma_display->setTextColor(WHITE);
            dma_display->setCursor(2+(i*20),1+8+8+8);
            dma_display->print(playlistForeground[i].getCurrentPatternId());

            if (playlistForeground[i].render_ms > 10) {
                
                dma_display->setTextColor(YELLOW);

            }

            if (playlistForeground[i].render_ms > 15) {
                
                dma_display->setTextColor(MAGENTA);

            }

            dma_display->print(playlistForeground[i].render_ms);

        }

        for (uint8_t i=MATRIX_HEIGHT-10; i<MATRIX_HEIGHT-8; i++) {

            // saw wave
            //
            dma_display->drawPixelRGB888(effects.beatSawOsciWidth[0], i, 0, 0, 128);          // blue - fast
            dma_display->drawPixelRGB888(effects.beatSawOsciWidth[1], i, 0, 128, 0);          // green
            dma_display->drawPixelRGB888(effects.beatSawOsciWidth[2], i, 128, 0, 0);          // red
            dma_display->drawPixelRGB888(effects.beatSawOsciWidth[3], i, 128, 128, 128);      // white - slow
            
            // sine wave
            //
            dma_display->drawPixelRGB888(effects.beatSineOsciWidth[0], i-1, 0, 0, 128);
            dma_display->drawPixelRGB888(effects.beatSineOsciWidth[1], i-1, 0, 128, 0);
            dma_display->drawPixelRGB888(effects.beatSineOsciWidth[2], i-1, 128, 0, 0);
            dma_display->drawPixelRGB888(effects.beatSineOsciWidth[3], i-1, 128, 128, 128);   // white - slow
            
            // square wave
            //
            dma_display->drawPixelRGB888(effects.beatSquareOsci8[0] / 4, i, 128, 0, 128);
        
        }

        // BLINK BPM still testing bpm! (paint last so it's over top)
        //
        if (effects.beatSawOsci8[0] > 200) {

            dma_display->drawPixelRGB888(MATRIX_WIDTH - 4, 2, 255, 255, 255);
            dma_display->drawPixelRGB888(MATRIX_WIDTH - 3, 2, 255, 255, 255);
            dma_display->drawPixelRGB888(MATRIX_WIDTH - 4, 3, 255, 255, 255);
            dma_display->drawPixelRGB888(MATRIX_WIDTH - 3, 3, 255, 255, 255);
        
        }

        #ifdef DEDICATION

            #include "dedication.h"

        #endif

    }

}
