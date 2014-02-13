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
    uint32_t _numberOfPatternItems;
    uint32_t _pixelCount; // What was designed against

    // Current pattern item information
    int _currentPatternItemIndex;
    uint32_t _patternStartTime;
    
    
    bool initSDCard();
    bool loadCurrentSequence();
    void freePatternItems();
    void freeSequenceNames();
public:
    CWPatternSequenceManager();
#if PATTERN_EDITOR
    ~CWPatternSequenceManager();
#endif
    bool init();
    
    bool loadFirstSequence(); // Loads the first sequence
    void loadNextSequence(); // returns true if we could advance (or loop to the start)
    void loadDefaultSequence();
    
    
    // Playing back patterns
    void nextPatternItem();
    void process(); // Main loop work
    
#if PATTERN_EDITOR
    char *getSequenceNameAtIndex(int index) { return _sequenceNames[index]; }
    int getNumberOfSequenceNames() { return _numberOfAvailableSequences; };
    int getCurrentSequenceIndex() { return _currentSequenceIndex; }

    uint32_t getPixelCount() { return _pixelCount; }
    uint32_t getNumberOfPatternItems() { return _numberOfPatternItems; }
    CDPatternItemHeader *getPatternItemHeaderAtIndex(int index) { return &_patternItems[index]; }
#endif
};


#endif /* defined(__LEDDigitalCyrWheel__CWPatternSequence__) */
