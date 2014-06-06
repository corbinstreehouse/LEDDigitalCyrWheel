//
//  CWPatternSequence.cpp
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/5/14.
//
//

#include "CWPatternSequenceManager.h"
#include "LEDPatterns.h"
#include "LEDDigitalCyrWheel.h"
#include "LEDCommon.h"

#define RECORD_INDICATOR_FILENAME "RECORD.TXT" // If this file exists, we record data in other files.

static char *getExtension(char *filename) {
    char *ext = strchr(filename, '.');
    if (ext) {
        ext++; // go past the dot...
        return ext;
    } else {
        return NULL;
    }
}

CWPatternSequenceManager::CWPatternSequenceManager() : m_ledPatterns(STRIP_LENGTH) {
    _patternItems = NULL;
    _sequenceNames = NULL;
}

#if PATTERN_EDITOR
void CWPatternSequenceManager::freeSequenceNames() {
    if (_sequenceNames) {
        for (int i = 0; i < _numberOfAvailableSequences; i++) {
            free(_sequenceNames[i]);
        }
        free(_sequenceNames);
        _sequenceNames = NULL;
    }
}

CWPatternSequenceManager::~CWPatternSequenceManager() {
    freePatternItems();
    freeSequenceNames();
}
    
void CWPatternSequenceManager::setCyrWheelView(CDCyrWheelView *view) {
    m_ledPatterns.setCyrWheelView(view);
}


#endif

void CWPatternSequenceManager::loadDefaultSequence() {
    DEBUG_PRINTLN("       --- loading default sequence because the name was NULL --- ");
    freePatternItems();

    _pixelCount = 300; // Well..whatever;it is too late at this point, and I don't even use it..
    _numberOfPatternItems = LEDPatternTypeMax;
    
    // After the header each item follows
    _patternItems = (CDPatternItemHeader *)malloc(_numberOfPatternItems * sizeof(CDPatternItemHeader));
    
    int i = 0;
    for (int p = LEDPatternTypeMin; p < LEDPatternTypeMax; p++) {
        if (p == LEDPatternTypeDoNothing || p == LEDPatternTypeFadeIn || p == LEDPatternTypeImageLinearFade || p == LEDPatternTypeImageEntireStrip) {
            continue; // skip a few
        }
        _patternItems[i].patternType = (LEDPatternType)p;
        _patternItems[i].patternEndCondition = CDPatternEndConditionOnButtonClick;
        _patternItems[i].intervalCount = 1;
        _patternItems[i].duration = 2000; //  2 seconds
        _patternItems[i].dataLength = 0;
        _patternItems[i].shouldSetBrightnessByRotationalVelocity = 0; // for testing..
//        _patternItems[i].color = (uint8_t)random(255) << 16 | (uint8_t)random(255) << 8 | (uint8_t)random(255); //        0xFF0000; // red
//        _patternItems[i].color = (uint8_t)random(255) << 16 | (uint8_t)random(255) << 8 | (uint8_t)random(255); //        0xFF0000; // red
        switch (random(3)) { case 0: _patternItems[i].color = 0xFF0000; break; case 1: _patternItems[i].color = 0x00FF00; break; case 2: _patternItems[i].color = 0x0000FF; break; }
        _patternItems[i].dataOffset = 0;
        i++;
    }
    _numberOfPatternItems = i;
    DEBUG_PRINTF(" --- default pattern count _numberOfPatternItems: %d\r\n", _numberOfPatternItems);
    
}

static inline bool verifyHeader(CDPatternSequenceHeader *h) {
    // header, and version 0
    DEBUG_PRINTF("checking header, v: %d , expected :%d\r\n", h->version, SEQUENCE_VERSION);
    return h->marker[0] == 'S' && h->marker[1] == 'Q' && h->marker[2] == 'C';
}

void CWPatternSequenceManager::freePatternItems() {
    if (_patternItems) {
        DEBUG_PRINTLN(" --- free pattern Items");
//        for (int i = 0; i < _numberOfPatternItems; i++) {
//            // If it has data, we have to free it
//            if (_patternItems[i].dataLength && _patternItems[i].data) {
//                free(_patternItems[i].data);
//            }
//        }
        free(_patternItems);
        _patternItems = NULL;
        _numberOfPatternItems = 0; // can be removed
    }
    
}

void CWPatternSequenceManager::makeSequenceFlashColor(uint32_t color) {
    _numberOfPatternItems = 2;
    _patternItems = (CDPatternItemHeader *)malloc(sizeof(CDPatternItemHeader) * _numberOfPatternItems);
    _patternItems[0].patternType = LEDPatternTypeSolidColor;
    _patternItems[0].duration = 500;
    _patternItems[0].color = color; // TODO: better representation of colors.. this is yellow..
    _patternItems[0].intervalCount = 1;
    _patternItems[0].patternEndCondition = CDPatternEndConditionAfterRepeatCount;
    
    _patternItems[1].patternType = LEDPatternTypeSolidColor;
    _patternItems[1].duration = 500;
    _patternItems[1].color = 0x000000; // TODO: better representation of colors..
    _patternItems[1].intervalCount = 1;
    _patternItems[1].patternEndCondition = CDPatternEndConditionAfterRepeatCount;
}

void CWPatternSequenceManager::loadSequenceNamed(const char *filename) {
    DEBUG_PRINTF("  loading sequence name: %s\r\n", filename);
    File sequenceFile = SD.open(filename);
    DEBUG_PRINTF(" OPENED file: %s\r\n", sequenceFile.name());
    if (!sequenceFile.available()) {
        // Try again???
        sequenceFile = SD.open(filename);
        DEBUG_PRINTF(" try again... file: %s\r\n", sequenceFile.name());
    }
    
    // This is reading the file format I created..
    // Header first
    CDPatternSequenceHeader patternHeader;
    if (sequenceFile.available()) {
        sequenceFile.readBytes((char*)&patternHeader, sizeof(CDPatternSequenceHeader));
    } else {
        patternHeader.version = 0; // Fail
    }
    
    // Verify it
    if (verifyHeader(&patternHeader)) {
        // Verify the version
        if (patternHeader.version == SEQUENCE_VERSION) {
            // Free existing stuff
            if (_patternItems) {
                DEBUG_PRINTLN("ERROR: PATTERN ITEMS SHOULD BE FREE!!!!");
            }
                // Then read in and store the stock info
            _pixelCount = patternHeader.pixelCount;
            _numberOfPatternItems = patternHeader.patternCount;
            DEBUG_PRINTF("pixelCount: %d, now reading %d items, headerSize: %d\r\n", _pixelCount, _numberOfPatternItems, sizeof(CDPatternItemHeader));
            
            // After the header each item follows
            int numBytes = _numberOfPatternItems * sizeof(CDPatternItemHeader);
            _patternItems = (CDPatternItemHeader *)malloc(numBytes);
            memset(_patternItems, 0, numBytes); // shouldn't be needed, but do it anyways

            for (int i = 0; i < _numberOfPatternItems; i++ ){
                DEBUG_PRINTF("reading item %d\r\n", i);
                sequenceFile.readBytes((char*)&_patternItems[i], sizeof(CDPatternItemHeader));
                DEBUG_PRINTF("Header, type: %d, duration: %d, intervalC: %d\r\n", _patternItems[i].patternType, _patternItems[i].duration, _patternItems[i].intervalCount);
                // Verify it
                ASSERT(_patternItems[i].patternType >= LEDPatternTypeMin && _patternItems[i].patternType < LEDPatternTypeMax);
                ASSERT(_patternItems[i].duration > 0);
                // After the header, is the (optional) image data
                uint32_t dataLength = _patternItems[i].dataLength;
                if (dataLength > 0) {
                    DEBUG_PRINTF("we have %d data\r\n", dataLength);
                    // Read in the data that is following the header, and put it in the data pointer...
                    // 65536 kb of ram..more than 20,000 pixels would overflow...which i'm now hitting w/larger images. darn it..i have to chunk these and dynamically load each one ;(
                    _patternItems[i].dataOffset = sequenceFile.position();
                    _patternItems[i].dataFilename = filename;
    //                _patternItems[i].data = (uint8_t *)malloc(sizeof(uint8_t) * dataLength);
    //                sequenceFile.readBytes((char*)_patternItems[i].data, dataLength);
                    // seek past the data
                    sequenceFile.seek(sequenceFile.position() + dataLength);
                } else {
                    // data pointer should always be NULL
                    _patternItems[i].dataOffset = 0;
                    _patternItems[i].dataFilename = NULL;
                }
            }
            DEBUG_PRINTLN("DONE");
        } else {
            // We don't support this version... flash purple
            makeSequenceFlashColor(0xFF00FF);
        }
    } else {
        // Bad data...flash yellow
        makeSequenceFlashColor(0xFFFF00);
    }
    sequenceFile.close();
}

bool CWPatternSequenceManager::loadCurrentSequence() {
    DEBUG_PRINTF("--- loading sequence %d of %d --- \r\n", _currentSequenceIndex, _numberOfAvailableSequences);
    freePatternItems();

    bool result = true;
    ASSERT(_currentSequenceIndex >= 0 && _currentSequenceIndex < _numberOfAvailableSequences);
    const char *filename = _sequenceNames[_currentSequenceIndex];
    if (filename == NULL) {
        // Load the default
        loadDefaultSequence();
    } else {
        loadSequenceNamed(filename);
    }

    firstPatternItem();
    return result;
}

bool CWPatternSequenceManager::initSDCard() {
    pinMode(SS, OUTPUT);
    bool result = SD.begin(SPI_FULL_SPEED, SD_CARD_CS_PIN);
    int i = 0;
    while (!result) {
        result = SD.begin(SPI_HALF_SPEED, SD_CARD_CS_PIN);
        i++;
        if (i == 1) {
            break; // give it 3 more chances??.. (it is slow to init for some reason...)
        }
    }
#if DEBUG
    if (result) {
        DEBUG_PRINTLN("SD Card Initialized");
    } else {
        DEBUG_PRINTLN("SD Card initialization failed!");
    }
    
#endif
    return result;
}

static inline bool isPatternFile(char *filename) {
    char *ext = getExtension(filename);
    if (ext) {
        if (strcmp(ext, "PAT") == 0 || strcmp(ext, "pat") == 0) {
            return true;
        }
    }
    return false;
}

bool CWPatternSequenceManager::initStrip() {
    m_savedBrightness = 128; // Default value?? this is still super bright. maybe the algorithm is wrong..
    m_ledPatterns.begin();
    m_ledPatterns.setBrightness(m_savedBrightness);
    return true;
}

bool CWPatternSequenceManager::init(bool buttonIsDown) {
    DEBUG_PRINTLN("::init");
    _shouldRecordData = false;

    delay(2); // without this, things don't always init....

    initOrientation();
    initStrip();
    
    bool result = initSDCard();

    if (result) {
        // Load up the names of available patterns
        File rootDir = SD.open("/");
        _numberOfAvailableSequences = 0;
        // Loop twice; first time count, second time allocate and store
        char filenameBuffer[PATH_COMPONENT_BUFFER_LEN];
        while (rootDir.getNextFilename(filenameBuffer)) {
            if (isPatternFile(filenameBuffer)) {
                _numberOfAvailableSequences++;
                DEBUG_PRINTF("found pattern: %s\r\n", filenameBuffer);
            } else if (strcmp(filenameBuffer, RECORD_INDICATOR_FILENAME) == 0) {
                _shouldRecordData = true;
            }
        }
        DEBUG_PRINTF("Found %d sequences\r\n", _numberOfAvailableSequences);
        
        if (_numberOfAvailableSequences > 0) {
            // Now we can malloc the space to save the names
            // One last name is for the "NULL" / default sequence
            _sequenceNames = (char **)malloc(sizeof(char*) * (_numberOfAvailableSequences + 1));
            _currentSequenceIndex = 0;
            rootDir.moveToStartOfDirectory();
            while (rootDir.getNextFilename(filenameBuffer)) {
                if (isPatternFile(filenameBuffer)) {
                    // allocate and store the name so we can easily load it later. -- I include the "/" so it is the "full path". + 1 is for the null terminator, and the extra +1 is for the "/"
                    char *mallocedName = (char *)malloc(sizeof(char) * strlen(filenameBuffer) + 2);
                    _sequenceNames[_currentSequenceIndex] = mallocedName;

                    mallocedName[0] = '/';
                    mallocedName++;
                    strcpy(mallocedName, filenameBuffer);
                    DEBUG_PRINTF("copied name: %s len: %d\r\n", _sequenceNames[_currentSequenceIndex], strlen(_sequenceNames[_currentSequenceIndex]));
                    
                    _currentSequenceIndex++;
                }
            }
        } else {
            // NULL default sequence is always there...
            _sequenceNames = (char **)malloc(sizeof(char*) * 1);
        }
        
        rootDir.close();
        // Null sequecne last...
        _sequenceNames[_numberOfAvailableSequences] = NULL;
        
        _numberOfAvailableSequences++;
        
        if (_shouldRecordData) {
            m_ledPatterns.flashThreeTimesWithDelay(CRGB(30,30,30), 150);
        }
        
        // TODO: Read the last sequence we were on to start from there again...but make this optional...
        
    } else {
        _numberOfAvailableSequences = 1;
        _sequenceNames = (char **)malloc(sizeof(char*) * 1);
        _sequenceNames[0] = NULL;
        
    }
    _currentSequenceIndex = 0;
    loadCurrentSequence();
    
    if (buttonIsDown) {
        // Go into calibration mode for the accell
        m_orientation.beginCalibration();
        // Override the default sequence to blink
        m_ledPatterns.setDuration(300);
        m_ledPatterns.setPatternColor(CRGB::Pink);
        m_ledPatterns.setPatternType(LEDPatternTypeBlink);
    }
    
    return result;
}

bool CWPatternSequenceManager::initOrientation() {
    DEBUG_PRINTLN("init orientation");
    bool result = m_orientation.init(); // TODO: return if it failed???
    
    DEBUG_PRINTLN("DONE init orientation");
    
    return result;
}

void CWPatternSequenceManager::loadNextSequence() {
    DEBUG_PRINTF("+++ load next sequence (at: %d of %d\r\n", _currentSequenceIndex, _numberOfAvailableSequences);
    
    if (_numberOfAvailableSequences > 0) {
        _currentSequenceIndex++;
        if (_currentSequenceIndex >= _numberOfAvailableSequences) {
            _currentSequenceIndex = 0;
        }
        loadCurrentSequence();
    }
}

void CWPatternSequenceManager::buttonClick() {
    if (m_orientation.isCalibrating()) {
        m_orientation.endCalibration();
        firstPatternItem();
    } else {
        nextPatternItem();
    }
}

void CWPatternSequenceManager::buttonLongClick() {
    if (_shouldRecordData) {
        if (m_orientation.isSavingData()) {
            m_ledPatterns.flashThreeTimesWithDelay(CRGB::Blue, 150);
            m_orientation.endSavingData();
        } else {
            // Flash green to let me know
            m_ledPatterns.flashThreeTimesWithDelay(CRGB::Green, 150);
            m_orientation.beginSavingData();
        }
    } else {
        loadNextSequence();
    }
}

void CWPatternSequenceManager::process() {
    m_orientation.process();
    if (m_orientation.isCalibrating()) {
        // Show the flashing, and return
        m_ledPatterns.show();
        return;
    }
    
    if (_numberOfPatternItems == 0) {
        DEBUG_PRINTLN("No pattern items to show!");
        m_ledPatterns.flashThreeTimesWithDelay(CRGB::Yellow, 150);
        delay(1000);
        return;
    }
    
    updateBrightness();
    
    // First, always do one pass through the update of the patterns; this updates the time
    m_ledPatterns.show();
    
    // This updates how much time has passed since the pattern started and how many full intervals have run through.
    // See if we should go to the next pattern
    CDPatternItemHeader *itemHeader = &_patternItems[_currentPatternItemIndex];
    // See if we should go to the next pattern. Interval counts are 1 based.
    if (itemHeader->patternEndCondition == CDPatternEndConditionAfterRepeatCount) {
        if (m_ledPatterns.getIntervalCount() >= itemHeader->intervalCount) {
            nextPatternItem();
            itemHeader = &_patternItems[_currentPatternItemIndex];
            // We will do at least one show of the pattern startig on the next loop
        }
    } else {
        // It is waiting for some other condition before going to the next pattern (button click is the only condition now)
    }
}


void CWPatternSequenceManager::updateBrightness() {
    CDPatternItemHeader *itemHeader = getCurrentItemHeader();
    if (itemHeader->shouldSetBrightnessByRotationalVelocity) {
        uint8_t brightness = m_orientation.getRotationalVelocityBrightness(m_ledPatterns.getBrightness());
        m_ledPatterns.setBrightness(brightness);
    } else {
        m_ledPatterns.setBrightness(m_savedBrightness);
    }
}

void CWPatternSequenceManager::setLowBatteryWarning() {
    m_savedBrightness = 64; // Lower brightness so we can see it get low! it is updated on the update pass
}

void CWPatternSequenceManager::firstPatternItem() {
    _currentPatternItemIndex = -1;
    nextPatternItem();
}

void CWPatternSequenceManager::nextPatternItem() {
    _currentPatternItemIndex++;
    if (_currentPatternItemIndex >= _numberOfPatternItems) {
        _currentPatternItemIndex = 0;
    }
    
    CDPatternItemHeader *itemHeader = getCurrentItemHeader();
    // Reset the stuff based on the new header
    m_ledPatterns.setPatternType(itemHeader->patternType);
    m_ledPatterns.setDuration(itemHeader->duration);
    m_ledPatterns.setPatternColor(itemHeader->color);
        
    m_orientation.setFirstPass(true); // why do I need this??

#if DEBUG
//    DEBUG_PRINTF("--------- Next pattern Item: %d of %d\r\n", _currentPatternItemIndex, _numberOfPatternItems);
//    CDPatternItemHeader *itemHeader = &_patternItems[_currentPatternItemIndex];
//    
//    NSLog(@"Duration: %.3f seconds", itemHeader->duration/1000.0);
#endif
}

