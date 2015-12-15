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
    #include "SdFat.h"
#endif

#include "CDOrientation.h"
#include "LEDCommon.h"
#include "LEDPatterns.h"
#include "CDPatternSequenceManagerShared.h"

#define SEQUENCE_FILE_EXTENSION "pat"

#if PATTERN_EDITOR
    #include "CDSimulatorLEDPatterns.h"
    #define LED_PATTERNS_CLASS CDSimulatorLEDPatterns
    @class CDCyrWheelView;
    #define MAX_PATH  1024 // larger for the desktop; this is created on the stack...
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

    #define MAX_PATH  260 // yeah, copy windows
#endif

typedef enum {
    CDPatternFileTypeSequenceFile, // .pat
    CDPatternFileTypeBitmapImage, // .bmp / bitmap
    CDPatternFileTypeDirectory,
    CDPatternFileTypeDefaultSequence, // special
    CDPatternFileTypeUnknown,
} CDPatternFileType;

// Keeping a struct will allow me to sort it, if I want
typedef struct _CDPatternFileInfo {
    uint16_t dirIndex;
    CDPatternFileType patternFileType;
    _CDPatternFileInfo *children;
    int numberOfChildren;
    _CDPatternFileInfo *parent;
    int indexInParent;
} CDPatternFileInfo;

class CWPatternSequenceManager {
private:
    CDPatternFileInfo m_rootFileInfo;
    CDPatternFileInfo *m_currentFileInfo; // May be ignored during dynamic patterns
    // TODO: maybe hold the current directory open for better perf?
#if SD_CARD_SUPPORT
    SdFat m_sd;
#endif
    
    // Current sequence information
    CDPatternItemHeader *_patternItems;
    uint32_t _numberOfPatternItems;
    // Current pattern item information
    int _currentPatternItemIndex;
    
    CDPatternItemHeader m_defaultBitmapHeader; // For loading BMP files; allows me to dynamically change the duration or the pattern or how it behaves

    uint8_t m_savedBrightness;
    
    uint32_t m_shouldRecordData:1;
    uint32_t m_shouldIgnoreButtonClickWhenTimed:1;
    uint32_t m_sdCardWorks:1;
//    uint32_t m_currentPatternItemsAreDynamic:1; // maybe not needed
    uint32_t __reserved:27;
    
    LED_PATTERNS_CLASS m_ledPatterns;
    CDOrientation m_orientation;

    uint32_t m_timedPatternStartTime; // In milliseconds; the time that all the current run of timed patterns starts, so we can accurately generate a full duration for all of them
    uint32_t m_timedUsedBeforeCurrentPattern;
//    CDWheelState m_state;
#if PATTERN_EDITOR
    NSURL *m_baseURL;
    NSURL *m_patternDirectoryURL;
#endif
    void loadSettings();
    
    bool initSDCard();
    bool initOrientation();
    bool initStrip();
    void freePatternItems();
    void freeRootFileInfo();
    
    CDPatternFileInfo *_findFirstLoadable(CDPatternFileInfo *fileInfo, bool forwards);
    CDPatternFileInfo *_findFirstLoadableChild(CDPatternFileInfo *fileInfo, bool forwards);
    CDPatternFileInfo *_findNextLoadableChild(CDPatternFileInfo *fromChild, bool forwards);
    
    void _ensureCurrentFileInfo();
    void updateLEDPatternBitmapFilename();
    void loadFileInfo(CDPatternFileInfo *fileInfo); // Divies to the appropriate method below
    void loadAsSequenceFileInfo(CDPatternFileInfo *fileInfo);
    void loadAsBitmapFileInfo(CDPatternFileInfo *fileInfo);
    
    void updateBrightness();
    const char *_getRootDirectory();
    
    
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
//    char *getFullpathName(const char *name, char *buffer, int bufferSize);
    
    void loadSequencesFromRootDirectory();
    void loadPatternFileInfoChildren(CDPatternFileInfo *parent);
    //    bool deleteSequenceAtIndex(int index); // TODO: probably take a name, and find that file to delete it...

    void loadNextDirectory();
    void loadPriorDirectory();
    
    inline bool isRootFileInfo(CDPatternFileInfo *info) { return info == &m_rootFileInfo; }
    inline bool currentFileInfoIsBitmapImage() {
        return  (m_currentFileInfo && m_currentFileInfo->patternFileType == CDPatternFileTypeBitmapImage);
    }

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
    void setCurrentSequenceAtIndex(int index); // index in the parent
    void setCurrentSequenceWithName(const char *name);
    
    bool getCardInitPassed() { return m_sdCardWorks; }
    
    
    void loadDefaultSequence();
    void loadCurrentSequence(); // loads the default sequence if there is no current one..
    
    // Playing back patterns in the current sequence
    void nextPatternItem();
    void firstPatternItem();
    void priorPatternItem();
    
    void processCommand(CDWheelCommand command);
    
    void setSingleItemPatternHeader(CDPatternItemHeader *header);
    void setDynamicPatternType(LEDPatternType type, CRGB color = CRGB::Red);
    
    void startCalibration();
    void endCalibration();
    void cancelCalibration();

    void startRecordingData();
    void endRecordingData();
    
//    CDWheelState getWheelState() { return m_state; }
    
    void process();
    
    void makeSequenceFlashColor(CRGB color);
    void flashThreeTimes(CRGB color, uint32_t delayAmount = 150);
    
//    int getNumberOfSequenceNames() { return _numberOfAvailableSequences; };
    void setLowBatteryWarning();
    inline LEDPatterns *getLEDPatterns() { return &m_ledPatterns; }
//    char *getSequenceNameAtIndex(int index) {
//        if (index >= 0 && index <= _numberOfAvailableSequences)
//            return _sequenceNames[index];
//        else
//            return NULL;
//    }

    bool getCurrentPatternFileName(char *buffer, size_t bufferSize);

    int getRootNumberOfSequenceFilenames() { return m_rootFileInfo.numberOfChildren; };
    
//    int getCurrentSequenceIndex() { return _currentSequenceIndex; }
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
    CDWheelState getWheelState() { return m_ledPatterns.isPaused() ? CDWheelStatePaused : CDWheelStatePlaying; }
    // TODO: some handler for when the wheel state changes to update bluetooth..... or maybe it just knows..

};


#endif /* defined(__LEDDigitalCyrWheel__CWPatternSequence__) */
