// This effect does nothing, intentionally. Good for when you sometimes want... nothing.

#ifndef PatternEffectNOOP_H
#define PatternEffectNOOP_H

class PatternEffectNOOP : public Drawable {
    
private:


public:

    PatternEffectNOOP() {

      name = (char *)"Doing Nothing, Intentionally";
      id = "!";
      enabled = true;

    }


    void start(uint8_t _pattern) {
        
    }

    unsigned int drawFrame(uint8_t _pattern, uint8_t _total) {

        delay(1); // just take 1ms to do nothing.

        return 0;

    }

};

#endif