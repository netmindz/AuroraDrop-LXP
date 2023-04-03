#ifndef WLED_LISTEN_H
#define WLED_LISTEN_H

#include "WLED-sync.h"

WLEDSync sync;

static uint8_t fftResult[NUM_GEQ_CHANNELS]= {0};
static float multAgc = 1.0f;
static TaskHandle_t FFT_Task = nullptr;

int animation_duration = 60000/120*16;

void setupAudio() {
  sync.begin();
  Serial.println("Listening for WLED audio sync data");

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
void FFTcode( void * pvParameters) {
  for(;;) {
    if (sync.read()) {
      for (int b = 0; b < NUM_GEQ_CHANNELS; b++) {
        uint8_t val = sync.fftResult[b];
      }
    }   
  }
}
#endif WLED_LISTEN_H