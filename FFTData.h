class FFTData{

    // this class holds some data, but nothing calls its functions...
    // ... and a lot of things are defined and never used.

    public:

    #define SERIAL_MSG_AUDIO_SPECTRUM 65  // to revisit serial messaging...
    #define MAX_SERIAL_MSG_LENGTH 255     // re-think !!!
    #define BINS 128                      // was 96    // re-think !!!, 192/2 = 96, 192/3 = 64

    uint8_t test1;

    //  const uint8_t SERIAL_MSG_AUDIO_SPECTRUM = 65;  // A
    const uint32_t DROP_OFF_TIME = 10;    // not implemented yet

    bool preambleFound = false;
    uint8_t preambleCount = 0;
    byte preambleBytes[7];

    uint32_t serial_pps_ms = millis();
    uint8_t serial_pps_count = 0;
    uint8_t serial_pps = 0;

    bool messageFound = false;
    uint8_t messageSize = 0;     // max 255 bytes in message
    uint8_t messageCount = 0;    // byte counter for message current being read
    uint8_t messageType = 0;

    byte inData[MAX_SERIAL_MSG_LENGTH];
    uint8_t iSilence = 255;                 // to implement instead of noAudio boolean, user counter insteasd for short delay?
    uint32_t dropOffTimer = millis();       // to implement peak drop off 

    uint16_t bpm = 120;    // for testing currently, not fully implemented yet by most test patterns

    /* ---------------------------------------------------------------------------------------------------------

    TODO: do we want more bytes for better resolution? is 128 enough? want stereo? 

    We currently receive 128 bytes of data representing the audio level info for undefined? frequency ranges,
    starting with low bass at byte 0 to high trebele at byte 95. 
    This data is then processed and various useful and sometimes more manageable bins/flags are updated for
    use by pattern animations.

    TODO: do we need to allocate memory dynamicaly for ESP32? 128 bytes currently?

    byte *specData;
    specData = (byte *)malloc(BARS);
    - or -
    byte **specData = nullptr;
    specData = (byte **)malloc(BARS * sizeof(byte *));


    Arrays:
    'specData' array has 128 'bins' which contains the raw data direct from the serial comms message & has best resolution
    'specDataPeak' array has 128 'bins' which contains the highest peak value, which will drop off until zero
    'specData8' has 8 'bins' which contain the averaged values from raw data ranges 0-11, 12-23, 24-35, 36-47, 48-59, 60-71, 72-83, 84-95
    'specData16' has 16 'bins' which contain the averaged values from 0-5, 6-11 12-17, etc
    'specData32' has 32 'bins' which contain the averaged values from 0-2, 3-5, 6-8, etc
    'specData64' has 64 'bins' which contain the averaged values from 0-1, 2-3, 4-5, etc
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
    byte specDataMaxBin;            // (0-127) which of the 128 raw bins has highest volume
    byte specDataMinBin;            // (0-127) which of the 128 raw bins has lowest volume
    byte specData8MaxBin[8];        // (0-11) which of the 12 sub bins within each of the 8 bins has highest volume
    byte specData8MinBin[8];        // (0-11) which of the 12 sub bins within each of the 8 bins has lowest volume
    byte specData16MaxBin[16];      // (0-5) which of the 6 sub bins within each of the 16 bins has highest volume
    byte specData16MinBin[16];      // (0-5) which of the 6 sub bins within each of the 16 bins has lowest volume
    byte specData32MaxBin[32];      // (0-2) which of the 3 sub bins within each of the 32 bins has highest volume
    byte specData32MinBin[32];      // (0-2) which of the 3 sub bins within each of the 32 bins has lowest volume

    ------------------------------------------------------------------------------------------------------ */

    byte specDataMaxVolume;
    byte specDataMinVolume;

    // TODO: allocate memory dynamicaly?
    //
    byte **specDataTodo = nullptr;

    // new audio data arrays
    // all data (currently 128 bins, all mono)
    //
    byte specData[BINS];                // 0-127 = silence to peak level, 128-255 == over peak 
    byte specDataPeak[BINS];            // 0-127 = silence to peak level, 128-255 == over peak 

    // 8 averaged bins
    //
    byte specData8[8];
    byte specData8Peak[8];

    // 16 averaged bins
    //
    byte specData16[16];
    byte specData16Peak[16];

    // 32 averaged bins
    //
    byte specData32[32];
    byte specData32Peak[32];

    // 64 averaged bins
    //
    byte specData64[64];
    byte specData64Peak[64];

    uint32_t dataReceivedTime = 0;

    bool noConnection = true;
    bool noData = true;
    bool noAudio = true;

    FFTData() {

        // TODO: do we need to do this, as arrays are currently only 128 bytes? should we do it anyway?
        // we should do dynamic allocation for large audio data, otherwise esp32 toolchain can't link static arrays of such a big size
        //
        specDataTodo = (byte **)malloc(BINS * sizeof(byte *));

    };

    void ProcessSpecData() {

        // this doesn't seem to ever be called.

        // TODO: peak dropoff timer
        //
        if (dropOffTimer + DROP_OFF_TIME < millis()) dropOffTimer = millis();

        noAudio = true;
        specDataMinVolume = specData[0];
        specDataMaxVolume = 0;

        for (uint8_t i = 0; i < (BINS); i++) {

            // if we see any data, clear the noAudio flag
            //
            if (specData[i] > 0) {
                
                noAudio = false;

            }

            // if min/max volume breached then update
            //
            if (specData[i] > specDataMaxVolume) {
                
                specDataMaxVolume = specData[i];

            }

            if (specData[i] < specDataMinVolume) {
                
                specDataMinVolume = specData[i];

            }

            // peak
            //
            if (specDataPeak[i] > 0) {
                
                specDataPeak[i]--;

            }

            if (specData[i] > specDataPeak[i]) {
                
                specDataPeak[i] = specData[i];

            }

        }

        // put data into smaller bin groups for lower resolution workings
        //
        long total = 0;

        // group of 8 bins
        //
        for (uint8_t i = 0; i < (BINS); i=i+12) {

            total = 0;

            for (uint8_t j = 0; j < 12; j++) {

                total = total + (long)specData[i+j];

            }

            specData8[i/12] = total / 12;

        }

        // group of 16 bins
        //
        for (uint8_t i = 0; i < (BINS); i=i+6) {

            total = 0;

            for (uint8_t j = 0; j < 6; j++) {
            
                total = total + (long)specData[i+j];

            }
            
            specData16[i/6] = total / 6;

        }

        // group of 32 bins
        //
        for (uint8_t i=0; i<BINS; i=i+3) {

            total = 0;

            for (uint8_t j = 0; j < 3; j++) {

                total = total + (long)specData[i+j];

            }

            specData32[i/3] = total / 3;

        }

        // group of 64 bins
        //
        for (uint8_t i=0; i<BINS; i=i+2) {

            total = 0;

            for (uint8_t j = 0; j < 2; j++) {
            
                total = total + (long)specData[i+j];

            }

            specData64[i/2] = total / 2;

        }

    }

};
