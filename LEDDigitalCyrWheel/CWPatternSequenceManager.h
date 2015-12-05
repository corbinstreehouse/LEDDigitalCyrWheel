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

#if SD_CARD_SUPPORT
#include "SD.h"
#endif

#include "CDOrientation.h"
#include "LEDCommon.h"
#include "LEDPatterns.h"
#include "CDPatternSequenceManagerShared.h"

#define PATTERN_FILE_EXTENSION "PAT"
#define PATTERN_FILE_EXTENSION_LC "pat" // stupid non case sensative


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

    // Current pattern item information
    int _currentPatternItemIndex;

    uint8_t m_savedBrightness;
    
    uint32_t m_shouldRecordData:1;
    uint32_t m_shouldIgnoreButtonClickWhenTimed:1;
    uint32_t m_sdCardWorks:1;
    uint32_t m_dynamicMode:1;
    uint32_t __reserved:27;
    
    LED_PATTERNS_CLASS m_ledPatterns;
    CDOrientation m_orientation;

    uint32_t m_timedPatternStartTime; // In milliseconds; the time that all the current run of timed patterns starts, so we can accurately generate a full duration for all of them
    uint32_t m_timedUsedBeforeCurrentPattern;
    CDWheelState m_state;
#if PATTERN_EDITOR
    NSURL *m_baseURL;
    NSURL *m_patternDirectoryURL;
#endif
    void loadSettings();
    
    bool initSDCard();
    bool initOrientation();
    bool initStrip();
    void freePatternItems();
    void freeSequenceNames();
    void loadSequenceNamed(const char *filename);
    
    void updateBrightness();
    const char *getRootDirectory();
    
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
    
    void loadCurrentPatternItem();
    char *getFullpathName(const char *name, char *buffer, int bufferSize);
    
public:
    CWPatternSequenceManager();
#if PATTERN_EDITOR
    ~CWPatternSequenceManager();
    void setCyrWheelView(CDCyrWheelView *view); // Binding..
    void setBaseURL(NSURL *url);
    void setPatternDirectoryURL(NSURL *url);
#endif
    void init();
    
    void buttonClick();
    void buttonLongClick();
    
    void loadFirstSequence();
    void loadNextSequence();
    void loadPriorSequence();
    
    void restartCurrentSequence();
    void setCurrentSequenceAtIndex(int index);
    bool deleteSequenceAtIndex(int index);
    
    bool getCardInitPassed() { return m_sdCardWorks; }
    
    void loadSequencesFromDisk();
    
    void loadDefaultSequence();
    void loadCurrentSequence(); // loads the default sequence if there is no current one..
    
    // Playing back patterns in the current sequence
    void nextPatternItem();
    void firstPatternItem();
    void priorPatternItem();
    
    void processCommand(CDWheelCommand command);
    
    void setDynamicPatternWithHeader(CDPatternItemHeader *header);
    void setDynamicPatternType(LEDPatternType type, CRGB color = CRGB::Red);
    
    void startCalibration();
    void endCalibration();
    void cancelCalibration();

    void startRecordingData();
    void endRecordingData();
    
    CDWheelState getWheelState() { return m_state; }
    
    void process();
    
    void makeSequenceFlashColor(CRGB color);
    void flashThreeTimes(CRGB color, uint32_t delayAmount = 150);
    
    int getNumberOfSequenceNames() { return _numberOfAvailableSequences; };
    void setLowBatteryWarning();
    inline LEDPatterns *getLEDPatterns() { return &m_ledPatterns; }
    char *getSequenceNameAtIndex(int index) {
        if (index >= 0 && index <= _numberOfAvailableSequences)
            return _sequenceNames[index];
        else
            return NULL;
    }
    int getIndexOfSequenceName(char *name);
    bool isSequenceEditableAtIndex(int index);
    char *getCurrentSequenceName() { return _sequenceNames[_currentSequenceIndex]; };
    int getCurrentSequenceIndex() { return _currentSequenceIndex; }
    uint32_t getPatternTimePassed() { return millis() - m_timedPatternStartTime - m_timedUsedBeforeCurrentPattern; };
    uint32_t getPatternTimePassedFromFirstTimedPattern() { return millis() - m_timedPatternStartTime; };

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
    
    void play() { m_ledPatterns.play(); }
    void pause() { m_ledPatterns.pause(); }
    bool isPaused() { return m_ledPatterns.isPaused(); }

};


#endif /* defined(__LEDDigitalCyrWheel__CWPatternSequence__) */
