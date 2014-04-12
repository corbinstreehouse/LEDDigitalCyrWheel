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

#if !PATTERN_EDITOR
#define ACCELEROMETER_SUPPORT 0 // not in the sim..
#endif

#if ACCELEROMETER_SUPPORT
#include "Wire.h"
#include "LSM303.h"
#endif


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
#if ACCELEROMETER_SUPPORT
    LSM303 _compass;
#endif
    
    bool initSDCard();
    bool initCompass();
    bool loadCurrentSequence();
    void freePatternItems();
    void freeSequenceNames();
    void loadSequenceNamed(const char *filename);
public:
    CWPatternSequenceManager();
#if PATTERN_EDITOR
    ~CWPatternSequenceManager();
#endif
    bool init();
    
    void loadNextSequence(); // returns true if we could advance (or loop to the start)
    void loadDefaultSequence();
    
    
    // Playing back patterns
    void nextPatternItem();
    void firstPatternItem();
    void process(bool initialProcess); // Main loop work
    
    int getNumberOfSequenceNames() { return _numberOfAvailableSequences; };
#if PATTERN_EDITOR
    char *getSequenceNameAtIndex(int index) { return _sequenceNames[index]; }
    char *getCurrentSequenceName() { return _sequenceNames[_currentSequenceIndex]; };
    int getCurrentSequenceIndex() { return _currentSequenceIndex; }

    uint32_t getPixelCount() { return _pixelCount; }
    uint32_t getNumberOfPatternItems() { return _numberOfPatternItems; }
    uint32_t getCurrentPatternItemIndex() { return _currentPatternItemIndex; }
    CDPatternItemHeader *getPatternItemHeaderAtIndex(int index) { return &_patternItems[index]; }
    CDPatternItemHeader *getCurrentPatternItemHeader() {
        if (_currentPatternItemIndex >= 0 && _currentPatternItemIndex < _numberOfPatternItems) {
            return &_patternItems[_currentPatternItemIndex];
        } else {
            return NULL;
        }
    }
#endif
};


#endif /* defined(__LEDDigitalCyrWheel__CWPatternSequence__) */
