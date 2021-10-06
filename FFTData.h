class FFTData{
  public:

  #define MAX_SERIAL_MSG_LENGTH 255       // re-think !!!
  #define BINS 96     // re-think !!!, 192/2 = 96, 192/3 = 64

  uint8_t test1;


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

  uint16_t bpm = 64;    // for testing


  /* ---------------------------------------------------------------------------------------------------------

    TODO: do we want more bytes for better resolution? is 96 enough? want stereo? 

    We currently receive 96 bytes of data representing the audio level info for undefined? frequency ranges,
    starting with low bass at byte 0 to high trebele at byte 95. 
    This data is then processed and various useful and sometimes more manageable bins/flags are updated for
    use by pattern animations.

    TODO: do we need to allocate memory dynamicaly for ESP32? 96 bytes currently?

    byte *specData;
    specData = (byte *)malloc(BARS);
    - or -
    byte **specData = nullptr;
    specData = (byte **)malloc(BARS * sizeof(byte *));


    Arrays:
    'specData' array has 96 'bins' which contains the raw data direct from the serial comms message & has best resolution
    'specDataPeak' array has 96 'bins' which contains the highest peak value, which will drop off until zero
    'specData8' has 8 'bins' which contain the averaged values from raw data ranges 0-11, 12-23, 24-35, 36-47, 48-59, 60-71, 72-83, 84-95
    'specData16' has 16 'bins' which contain the averaged values from 0-5, 6-11 12-17, etc
    'specData32' has 32 'bins' which contain the averaged values from 0-2, 3-5, 6-8, etc
    ditto for peak arrays

    Flags:
    'NoConnection' indiicates there is no serial comms with pc
    'NoData' indiicates there is no audio data coming from pc
    'NoAudio' indiicates there is all zeros in the audio data coming from pc (silence)

    Future thoughts:
    Single averaged volume level from over entire frequency range
    Highest/lowest volume level from over entire frequency range
    Indication that volume level over entire spectrum is low (quiet section or fading, is this the same as above?)
    Indication of which frequency range is most active
    General Bpm/Tempo
    Flag to indicate sudden volume spikes within certain frequency ranges
    Indicators for highests peak(s) within full(sub) frequency range(s)
    
    #define LO_LO 0
    #define LO 1
    #define LO_MID 2
    #define MID 3
    #define MID_HI 4
    #define HI 5
    #define HI_HI 6
    #define HI_HI_HI 7

    uint8_t tempo = 92; 
    uint16_t bpm = 92;
    byte specDataAverageVolume;
    byte specDataMaxVolume;
    byte specDataMinVolume;
    byte specDataMaxBin;            // (0-95) which of the 96 raw bins has highest volume
    byte specDataMinBin;            // (0-95) which of the 96 raw bins has lowest volume
    byte specData8MaxBin[8];        // (0-11) which of the 12 sub bins within each of the 8 bins has highest volume
    byte specData8MinBin[8];        // (0-11) which of the 12 sub bins within each of the 8 bins has lowest volume
    byte specData16MaxBin[16];      // (0-5) which of the 6 sub bins within each of the 16 bins has highest volume
    byte specData16MinBin[16];      // (0-5) which of the 6 sub bins within each of the 16 bins has lowest volume
    byte specData32MaxBin[32];      // (0-2) which of the 3 sub bins within each of the 32 bins has highest volume
    byte specData32MinBin[32];      // (0-2) which of the 3 sub bins within each of the 32 bins has lowest volume


   ------------------------------------------------------------------------------------------------------ */

  byte specDataMaxVolume;
  byte specDataMinVolume;

  /* ------------------------------------------------------------------------------------------------------ */


  // TODO: allocate memory dynamicaly?
  byte **specDataTodo = nullptr;
  
  // new audio data arrays
  // all data (currently 96 bins, all mono)
  byte specData[BINS];                // 0-127 = silence to peak level, 128-255 == over peak 
  byte specDataPeak[BINS];            // 0-127 = silence to peak level, 128-255 == over peak 

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


  FFTData() {
    // TODO: do we need to do this, as arrays are currently only 96 bytes?
    // we should do dynamic allocation for large audio data, otherwise esp32 toolchain can't link static arrays of such a big size
    specDataTodo = (byte **)malloc(BINS * sizeof(byte *));
  };


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
            specDataMinVolume = specData[0];
            specDataMaxVolume = 0;
            for (uint8_t i = 0; i < (BINS); i++) 
            {
              // if we see any data, clear the noAudio flag
              if (specData[i] > 0) noAudio = false;

              // if min/max volume breached then update
              if (specData[i] > specDataMaxVolume) specDataMaxVolume = specData[i];
              if (specData[i] < specDataMinVolume) specDataMinVolume = specData[i];

              // new array, audio data 0-255 where 127 is peak, > 127 over peak
              specData[i] = inData[i];              // version, a value from 0 to 255, where 0 = silence


            }

            // todo peak dropoff timer
            if (dropOffTimer + DROP_OFF_TIME < millis()) dropOffTimer = millis();

            // put data into smaller bin groups for lower resolution workings
            long total = 0;
            // group of 8 bins
            for (uint8_t i = 0; i < (BINS); i=i+12) 
            {
              total = 0;
              for (uint8_t j = 0; j < 12; j++) 
                total = total + (long)specData[i+j];
              specData8[i/12] = total / 12;
            }

            // group of 16 bins
            for (uint8_t i = 0; i < (BINS); i=i+6) 
            {
              total = 0;
              for (uint8_t j = 0; j < 6; j++) 
                total = total + (long)specData[i+j];
              specData16[i/6] = total / 6;
            }

            // group of 32 bins
            for (uint8_t i=0; i<BINS; i=i+3) 
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

      // end 
    }
  }


  void processUDPData(uint8_t data[]) 
  {
    // audio spectrum data, 96 bytes long

    
            //PRINTS("Audio Spectrum Data\n");
            if (iSilence < 255) iSilence++;
            // we expect to find 96 bytes of data
            noAudio = true;
            specDataMinVolume = specData[0];
            specDataMaxVolume = 0;
            for (uint8_t i = 0; i < (BINS); i++) 
            {
              // if we see any data, clear the noAudio flag
              if (specData[i] > 0) noAudio = false;

              // if min/max volume breached then update
              if (specData[i] > specDataMaxVolume) specDataMaxVolume = specData[i];
              if (specData[i] < specDataMinVolume) specDataMinVolume = specData[i];

              // new array, audio data 0-255 where 127 is peak, > 127 over peak
              specData[i] = data[i+7];              // version, a value from 0 to 255, where 0 = silence


            }



  }


};

