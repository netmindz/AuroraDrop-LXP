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

#ifndef Drawable_H
#define Drawable_H

class Drawable {

    public:

    char* name;

    // modified by AuroraDrop
    //
    //uint8_t randomWeight = 10;            // for helping weighted random selection (ha) of better patterns into playlists, 0=low, 10=high
    unsigned int default_fps = 90;
    unsigned long ms_animation_max_duration = 10000;    // 10 default seconds
    unsigned long fps = 0;   // running fps (this is NOT a matix refresh rate!)
    unsigned long fps_last = 0;   // fps
    unsigned long  fps_timer = 0; 
    unsigned int  pattern_fps = 90;
    unsigned long last_frame = 0;
    unsigned long  ms_previous = 0;
    unsigned long render_ms;

    char* id;
    uint8_t id2;
    bool enabled = false;

    virtual bool isEnabled() {

        return enabled;

    }

    virtual void setEnabled(bool _value) {
        
        enabled = _value;

    }


    virtual bool isRunnable() {

        return false;

    }
    
    virtual bool isPlaylist() {

        return false;

    }

    // a single frame should be drawn as fast as possible, without any delay or blocking
    // return how many millisecond delay is requested before the next call to drawFrame()
    //
    virtual unsigned int drawFrame(uint8_t _instance, uint8_t _total) {

		//dma_display->fillScreen(0);   // removed to support LEDSTRIP

        //backgroundLayer.fillScreen({ 0, 0, 0 });

        return 0;

    };

    virtual void printTesting() {

        Serial.println("Testing...");

    }

    virtual void start(uint8_t _pattern) {};

    virtual void stop() {};
    
};

#endif
