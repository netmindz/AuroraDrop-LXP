class SerialData{
  public:

  #define MAX_SERIAL_MSG_LENGTH 255       // re-think !!!
  #define BARS 96     // re-think !!!, 192/2 = 96, 192/3 = 64




//  const uint8_t SERIAL_MSG_AUDIO_SPECTRUM = 65;  // A
  const uint32_t DROP_OFF_TIME = 10;


  bool preambleFound = false;
  uint8_t preambleCount = 0;
  byte preambleBytes[7];

  bool messageFound = false;
  uint8_t messageSize = 0;     // max 255 bytes in message
  uint8_t messageCount = 0;    // byte counter for message current being read
  uint8_t messageType = 0;



  byte inData[MAX_SERIAL_MSG_LENGTH];
//  byte spec8Data[BARS];             // 0-8 = silence to peak
//  byte spec8DataPeak[BARS];         // 0-8 = silence to peak
//  byte spec255Data[BARS];           // 0-127 = silence to peak level, 128-255 == over peak 
  uint8_t iSilence = 255;
  uint32_t dropOffTimer = millis();



  // new audi0 data arrays
  // all data bins (currently 96, all mono)
  byte specData[BARS];           // 0-127 = silence to peak level, 128-255 == over peak 
  byte specDataPeak[BARS];           // 0-127 = silence to peak level, 128-255 == over peak 

  // 8 averaged bins
  byte specData8[8];
  byte specData8Peak[8];

  // 16 averaged bins
  byte specData16[16];
  byte specData16Peak[16];

  // 32 averaged bins
  byte specData32[32];
  byte specData32Peak[32];
  



  uint32_t dataReceivedTime = 0;

  bool noConnection = true;
  bool noData = true;
  bool noAudio = true;
  uint8_t noAudioFade = 0;
  void processSerialData()
  {
    int i = 1;
    while (Serial.available() > 0) 
    {

      // the preamble connsists of five + two (seven) bytes: 69, 96 ,69 ,69 ,0, xx, yy 
      // where xx denotes the message type, and yy the length (number of bytes) in the message
      if (!preambleFound) 
      {
        // start looking for the preamble sequence
        while (Serial.available() > 0 && !preambleFound) 
        {
          // shift preamble array up
          preambleBytes[6] = preambleBytes[5];
          preambleBytes[5] = preambleBytes[4];
          preambleBytes[4] = preambleBytes[3];
          preambleBytes[3] = preambleBytes[2];
          preambleBytes[2] = preambleBytes[1];
          preambleBytes[1] = preambleBytes[0];
          // read and add new byte
          preambleBytes[0] = Serial.read();
          // compare latest 5 byte sequence with preamble
          if (preambleBytes[6] == 69 && preambleBytes[5] == 96 && preambleBytes[4] == 69 && preambleBytes[3] == 96 && preambleBytes[2] == 0) 
          {
            // if match then set flag
            preambleFound = true;
            messageFound = false;
            messageCount = 0;
            messageSize = preambleBytes[0];
            messageType = preambleBytes[1];
            //PRINTS("PREAMBLE FOUND\n");
          }
        }
      }

      // process the incoming message data in the buffer
      if (preambleFound && !messageFound) 
      {
        while (Serial.available() > 0 && !messageFound) 
        {
          // get the next byte and put into array
          inData[messageCount] = Serial.read();
          messageCount++;
          if (messageCount >= messageSize)
          {
            messageFound = true;
          }
        }
      }


      // process the received message
      if (preambleFound && messageFound) 
      {
        //Serial.print("MESSAGE FOUND: ");
        //Serial.println(messageType);

        dataReceivedTime = millis();
        // process the message depending on its type
        String sTemp;
        switch (messageType) 
        {
          case 0:
          // do nothing

          break;


          case SERIAL_MSG_AUDIO_SPECTRUM:
            // audio spectrum data, 96 bytes long
            //PRINTS("Audio Spectrum Data\n");
            if (iSilence < 255) iSilence++;
            // we expect to find 96 bytes of data
            noAudio = true;
            for (uint8_t i = 0; i < (BARS); i++) 
            {
              // if we see any data, clear the noAudiod flag
              if (specData[i] > 0) noAudio = false;

              // new array, audio data 0-255 where 127 is peak, > 127 over peak
              specData[i] = inData[i];              // version, a value from 0 to 255, where 0 = silence


/*
              // old array, to remove later
              spec255Data[i] = inData[i];              // version, a value from 0 to 255, where 0 = silence
              // scale 0-255 to 0-15  (normal range = 0-8)
              //double x = (double)inData[i];
              //double y = x / 17;
              spec8Data[i] = (byte)((float)inData[i] / 17.0);                // old version, a value from 0 to 8
              if (dropOffTimer + DROP_OFF_TIME < millis())
              {
                if (spec8DataPeak[i] > 0) spec8DataPeak[i]--;
              }
              if (spec8Data[i] > spec8DataPeak[i]) spec8DataPeak[i] = spec8Data[i];
              if (spec8Data[i] > 1) iSilence = 0;

*/


            }
            // reset the no audio fader if we found audio
            if (noAudioFade > 0 && noAudio) noAudioFade--;
            if (!noAudio) noAudioFade = 255;

            // todo peak dropoff timer
            if (dropOffTimer + DROP_OFF_TIME < millis()) dropOffTimer = millis();

            // put data into smaller bin groups for lower resolution workings
            long total = 0;
            // group of 8 bins
            for (uint8_t i = 0; i < (BARS); i=i+12) 
            {
              total = 0;
              for (uint8_t j = 0; j < 12; j++) 
                total = total + (long)specData[i+j];
              specData8[i/12] = total / 12;
            }

            // group of 16 bins
            for (uint8_t i = 0; i < (BARS); i=i+6) 
            {
              total = 0;
              for (uint8_t j = 0; j < 6; j++) 
                total = total + (long)specData[i+j];
              specData16[i/6] = total / 6;
            }

            // group of 32 bins
            for (uint8_t i=0; i<BARS; i=i+3) 
            {
              total = 0;
              for (uint8_t j = 0; j < 3; j++) 
                total = total + (long)specData[i+j];
              specData32[i/3] = total / 3;
            }



            //Serial.println(spec8Data[4]);

            break;




        }

        preambleFound = false;
        messageFound = false;
      }





      // end processing
    }
  }



/*

void processSerialData()
{

  // if we've not had data for 5 seconds, indicate silence and go to demo mode
  if (machineState == MS_STANDARD && millis() > dataReceivedTime + 5000) {
    iSilence = AUDIO_SILENCE;
    machineState = MS_DEMO1;
    stateStep = 0;
    changeMachineState = true;
    changeStep = true;
    
  }
  // process data in the serial input buffer
  while (Serial.available() > 0) 
  {
    //PRINTS("SERIAL FOUND\n");
    // the preamble connsists of five + two (seven) bytes: 69, 96 ,69 ,69 ,0, xx, yy 
    // where xx denotes the message type, and yy the length (number of bytes) in the message
    if (!preambleFound) 
    {
        // start looking for the preamble sequence

        while (Serial.available() > 0 && !preambleFound) 
        {
          // shift preamble array up
          preambleBytes[6] = preambleBytes[5];
          preambleBytes[5] = preambleBytes[4];
          preambleBytes[4] = preambleBytes[3];
          preambleBytes[3] = preambleBytes[2];
          preambleBytes[2] = preambleBytes[1];
          preambleBytes[1] = preambleBytes[0];
          // read and add new byte
          preambleBytes[0] = Serial.read();
          // compare latest 5 byte sequence with preamble
          if (preambleBytes[6] == 69 && preambleBytes[5] == 96 && preambleBytes[4] == 69 && preambleBytes[3] == 96 && preambleBytes[2] == 0) {
            // if match then set flag
            preambleFound = true;
            messageFound = false;
            messageCount = 0;
            messageSize = preambleBytes[0];
            messageType = preambleBytes[1];
            //PRINTS("PREAMBLE FOUND\n");
          }
        }
    }

    
    // process the incoming message data in the buffer
    if (preambleFound && !messageFound) 
    {

      while (Serial.available() > 0 && !messageFound) 
      {
        // get the next byte and put into array
          inData[messageCount] = Serial.read();
          messageCount++;
          if (messageCount >= messageSize)
          {
            messageFound = true;
          }
      }
    }

    // process the received message
    if (preambleFound && messageFound) 
    {
      dataReceivedTime = millis();


      // process the message depending on its type
      String sTemp;
      switch (messageType) 
      {
        case 0:
          // do nothing
          break;
        case SERIAL_MSG_CHANGE_MODE:
          switch (inData[0])
          {
            case 1:
              machineState = MS_STANDARD;
              break;
            case 2:
              machineState = MS_SPECTRUM;
              break;
            case 3:
              machineState = MS_CPU_USAGE;
              break;
            case 4:
              machineState = MS_DEMO1;
              break;
            case 5:
              machineState = MS_DEMO2;
              break;
          }
          stateStep = 0;
          changeMachineState = true;
          changeStep = true;
          break;
        case SERIAL_MSG_AUDIO_SPECTRUM:
          // audio spectrum data, 96 bytes long
          //PRINTS("Audio Spectrum Data\n");
          if (iSilence < 255) iSilence++;
          // we expect to find at least 8, and up to 32/96 bytes of data
          for (uint8_t i = 0; i < (MAX_DEVICES * 8); i++) 
          {
            spec255Data[i] = inData[i];              // new version, a value from 0 to 255, where 0 = silence
            // scale 0-255 to 0-15  (normal range = 0-8)
            //double x = (double)inData[i];
            //double y = x / 17;
            spec8Data[i] = (byte)((float)inData[i] / 17.0);                // old version, a value from 0 to 8
            if (dropOffTimer + DROP_OFF_TIME < millis())
            {
              if (spec8DataPeak[i] > 0) spec8DataPeak[i]--;
            }
            if (spec8Data[i] > spec8DataPeak[i]) spec8DataPeak[i] = spec8Data[i];
            if (spec8Data[i] > 1) iSilence = 0;
          }
          if (dropOffTimer + DROP_OFF_TIME < millis()) dropOffTimer = millis();
          break;

        case SERIAL_MSG_CPU_USAGE:
          // cpu usage data, 66 bytes long, 1st byte is number of cores, 2nd is overall cpu usage, remaining 64 are individual core/thread usage 
          for (uint8_t i = 0; i < 66; i++) 
          {
            cpuUsageData[i] = inData[i];
          }
          break;

        case SERIAL_MSG_REQ_CONFIG_INFO:
          // request to respond with configuration info, 0 bytes long
          // eprom version
          switch (eepromNetworkMode) {
            case 0:
              sTemp = "0";
              break;
            case 1:
              sTemp = "1";
              break;
            case 2:
              sTemp = "2";
              break;
            case 3:
              sTemp = "3";
              break;
          }
          // skip eeprom version for now
          sTemp = "1," + sTemp + "," + eepromWifiSSID + "," + eepromWifiPassword + "," + eepromApSSID + "," + eepromApPassword + "," + eepromWelcomeMessage;
          Serial.println(sTemp); 
          break;
              
        case SERIAL_MSG_BITMAP:
          // 192 bytes, 2 bits per pixel, 96*8 pixels = 1536 bits
          for (uint16_t i = 0; i < 192; i++) 
          {
            // base coordinates for the first pixel
            byte yy = (i/24);  //  36 = 192bytes / 8rows  // 36 = 288bytes / 8rows      //96 cols / 3 bits per pixel
            byte xx = (i*4)-(yy*96);
            byte pix;
            // first pixel
            pix = bitRead(inData[i], 0) + (bitRead(inData[i], 1) * 2);
            bitmapData[xx][yy] = pix;
            // second pixel
            pix = bitRead(inData[i], 2) + (bitRead(inData[i], 3) * 2);
            bitmapData[xx+1][yy] = pix;
            // third pixel
            pix = bitRead(inData[i], 4) + (bitRead(inData[i], 5) * 2);
            bitmapData[xx+2][yy] = pix;
            // fourth pixel
            pix = bitRead(inData[i], 6) + (bitRead(inData[i], 7) * 2);
            bitmapData[xx+3][yy] = pix;

          }




          //byte bitmapData[MAX_DEVICES * 8][8];

          //for (uint8_t i = 0; i < (MAX_DEVICES * 8); i++) 
          //{
          //  specData[i] = inData[i];              // a value from 0 to 255, where 0 = silence
          //  if (specData[i] > 1) iSilence = 0;
          //}

          break;
          


        case SERIAL_MSG_SHUTTING_DOWN:
          // pc shutting down, go to sleep 
          PRINTS("\n## SHUTTING DOWN ##\n");
          
          machineState = MS_SHUTTING_DOWN;
          stateStep = 0;
          changeMachineState = true;
          changeStep = true;
          break;
          
        default:
          // log only
          PRINT("Unknown Message Type:", preambleBytes[0]);
          break;
      }

      preambleFound = false;
      messageFound = false;
    }

  }
  
}


*/


};

