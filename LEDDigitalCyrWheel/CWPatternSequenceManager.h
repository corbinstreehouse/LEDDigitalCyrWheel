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

#include "CDOrientation.h"
#include "LEDCommon.h"

#if PATTERN_EDITOR
    #include "CDSimulatorLEDPatterns.h"
    #define LED_PATTERNS_CLASS CDSimulatorLEDPatterns
    @class CDCyrWheelView;
#else
    #if USE_OCTO // doesn't work
        #include "OctoWS2811.h"
        #include "OctoWS2811LEDPatterns.h"
        #define LED_PATTERNS_CLASS OctoWS2811LEDPatterns
    #elif USE_ADAFRUIT
        #include "NeoPixelLEDPatterns.h"
        #define LED_PATTERNS_CLASS NeoPixelLEDPatterns
    #else
        #include "FastLEDPatterns.h"
        #define LED_PATTERNS_CLASS FastLEDPatterns
    #endif
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

    uint8_t m_savedBrightness;
    
    uint32_t _shouldRecordData:1;
    uint32_t m_shouldIgnoreButtonClickWhenTimed:1;
    uint32_t __reserved:31;
    
    LED_PATTERNS_CLASS m_ledPatterns;
    CDOrientation m_orientation;

    uint32_t m_timedPatternStartTime; // In milliseconds; the time that all the current run of timed patterns starts, so we can accurately generate a full duration for all of them
    uint32_t m_timedUsedBeforeCurrentPattern;
    
    bool initSDCard();
    bool initOrientation();
    bool initStrip();
    void freePatternItems();
    void freeSequenceNames();
    void loadSequenceNamed(const char *filename);
    
    void updateBrightness();
    
    inline CDPatternItemHeader *getCurrentItemHeader() {
        return &_patternItems[_currentPatternItemIndex];
    }
    
    // for crossfade
    inline CDPatternItemHeader *getNextItemHeader() {
        int tmp = _currentPatternItemIndex;
        tmp++;
        if (tmp >= _numberOfPatternItems) {
            tmp = 0;
        }
        return &_patternItems[tmp];
    }
    
    inline CDPatternItemHeader *getPreviousItemHeader() {
        int tmp = _currentPatternItemIndex;
        tmp--;
        if (tmp < 0) {
            tmp = _numberOfPatternItems - 1;
        }
        return &_patternItems[tmp];
    }
    
    inline void resetStartingTime() {
        m_timedPatternStartTime = millis();
        m_timedUsedBeforeCurrentPattern = 0;
    }
    
    CDPatternItemHeader makeFlashPatternItem(CRGB color);
public:
    CWPatternSequenceManager();
#if PATTERN_EDITOR
    ~CWPatternSequenceManager();
    void setCyrWheelView(CDCyrWheelView *view); // Binding..
#endif
    bool init(bool buttonIsDown);
    
    void buttonClick();
    void buttonLongClick();
    void loadNextSequence(); // returns true if we could advance (or loop to the start)
    void loadDefaultSequence();
    bool loadCurrentSequence();
    
    // Playing back patterns
    void nextPatternItem();
    void firstPatternItem();
    void process();
    
    void makeSequenceFlashColor(uint32_t color);
    
    int getNumberOfSequenceNames() { return _numberOfAvailableSequences; };
    void setLowBatteryWarning();
    inline LEDPatterns *getLEDPatterns() { return &m_ledPatterns; }
#if PATTERN_EDITOR
    char *getSequenceNameAtIndex(int index) { return _sequenceNames[index]; }
    char *getCurrentSequenceName() { return _sequenceNames[_currentSequenceIndex]; };
    int getCurrentSequenceIndex() { return _currentSequenceIndex; }
    uint32_t getPatternTimePassed() { return millis() - m_timedPatternStartTime - m_timedUsedBeforeCurrentPattern; };
    uint32_t getPatternTimePassedFromFirstTimedPattern() { return millis() - m_timedPatternStartTime; };

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
