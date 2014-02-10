//
//  CWPatternSequenceManager.h
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/5/14.
//
//

#ifndef __LEDDigitalCyrWheel__CWPatternSequence__
#define __LEDDigitalCyrWheel__CWPatternSequence__

#include <stdint.h>
#include "CDPatternData.h"

#include "Arduino.h"
#include "SD.h"

class CWPatternSequenceManager {
private:
    // all available sequences
    int _numberOfAvailableSequences;
    char **_sequenceNames;
    int _currentSequenceIndex;
    
    // Current sequence information
    CDPatternItemHeader *_patternItems;
    int _numberOfPatternItems;
    uint32_t _pixelCount; // What was designed against
    
    bool initSDCard();
    void loadCurrentSequence();
public:
    bool init();
    
    bool loadFirstSequence(); // Loads the first sequence
    void loadNextSequence(); // returns true if we could advance (or loop to the start)
    void loadDefaultSequence();
};


#endif /* defined(__LEDDigitalCyrWheel__CWPatternSequence__) */
