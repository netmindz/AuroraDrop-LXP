/*
 * This code has been taken from the sound reactive fork of WLED by Andrew
 * Tuline, and the analog audio processing has been (mostly) removed as we
 * will only be using the INMP441.
 * 
 * The FFT code runs on core 0 while everything else runs on core 1. This
 * means we can make our main code more complex without affecting the FFT
 * processing.
 */

#include <driver/i2s.h>
#include <arduinoFFT.h>

#define MIN_SHOW_DELAY 20

const i2s_port_t I2S_PORT = I2S_NUM_1;

const int BLOCK_SIZE = 64;

const int SAMPLE_RATE = 10240;

TaskHandle_t FFT_Task;

//float cutrate = 1.0; // 1 is divide by 1 = no cut
//float cutratemin = 1.1;
float avg_mag = 4096/16;
float pre_avg_mag = 4096/16;

int squelch = 2;    // was 0                       // Squelch, cuts out low level sounds
float gain = 0;      // was 30                       // Gain, boosts input level*/
float gain_max = 60; // after a certain point I think we're boosting room tone. :D

uint16_t micData;                               // Analog input for FFT

const uint16_t samples = 512;    // 512 seems OK for me but it seems code in FFT expects 256 samples for binning (or not?)

double FFT_MajorPeak = 0;
double FFT_Magnitude = 0;
uint16_t mAvg = 0;

// These are the input and output vectors.  Input vectors receive computed results from FFT.
//
double vReal[samples];
double vImag[samples];
double fftBin[samples];

// take copy of data prior to fft
//
//double vRealCopy[max_samples];

// Try and normalize fftBin values to a max of 4096, so that 4096/16 = 256.
// Oh, and bins 0,1,2 are no good, so we'll zero them out.
//
double fftCalc[16];
int fftResult[16];                      // Our calculated result table, which we feed to the animations.
double fftResultMax[16];                // A table used for testing to determine how our post-processing is working.

// Table of linearNoise results to be multiplied by squelch in order to reduce squelch across fftResult bins.
//
int linearNoise[16] = { 34, 28, 26, 25, 20, 12, 9, 6, 4, 4, 3, 2, 2, 2, 2, 2 };

// Table of multiplication factors so that we can even out the frequency response.
//
// double fftResultPink[16] = {1.00,1.00,1.73,1.78,1.68,1.56,1.55,1.63,1.79,1.62,1.80,2.06,2.47,3.35,6.83,9.55}; // Troy
// double fftResultPink[16] = {1.70,1.71,1.73,1.78,1.68,1.56,1.55,1.63,1.79,1.62,1.80,2.06,2.47,3.35,6.83,9.55}; // original
// double fftResultPink[16] = {2.01,2.31,2.53,2.42,1.87,1.00,1.51,2.33,3.46,1.45,1.61,1.69,2.19,2.40,2.72,2.71}; // sampled room tone on INMP441 mic, zero gain
double fftResultPink[16] = {2.15,2.48,2.74,2.64,2.08,1.00,1.64,2.41,4.22,1.61,2.05,1.86,2.36,2.69,3.17,3.12}; // later when things were quieter.
// double fftResultPink[16] = {1.00,1.04,1.08,1.20,1.22,1.27,1.31,1.20,1.07,1.51,1.11,1.64,1.28,1.39,1.29,1.53};

int specDataMinVolume = 128;
int specDataMaxVolume = 0;
int totalVolume = 0;
float mean_int = 425.0; // just below 100 bpm in ms

// Create FFT object
//
arduinoFFT FFT = arduinoFFT(vReal, vImag, samples, SAMPLE_RATE);

// BPM Test Stuff:
//
// Define this to use reciprocal multiplication for division and some more speedups that might decrease precision
// #define FFT_SPEED_OVER_PRECISION
float mag = 128;
float magAvg = 64;
int avgSampleCount = 1;
float lastBeat = 0;  // time of last beat in millis()
float bpm_interval = 480; // 125 BPM = 480ms
float bpm_interval_avg = bpm_interval_avg;
float accumulated_frame_error = 0;
float magThreshold = 1.5;
float magThresholdAvg = magThreshold;
int animation_duration = 60000/120*16;
//
double beatTime = 60.0 / 140 * 1000;
//
// #include "MedianFilterLib2.h"
// MedianFilter2<float> medianFilter2(16);
// END BPM STUFF

double fftAvg( int from, int to) {

    int i = from;
    double result = 0;

    while ( i <= to) {

        result += fftBin[i++];

    }

    return result / (abs(to-from)+1);

}

double fftAvgReal( int from, int to) {

    int i = from;
    double result = 0;

    while ( i <= to) {

        result += vReal[i++];

    }

    return result / (abs(to-from)+1);

}

// FFT main code - goes into its own task on its own core
//
void FFTcode( void * pvParameters) {

    for(;;) {

        delay(1);   // DO NOT DELETE THIS LINE! It is needed to give the IDLE(0) task enough time and to keep the watchdog happy.
                    // taskYIELD(), yield(), vTaskDelay() and esp_task_wdt_feed() didn't seem to work.

        uint32_t audio_time = millis();
        
        size_t bytesRead = 0;
        int32_t buffer32[samples] = {0};
        
        i2s_read(I2S_PORT, &buffer32, sizeof(buffer32), &bytesRead, portMAX_DELAY);

        int samplesRead = bytesRead / 4;

        int32_t buffer32keep = buffer32[255];

        for (int i=0; i<samplesRead; i++) {

            // I've seen this done a bunch of ways. One is to pull a 32 bit sample
            // into four 8-bit slots and then "do something" to extract the useful 
            // data. But this seems to work for this purpose.
            //
            // post: "Inputting audio to an ESP32 from an INMP441 I2S microphone: success"
            // link: https://www.esp32.com/viewtopic.php?t=15185
            //
            // I question if this is "right" or just copied from somewhere:
            //

            micData = abs(buffer32[i] >> 16); 
            //
            // I've also seen it with 14 bits shifted... 

            // uint8_t buffer32[samples * 4];
            // ... read I2S ...
            // uint8_t mid = buffer32[i * 4 + 2];
            // uint8_t msb = buffer32[i * 4 + 3];
            // uint16_t micData = 0x8000 + (((uint32_t)msb) << 8) + ((uint32_t)mid);

            vReal[i] = micData;
            vImag[i] = 0.0;

        }

        FFT.DCRemoval(); // The INMP441 has an internal high-pass to eliminate low-end signals but this still seems to help
        FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);    // Weigh data
        FFT.Compute(FFT_FORWARD);                           // Compute FFT
        FFT.ComplexToMagnitude();                           // Compute magnitudes

        // vReal[3 .. 255] contain useful data, each a 20Hz interval (60Hz - 5120Hz).
        // There could be interesting data at bins 0 to 2, but there are too many artifacts.
        //
        FFT.MajorPeak(&FFT_MajorPeak, &FFT_Magnitude);          // let the effects know which freq was most dominant

        for (int i = 0; i < samples; i++) {

            double t = 0.0;
            t = abs(vReal[i]);
            // t = t / cutrate;      // Reduce magnitude. Want end result to be linear and ~4096 max. I'm torn if this is the right way to do things.
            fftBin[i] = t;

        }

        /* This FFT post processing is a DIY endeavour. What we really need is someone with sound engineering expertise to do a great job here AND most importantly, that the animations look GREAT as a result.
        *
        * Andrew's updated mapping of 256 bins down to the 16 result bins with Sample Freq = 10240, samples = 512 and some overlap.
        * Based on testing, the lowest/Start frequency is 60 Hz (with bin 3) and a highest/End frequency of 5120 Hz in bin 255.
        * Now, Take the 60Hz and multiply by 1.320367784 to get the next frequency and so on until the end. Then detetermine the bins.
        * End frequency = Start frequency * multiplier ^ 16
        * Multiplier = (End frequency/ Start frequency) ^ 1/16
        * Multiplier = 1.320367784
        */

        //                     // Range
        // fftCalc[0] = (fftAdd(3,4)) /2;        // 60 - 100
        // fftCalc[1] = (fftAdd(4,5)) /2;        // 80 - 120
        // fftCalc[2] = (fftAdd(5,7)) /3;        // 100 - 160
        // fftCalc[3] = (fftAdd(7,9)) /3;        // 140 - 200
        // fftCalc[4] = (fftAdd(9,12)) /4;       // 180 - 260
        // fftCalc[5] = (fftAdd(12,16)) /5;      // 240 - 340
        // fftCalc[6] = (fftAdd(16,21)) /6;      // 320 - 440
        // fftCalc[7] = (fftAdd(21,28)) /8;      // 420 - 600
        // fftCalc[8] = (fftAdd(29,37)) /10;     // 580 - 760
        // fftCalc[9] = (fftAdd(37,48)) /12;     // 740 - 980
        // fftCalc[10] = (fftAdd(48,64)) /17;    // 960 - 1300
        // fftCalc[11] = (fftAdd(64,84)) /21;    // 1280 - 1700
        // fftCalc[12] = (fftAdd(84,111)) /28;   // 1680 - 2240
        // fftCalc[13] = (fftAdd(111,147)) /37;  // 2220 - 2960
        // fftCalc[14] = (fftAdd(147,194)) /48;  // 2940 - 3900
        // fftCalc[15] = (fftAdd(194, 255)) /62; // 3880 - 5120

        fftCalc[0] = fftAvg(3,4);       // 60 - 100
        fftCalc[1] = fftAvg(4,5);        // 80 - 120
        fftCalc[2] = fftAvg(5,7);        // 100 - 160
        fftCalc[3] = fftAvg(7,9);        // 140 - 200
        fftCalc[4] = fftAvg(9,12);       // 180 - 260
        fftCalc[5] = fftAvg(12,16);      // 240 - 340
        fftCalc[6] = fftAvg(16,21);      // 320 - 440
        fftCalc[7] = fftAvg(21,28);      // 420 - 600
        fftCalc[8] = fftAvg(29,37);     // 580 - 760
        fftCalc[9] = fftAvg(37,48);     // 740 - 980
        fftCalc[10] = fftAvg(48,64);    // 960 - 1300
        fftCalc[11] = fftAvg(64,84);    // 1280 - 1700
        fftCalc[12] = fftAvg(84,111);   // 1680 - 2240
        fftCalc[13] = fftAvg(111,147);  // 2220 - 2960
        fftCalc[14] = fftAvg(147,194);  // 2940 - 3900
        fftCalc[15] = fftAvg(194, 255); // 3880 - 5120

        // Noise supression of fftCalc bins using squelch adjustment for different input types.
        //
        // for (int i=0; i < 16; i++) {

        //     fftCalc[i] = fftCalc[i]-(float)squelch*(float)linearNoise[i]/4.0 <= 0? 0 : fftCalc[i];

        // }

        // Adjustment for frequency curves.
        //
        for (int i=0; i < 16; i++) {

            fftCalc[i] = fftCalc[i] * fftResultPink[i];

        }

        // Manual linear adjustment of gain using gain adjustment for different input types.
        //
        for (int i=0; i < 16; i++) {

            fftCalc[i] = fftCalc[i] * gain / 40 + fftCalc[i]/16.0;

        }

        // Now, let's dump it all into fftResult. Need to do this, otherwise other routines might grab fftResult values prematurely.
        //
        for (int i=0; i < 16; i++) {

            fftResult[i] = constrain((int)fftCalc[i],0,254);

        }

        specDataMinVolume = fftResult[0];
        int specDataMaxVolume = 0;
        totalVolume = 0;

        for (uint8_t i = 0; i < 16; i++) {

            totalVolume += fftResult[i];

            // if min/max volume breached then update
            //
            if (fftResult[i] > specDataMaxVolume) {
                
                specDataMaxVolume = fftResult[i];

            }

            if (fftResult[i] < specDataMinVolume) {
                
                specDataMinVolume = fftResult[i];

            }

        }

        if (specDataMaxVolume == 254) {

            if (gain > 0) gain -= 1;
            if (gain < 0) gain = 0;

        } else if (specDataMaxVolume < 128) {

            gain += 0.01;
            if (gain > gain_max) gain = gain_max;

        }

        for (int i=0;i<128;i++) {

            fftData.specData[i] = fftAvg(i*2,(i*2)+1);

        }

        for (int i=0;i<64;i++) {

            fftData.specData64[i] = fftAvg(i*4,(i*4)+1);

        }

        for (int i=0;i<32;i++) {

            fftData.specData32[i] = fftAvg(i*8,(i*8)+1);

        }

        for (int i=0;i<16;i++) {

            fftData.specData16[i] = fftAvg(i*16,(i*16)+1);

        }

        for (int i=0;i<8;i++) {

            fftData.specData8[i] = fftAvg(i*32,(i*32)+1);

        }

        // for (int i=0;i<8;i++) {

        //     fftData.specData8[i] = fftResult[i];

        // }

        // BPM inspiration: https://github.com/blaz-r/ESP32-music-beat-sync/blob/main/src/ESP32-music-beat-sync.cpp
        // It's not currently "great" but it figures it out within a two BPM window.
        //
        magAvg = magAvg * 0.99 + fftResult[0] * 0.01;

        if (millis()-lastBeat > beatTime && fftResult[0]/magAvg > magThreshold) {
            
            bpm_interval = millis() - lastBeat;

            lastBeat = millis();

            if (bpm_interval > 426 && bpm_interval < 600) { // between 100 and 140 BPM in ms

                mean_int = mean_int * 0.9 + bpm_interval * 0.1;

                fftData.bpm = 60*1000 / mean_int;

                animation_duration = 60000/fftData.bpm*16;

                magThresholdAvg = magThresholdAvg * 0.9 + (fftResult[0]/magAvg) * 0.1;

                // Serial.printf("Int:%f,mInt:%f,BPM:%d,FrameErr:%f\n", bpm_interval, mean_int, fftData.bpm, accumulated_frame_error/10);
                
                accumulated_frame_error = 0;

            }

        } else {

            // not currenntly used, but the idea is to suss out how much
            // 'extra' time renderig takes up to get a more accurate BPM
            // ...but this doesn't seem to really be directly correlated 
            // to BPM intervals.
            //
            accumulated_frame_error += millis()-audio_time;

        } // END BPM
        
        // testing
        // for (int i=0;i<16;i++) {
            //   fftData.specData[i] = fftResult[i];
            //   fftData.specData[i+16] = fftResult[i];
            //   fftData.specData[i+32] = fftResult[i];
            //   fftData.specData[i+48] = fftResult[i];
            //   fftData.specData[i+64] = fftResult[i];
            //   fftData.specData[i+80] = fftResult[i];
            //   fftData.specData[i+96] = fftResult[i];
            //   fftData.specData[i+112] = fftResult[i];

            //   fftData.specData64[i] = fftResult[i];
            //   fftData.specData64[i+16] = fftResult[i];
            //   fftData.specData64[i+32] = fftResult[i];
            //   fftData.specData64[i+48] = fftResult[i];

            //   fftData.specData32[i] = fftResult[i];
            //   fftData.specData32[i+16] = fftResult[i];

            //   fftData.specData16[i] = fftResult[i];
        // }

        fftData.noAudio = false;

    }

}

void setupNothing() {
  
  // noop

}

void setupAudio() {

    Serial.println("INMP441 Audio setup...");
    esp_err_t err;

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,      // SCK
        .ws_io_num = I2S_WS,        // WS
        .data_out_num = -1,         // not used (only for speakers)
        .data_in_num = I2S_SD       // SD .... and L/R in all my tests is to VCC (not ground!) for Left Output
    };

    Serial.println("Doing Driver...");
    delay(1000);

    // Configuring the I2S driver and pins.
    // This function must be called before any I2S driver read/write operations.
    //
    err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

    if (err != ESP_OK) {

        Serial.printf("Failed installing driver: %d\n", err);
        while (true);

    }

    Serial.println("Doing Pins...");
    delay(1000);

    err = i2s_set_pin(I2S_PORT, &pin_config);

    if (err != ESP_OK) {

        Serial.printf("Failed setting pin: %d\n", err);
        while (true);

    }

    Serial.println("Doing Clock...");
    delay(1000);

    err = i2s_set_clk(I2S_PORT, SAMPLE_RATE*2, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_MONO);

    if (err != ESP_OK) {

        Serial.printf("Failed setting clock: %d\n", err);
        while (true);

    }

    Serial.println("I2S driver installed.");

    delay(100);

    // Test to see if we have a digital microphone installed or not.

    float mean = 0.0;
    size_t bytesRead = 0;
    int32_t buffer32[samples] = {0};

    i2s_read(I2S_PORT, &buffer32, sizeof(buffer32), &bytesRead, portMAX_DELAY);

    int samplesRead = bytesRead / 4;

    if (samplesRead == samples) {

        Serial.println("Read the exprected "+String(samples)+" samples");

        for (int i=0; i<samplesRead; i++) {

            mean += abs(buffer32[i] >> 16); 

        }

        if (mean != 0.0) {

            Serial.println("...and they weren't all zeros! Working!");

        } else {

            Serial.println("...and they were all zeros.");
            Serial.println("Check the mic pin definitions and L/R pin and potentially switch L/R to VCC from GND or vice-versa.");

        }

    } else {

        Serial.println("No samples read!");

    }
    
    // Define the FFT Task and lock it to core 0
    //
    xTaskCreatePinnedToCore(
        FFTcode,                          // Function to implement the task
        "FFT",                            // Name of the task
        30000,                            // Stack size in words
        NULL,                             // Task input parameter
        1,                                // Priority of the task
        &FFT_Task,                        // Task handle
    0);                               // Core where the task should run

}