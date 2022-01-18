TaskHandle_t TCP_Task;
WiFiClient client;

uint32_t tcpMicros;

bool tcpClientConnected = false;
uint8_t tps = 0;                // telegrams per second
uint32_t tps_ms = millis();

// TCP main code
void TCPcode( void * parameter) {

  Serial.println("TCPcode starting");

  for(;;) {
    delay(1);           // DO NOT DELETE THIS LINE! It is needed to give the IDLE(0) task enough time and to keep the watchdog happy.
                        // taskYIELD(), yield(), vTaskDelay() and esp_task_wdt_feed() didn't seem to work.
                        
    tcpMicros = micros();

    if (WiFi.status() == WL_CONNECTED && wifiConnected) {
      // if the client isn't connect, then attempt
      if (!tcpClientConnected) {

        if (!client.connect(hostPcIpAddress, hostPcIpPort)) {
          Serial.println("TCP client connection failed");
          delay(1000);
        }
        else {
          Serial.println("TCP client connected");
          tcpClientConnected = true;
          client.println("AuroraDrop;V1");
        }
      }
      else {
        // if our TCP client is connected to the server, wait for data to be available
        uint32_t timeout = millis();
        while (client.available() < BINS && tcpClientConnected) {
          if (millis() - timeout > 1500) {
            Serial.println(">>> Client Timeout !");
            tcpClientConnected = false;
            client.stop();
            delay(1000);
          }
        }

        // if data is avaialble, process it
#ifdef DEBUG
        if (tcpClientConnected) {

          if (tps_ms + 1000 < millis()) {
            Serial.print("ESP32 TPS: ");
            Serial.println(tps);
            tps = 0;
            tps_ms = millis();
          }
          tps++;
#endif

          // clear out any old data
          int old = client.available() - BINS;
          if (old > 0) Serial.println("Too many bytes!!");
          byte by = 0;
          for (int i=0; i<old; i++) {
            by = client.read();
          }
          // read the latest data
          client.readBytes(fftData.specData,BINS);
          fftData.ProcessSpecData();
          fftData.noAudio = false;

          // wait a wee bit, and send new request, looking for around 25 telegrams per second
          delay(25);   // 25 is good, using lower value only really useful when frame limiting is disabled
          client.write("Auroradrop;V1");
        }
      }
    }
    else {
      delay(500);
    }
  }
}

// 
void setupTCPClient() {
  Serial.println("TCP Client setup...");

 xTaskCreatePinnedToCore(
        TCPcode,                          // Function to implement the task
        "TCPClient",                      // Name of the task
        10000,                            // Stack size in words
        NULL,                             // Task input parameter
        1,                                // Priority of the task
        &TCP_Task,                        // Task handle
        0);                               // Core where the task should run

}

