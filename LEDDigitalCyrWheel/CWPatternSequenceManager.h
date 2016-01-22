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
    #include "CyrWheelLEDPatterns.h"
    #define LED_PATTERNS_CLASS CyrWheelLEDPatterns
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

typedef enum {
    CDWheelChangeReasonStateChanged,
    CDWheelChangeReasonBrightnessChanged,
    CDWheelChangeReasonPatternChanged,
    CDWheelChangeReasonSequenceChanged,
} CDWheelChangeReason;

typedef void (CDWheelChangedHandler)(CDWheelChangeReason changeReason, void *data);

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
    
    uint8_t m_brightness;
    
    uint32_t m_shouldRecordData:1;
    uint32_t m_shouldIgnoreButtonClickWhenTimed:1;
    uint32_t m_sdCardWorks:1;
    uint32_t m_lowBattery:1;
    uint32_t __reserved:27;
    
    LED_PATTERNS_CLASS m_ledPatterns;
    CDOrientation m_orientation;

    uint32_t m_timedPatternStartTime; // In milliseconds; the time that all the current run of timed patterns starts, so we can accurately generate a full duration for all of them
    uint32_t m_timedUsedBeforeCurrentPattern;
#if PATTERN_EDITOR
    NSURL *m_baseURL;
    NSURL *m_patternDirectoryURL;
#endif
    
    
    CDWheelChangedHandler *m_changeHandler;
    void *m_changeHandlerData;
    
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
    void loadFileInfo(CDPatternFileInfo *fileInfo); // Divies to the appropriate method below
    void loadAsSequenceFileInfo(CDPatternFileInfo *fileInfo);
    void loadAsBitmapFileInfo(CDPatternFileInfo *fileInfo);
    
    void updateBrightness();
    const char *_getRootDirectory();
    const char *_getPatternDirectory();
    
    
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
    void makePatternsBlinkColor(CRGB color);
    
    
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
    
    inline void sendWheelChanged(CDWheelChangeReason reason) {
        if (m_changeHandler != NULL) {
            (*m_changeHandler)(reason, m_changeHandlerData);
        }
    }

public:
    CWPatternSequenceManager();
#if PATTERN_EDITOR
    ~CWPatternSequenceManager();
    void setCyrWheelView(CDCyrWheelView *view); // Binding..
    void setBaseURL(NSURL *url);
    NSURL *getBaseURL() {return m_baseURL; }
    void setPatternDirectoryURL(NSURL *url) { m_patternDirectoryURL = url; }
#endif
    void init();
    
    void buttonClick();
    void buttonLongClick();
    
    void loadFirstSequence();
    void loadNextSequence();
    void loadPriorSequence();
    
    void restartCurrentSequence();
    void setCurrentSequenceAtIndex(int index); // index in the parent
//    void setCurrentSequenceWithName(const char *name); // broken..
    
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
    
    void process();
    
    void makeSequenceFlashColor(CRGB color);
    void flashThreeTimes(CRGB color, uint32_t delayAmount = 150);
    
    void setLowBatteryWarning();
    inline LEDPatterns *getLEDPatterns() { return &m_ledPatterns; }

    bool getCurrentPatternFileName(char *buffer, size_t bufferSize);

    int getRootNumberOfSequenceFilenames() { return m_rootFileInfo.numberOfChildren; };
    
    uint32_t getPatternTimePassed() { return millis() - m_timedPatternStartTime - m_timedUsedBeforeCurrentPattern; };
    uint32_t getPatternTimePassedFromFirstTimedPattern() { return millis() - m_timedPatternStartTime; };

    uint32_t getNumberOfPatternItems() { return _numberOfPatternItems; }
    uint32_t getCurrentPatternItemIndex() { return _currentPatternItemIndex; }
    CDPatternItemHeader *getPatternItemHeaderAtIndex(int index) { return &_patternItems[index]; }

#if PATTERN_EDITOR
    CDPatternItemHeader *getCurrentPatternItemHeader() {
        if (_currentPatternItemIndex >= 0 && _currentPatternItemIndex < _numberOfPatternItems) {
            return &_patternItems[_currentPatternItemIndex];
        } else {
            return NULL;
        }
    }
#endif
    
    void play() { m_ledPatterns.play(); sendWheelChanged(CDWheelChangeReasonStateChanged); }
    void pause() { m_ledPatterns.pause(); sendWheelChanged(CDWheelChangeReasonStateChanged); }
    bool isPaused() { return m_ledPatterns.isPaused(); }
    CDWheelState getWheelState() { return m_ledPatterns.isPaused() ? CDWheelStatePaused : CDWheelStatePlaying; }

    void setWheelChangeHandler(CDWheelChangedHandler *handler, void *data) { m_changeHandler = handler; m_changeHandlerData = data; }
    
    
    uint8_t getBrightness() { return m_brightness; }
    void setBrightness(uint8_t brightness);

};


#endif /* defined(__LEDDigitalCyrWheel__CWPatternSequence__) */
