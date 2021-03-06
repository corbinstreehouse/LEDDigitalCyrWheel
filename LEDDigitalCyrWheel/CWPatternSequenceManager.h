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

#ifndef ACCELEROMETER
#define ACCELEROMETER 1
#endif

#if ACCELEROMETER

    #include "CDBaseOrientation.h"
    #if PATTERN_EDITOR
        #define ORIENTATION_CLASS CDBaseOrientation
    #else
        #include "CDPololuOrientation.h"
        #define ORIENTATION_CLASS CDPololuOrientation
    #endif

#endif

#include "LEDCommon.h"
#include "LEDPatterns.h"
#include "CDPatternSequenceManagerShared.h"

#define SEQUENCE_FILE_EXTENSION "pat"
#define BITMAP_FILE_EXTENSION "bmp"

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
#if PATTERN_EDITOR
    CDWheelChangeReasonPlayheadPositionChanged,
#endif
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
    CDPatternItemHeader *m_patternItems;
    
    uint32_t m_numberOfPatternItems;
    // Current pattern item; -1 means nothing is current at the time
    int32_t m_currentPatternItemIndex;
    
    uint8_t m_brightness;
    
    float m_bootProgress;
    
    uint32_t m_shouldRecordData:1;
    uint32_t m_shouldIgnoreButtonClickWhenTimed:1;
    uint32_t m_sdCardWorks:1;
    uint32_t m_lowBattery:1;
    uint32_t m_shouldShowBootProgress:1;
    uint32_t m_defaultShouldStretchBitmap:1;
    uint32_t m_dynamicPattern:1;
    uint32_t m_autoNextPatternItem:1;
    uint32_t __reserved:24;
    
    LED_PATTERNS_CLASS m_ledPatterns;
#if ACCELEROMETER
    ORIENTATION_CLASS m_orientation;
#endif

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
    void loadAsSequenceFromFatFile(FatFile *sequenceFile);

    void updateBrightness();
    const char *_getPatternDirectory();
    
    // for crossfade
    inline CDPatternItemHeader *getNextItemHeader() {
        ASSERT(m_currentPatternItemIndex != -1);
        int tmp = m_currentPatternItemIndex;
        tmp++;
        if (tmp >= m_numberOfPatternItems) {
            tmp = 0;
        }
        return &m_patternItems[tmp];
    }
    
    inline CDPatternItemHeader *getPreviousItemHeader() {
        ASSERT(m_currentPatternItemIndex != -1);
        int tmp = m_currentPatternItemIndex;
        tmp--;
        if (tmp < 0) {
            tmp = m_numberOfPatternItems - 1;
        }
        return &m_patternItems[tmp];
    }
    
    void loadPatternItemHeaderIntoPatterns(CDPatternItemHeader *itemHeader);

    
    void resetStartingTime();
    
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
    
    void setupBootProgress();
    void burnInitialStateInEEPROM();
    void sequenceChanged();
    
    inline void autoNextPatternItem() {
        m_autoNextPatternItem = true;
        nextPatternItem();
        m_autoNextPatternItem = false;
    }
    
    inline uint32_t tickCountInMS() {
        if (m_ledPatterns.isPaused()) {
            return m_ledPatterns.getPauseTime();
        } else {
            return millis();
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
    
    void setPlayheadPositionInMS(uint32_t position);
    uint32_t getPlayheadPositionInMS();
    
#endif
    void init(bool buttonIsDown = false);
    
    void buttonClick();
    void buttonLongClick();
    
    void reloadRootSequences();
    void loadFirstSequence();
    void loadNextSequence();
    void loadPriorSequence();
    
    void restartCurrentSequence();
    void setCurrentSequenceAtIndex(int index); // index in the parent
    
    bool getCardInitPassed() { return m_sdCardWorks; }

    const char *_getRootDirectory();
    
    void incBootProgress();
    
    void loadDefaultSequence();
    void loadCurrentSequence(); // loads the default sequence if there is no current one..
    
    // Playing back patterns in the current sequence
    void nextPatternItem();
    void firstPatternItem();
    void priorPatternItem();
    void rewind(); // Like prior, but attempts to rest the current item if needed
    
    void processCommand(CDWheelCommand command);
    
    void setSingleItemPatternHeader(CDPatternItemHeader *header);
    void setDynamicPatternType(LEDPatternType type, uint32_t patternDuration = 500, CRGB color = CRGB::Red);
    void setDynamicBitmapPatternType(const char *filename, uint32_t patternDuration, LEDBitmapPatternOptions bitmapOptions);
    bool isDynamicPattern() { return m_dynamicPattern; }
    void playSequenceWithFilename(const char *filename);
    
#if ACCELEROMETER
    void startCalibration();
    void endCalibration();
    void cancelCalibration();
    void writeOrientationData(Stream *stream);

    void startRecordingData();
    void endRecordingData();
#endif
    
    void process();
    
    void makeSequenceFlashColor(CRGB color);
    void flashThreeTimes(CRGB color, uint32_t delayAmount = 150);
    
    void setLowBatteryWarning();
    inline LEDPatterns *getLEDPatterns() { return &m_ledPatterns; }

    void getCurrentPatternFileName(char *buffer, size_t bufferSize, bool useSFN = true);

    int getRootNumberOfSequenceFilenames() { return m_rootFileInfo.numberOfChildren; };

    // TODO: only changes to the root dir..
    void changeToDirectory(const char *dir);
  
    // Hmm..need blocks/closures
    template <typename T>
    void enumerateCurrentPatternFileInfoChildren(T *objPtr, void (T::*memberPtr)(const CDPatternFileInfo *fileInfo)) {
        _ensureCurrentFileInfo();
        CDPatternFileInfo *parent = m_currentFileInfo->parent;
        if (parent) {
            for (int i = 0; i < parent->numberOfChildren; i++) {
                const CDPatternFileInfo *info = &parent->children[i];
                (objPtr->*memberPtr)(info);
            }
        }
    }
    
    CDPatternFileInfo *getCurrentPatternFileInfo() {
        _ensureCurrentFileInfo();
        return m_currentFileInfo;
    }
    
    // always Long filenames returned
    void getFilenameForPatternFileInfo(const CDPatternFileInfo *fileInfo, char *buffer, size_t bufferSize);
    
#if PATTERN_EDITOR
    uint32_t getPatternTimePassed() { return millis() - m_timedPatternStartTime - m_timedUsedBeforeCurrentPattern; };
    uint32_t getPatternTimePassedFromFirstTimedPattern() { return millis() - m_timedPatternStartTime; };
#endif

    uint32_t getNumberOfPatternItems() { return m_numberOfPatternItems; }
    int32_t getCurrentPatternItemIndex() { return m_currentPatternItemIndex; } // -1 means nothing is current
    CDPatternItemHeader *getPatternItemHeaderAtIndex(int index) { return &m_patternItems[index]; }

    void loadSequenceInMemoryFromFatFile(FatFile *sequenceFile);

    inline CDPatternItemHeader *getCurrentItemHeader() {
        if (m_currentPatternItemIndex >= 0 && m_currentPatternItemIndex < m_numberOfPatternItems) {
            return &m_patternItems[m_currentPatternItemIndex];
        } else {
            return NULL;
        }
    }
    
    bool currentPatternIsPOV() {
        CDPatternItemHeader *h = getCurrentItemHeader();
        return h && h->patternOptions.bitmapOptions.pov && (h->patternType == LEDPatternTypeBitmap || h->patternType == LEDPatternTypeImageReferencedBitmap);
    }

    void play(); 
    void pause();
    inline bool isPaused() { return m_ledPatterns.isPaused(); }
    CDWheelState getWheelState();

    void setWheelChangeHandler(CDWheelChangedHandler *handler, void *data) { m_changeHandler = handler; m_changeHandlerData = data; }
    
    
    uint8_t getBrightness() { return m_brightness; }
    void setBrightness(uint8_t brightness);
    
    // In ms..
    uint32_t getCurrentPatternSpeed();
    void setCurrentPatternSpeed(uint32_t speedInMs);
    void setCurrentPatternColor(CRGB color);
    void setCurrentPattenShouldSetBrightnessByRotationalVelocity(bool value);
    void setCurrentPattenOptions(LEDPatternOptions options);
    
    bool shouldShowBootProgress() { return m_shouldShowBootProgress; }
    void setShouldShowBootProgress(bool value);
    
    uint16_t getFPS() { return FastLED.getFPS(); }
    
    void forceShow() { m_ledPatterns.forceShow(); }

};

extern char *getExtension(char *filename);

#endif /* defined(__LEDDigitalCyrWheel__CWPatternSequence__) */
