#ifndef PatternAudioAurora_H
#define PatternAudioAurora_H

class PatternAudioAurora : public Drawable {
private:

  bool hueCycle1 = false;
  byte hue1 = 0;
  uint32_t hue_ms1;
  uint8_t offset1 = 0;
  uint8_t counter1 = 0;
  uint8_t mirror1 = 0;
  uint8_t stagger1 = 1;

  bool hueCycle2 = false;
  byte hue2 = 0;
  uint32_t hue_ms2;
  uint8_t offset2 = 0;
  uint8_t counter2 = 0;
  uint8_t mirror2 = 0;
  uint8_t stagger2 = 1;

public:
  PatternAudioAurora() {
    name = (char *)"Aurora";
    id = "X";
  }


  uint8_t zzzz = 0;

  // #############
  // ### START ###
  // #############
  void start(uint8_t _pattern) {

    hue1 = random8(0,255);
    hue_ms1 = millis();
    hueCycle1 = true; // random8(0,2);
    offset1 = 0;
    counter1 = 0;
    mirror1 = random8(0,4);
    stagger1 = random8(0,5); //if (stagger==3) stagger = 4;

    hue2 = random8(0,255);
    hue_ms2 = millis();
    hueCycle2 = true; // random8(0,2);
    offset2 = 0;
    counter2 = 0;
    mirror2 = random8(0,4);
    stagger2 = random8(0,5); //if (stagger==3) stagger = 4;
        
    effects.NoiseVariablesSetup();

        
  }

  // poor performance and overall effect

  // ##################
  // ### DRAW FRAME ###
  // ##################
  unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

    // we really need a little extra dimming
    effects.DimAll(230);

    // do it all twice
    offset1 = 0;
    for (int y=0; y<MATRIX_WIDTH; y++) 
    {
      for (int x=0; x<MATRIX_HEIGHT; x++) 
      {
        int pos = x + offset1;
        if (pos >= 96) pos = 96 - pos;
        byte a = fftData.specData[fftData.specData[pos]];
        a = a / random8(2,6);
        float zzz = pos * 2.68;
        switch (mirror1)
        {
          case 0:
            effects.leds[XY16(x, y)] += effects.ColorFromCurrentPalette((int)zzz + hue1, a);
            break;
          case 1:
            effects.leds[XY16(MATRIX_WIDTH - x, y)] += effects.ColorFromCurrentPalette((int)zzz + hue1, a);
            break;
          case 2:
            effects.leds[XY16(x, MATRIX_HEIGHT - y)] += effects.ColorFromCurrentPalette((int)zzz + hue1, a);
            break;
          case 3:
            effects.leds[XY16(MATRIX_WIDTH - x, MATRIX_HEIGHT - y)] += effects.ColorFromCurrentPalette((int)zzz + hue1, a);
            break;
        }
      }
      offset1 = offset1 + stagger1;
      if (offset1 >= 96) offset1 = 0;
    }


    // do it all twice
    offset2 = 0;
    for (int y=0; y<MATRIX_WIDTH; y++) 
    {
      for (int x=0; x<MATRIX_HEIGHT; x++) 
      {
        int pos = x + offset2;
        if (pos >= 96) pos = 96 - pos;
        byte a = fftData.specData[fftData.specData[pos]];
        a = a / random8(2,6);
        float zzz = pos * 2.68;
        switch (mirror2)
        {
          case 0:
            effects.leds[XY16(x, y)] += effects.ColorFromCurrentPalette((int)zzz + hue2, a);
            break;
          case 1:
            effects.leds[XY16(MATRIX_WIDTH - x, y)] += effects.ColorFromCurrentPalette((int)zzz + hue2, a);
            break;
          case 2:
            effects.leds[XY16(x, MATRIX_HEIGHT - y)] += effects.ColorFromCurrentPalette((int)zzz + hue2, a);
            break;
          case 3:
            effects.leds[XY16(MATRIX_WIDTH - x, MATRIX_HEIGHT - y)] += effects.ColorFromCurrentPalette((int)zzz + hue2, a);
            break;
        }
      }
      offset2 = offset2 + stagger2;
      if (offset2 >= 96) offset2 = 0;
    }
     
    
    if (hueCycle1 && hue_ms1 + 200 < millis()) 
    {
      hue_ms1 = millis();
      hue1++;
    }

    if (hueCycle2 && hue_ms2 + 200 < millis()) 
    {
      hue_ms2 = millis();
      hue2++;
    }




    return 0;
  }
};

#endif
