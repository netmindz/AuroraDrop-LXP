TaskHandle_t COM_Task;

uint32_t comMicros;

bool comClientConnected = false;
uint8_t comTps = 0;                // telegrams per second
uint32_t comTps_ms = millis();
uint32_t dataReceivedTime = millis();

byte buffer[512];
int bufferSize = 0;
int eofCount = 0;

// COM main code
void COMcode( void * parameter) {

  Serial.println("COMcode starting");

  for(;;) {
    delay(1);           // DO NOT DELETE THIS LINE! It is needed to give the IDLE(0) task enough time and to keep the watchdog happy.
                        // taskYIELD(), yield(), vTaskDelay() and esp_task_wdt_feed() didn't seem to work.
                        
    comMicros = micros();

    if (millis() > dataReceivedTime + 1000) fftData.noAudio = true;

    // data is 128 bytes long follwed by the 5 characters '<EOF>'

/*
    // wait until there is at least 133 characters in the serial buffer (128 + 5), if nothing after 1sec, throw an error
    uint32_t timeout = millis();
    comClientConnected = true;
    while (Serial.available() < BINS + 5) {
      if (millis() - timeout > 2000) {
        Serial.print("!!! COM client timeout with bytes available: ");
        Serial.println(Serial.available());
        comClientConnected = false;
        // clear out any old
        int dud = Serial.available();
        for (int i=0; i < dud; i++) {
          Serial.read();
        }
        //delay(1000);
      }
    }
*/

    comClientConnected = true;
    if (comClientConnected) {
      // read what's in the serial port into the buffer
      int bytetAvail = Serial.available();
      for (int i=0; i < bytetAvail; i++) {
        byte bb = Serial.read();
        buffer[bufferSize] = bb;
        bufferSize++;
        if (bufferSize > 511) bufferSize = 511;
        if (bb == (char)'<' && eofCount == 0) eofCount++;
        if (bb == (char)'E' && eofCount == 1) eofCount++;
        if (bb == (char)'O' && eofCount == 2) eofCount++;
        if (bb == (char)'F' && eofCount == 3) eofCount++;
        if (bb == (char)'>' && eofCount == 4) eofCount++;
        if (eofCount == 5) {
          // if we found <EOF> then we should have a buffer size of 128+5
          if (bufferSize == 133) {
            bufferSize = 0;
            eofCount = 0;
            //Serial.println("GOOD EOF FOUND!!!");
            // copy buffer to fftData
            for (int f=0; f<128; f++) {
              fftData.specData[f] = buffer[f];
            }
            dataReceivedTime = millis();
            fftData.ProcessSpecData();

#ifdef DEBUG
            if (comTps_ms + 1000 < millis()) {
              Serial.print("ESP32 comTps: ");
              Serial.println(comTps);
              comTps = 0;
              comTps_ms = millis();
            }
            comTps++;
#endif

          }
          else {
            // wrong size, just delete everything
            eofCount = 0;
            Serial.println("BAD EOF FOUND!!!");
            bufferSize = 0;
          }
        }
      }
      
      // check the buffer for an fft telegram
    }



/*
    if (!comClientConnected) {
      // clear out anything in the buffer
      int duds = Serial.available();
      for (int i=0; i < duds; i++) {
        Serial.read();
      }
      // send an initial request for fft data
      Serial.write("AuroraDrop;V1\r\n");
      comClientConnected = true;
    }

    uint32_t timeout = millis();
    while (Serial.available() < BINS && comClientConnected) {
      if (millis() - timeout > 1000) {
        Serial.print("!!! COM Client Timeout with Available: ");
        Serial.println(Serial.available());
        comClientConnected = false;
        // clear out any old
        int dud = Serial.available();
        for (int i=0; i < dud; i++) {
          Serial.read();
        }
        //delay(1000);
      }
    }

    if (comClientConnected) {

      // clear out any old data
      int old = Serial.available() - BINS;
      if (old > 0) {
        Serial.print("!! Too many bytes: ");
        Serial.println(old);
      } 
      byte by = 0;
      for (int i=0; i<old; i++) {
        by = Serial.read();
      }
      // read the latest data
      Serial.read(fftData.specData, 128);
      //for (int i=0; i<BINS; i++) {
      //  fftData.specData[i] = Serial.read();
      //}
      fftData.ProcessSpecData();

      // wait a wee bit, and send new request, looking for around 25 telegrams per second
      delay(25);   // 25 is good, using lower value only really useful when frame limiting is disabled
      Serial.write("AuroraDrop;V1\r\n");
    }
*/




  }
}


// 
void setupCOMClient() {

  xTaskCreatePinnedToCore(
        COMcode,                          // Function to implement the task
        "COMClient",                      // Name of the task
        10000,                            // Stack size in words
        NULL,                             // Task input parameter
        1,                                // Priority of the task
        &COM_Task,                        // Task handle
        0);                               // Core where the task should run
  //Serial.write("Auroradrop;V1");

}

