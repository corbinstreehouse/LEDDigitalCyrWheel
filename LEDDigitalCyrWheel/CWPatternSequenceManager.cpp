//
//  CWPatternSequence.cpp
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/5/14.
//
//

#include "CWPatternSequenceManager.h"
#include "CDLEDStripPatterns.h"

#define SD_CARD_CS_PIN 4

#if DEBUG
#if PATTERN_EDITOR
    #define ASSERT(a) NSCAssert(a, @"Fail");
#else
    #define ASSERT(a) if (!(a)) { \
        Serial.print("ASSERT ");  \
        Serial.print(__FILE__); Serial.print(" : "); \
        Serial.print(__LINE__); }
#endif

#else
    #define ASSERT(a) ((void)0)
#endif


static char *getExtension(char *filename) {
    char *ext = strchr(filename, '.');
    if (ext) {
        ext++; // go past the dot...
        return ext;
    } else {
        return NULL;
    }
}

CWPatternSequenceManager::CWPatternSequenceManager() {
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
#endif

// not needed if i ever only have one instance
#if PATTERN_EDITOR
CWPatternSequenceManager::~CWPatternSequenceManager() {
    freePatternItems();
    freeSequenceNames();
}
#endif

void CWPatternSequenceManager::loadDefaultSequence() {
#if DEBUG
    Serial.println("--- loading default sequence --- ");
#endif
#if PATTERN_EDITOR
    freeSequenceNames();
#endif
    freePatternItems();

    _currentSequenceIndex = 0;
    _numberOfAvailableSequences = 0;
    _doOneMoreTick = false;
    
    _pixelCount = 0; // Well..whatever
    _numberOfPatternItems = CDPatternTypeMax;
    // After the header each item follows
    _patternItems = (CDPatternItemHeader *)malloc(_numberOfPatternItems * sizeof(CDPatternItemHeader));
    
    for (int i = CDPatternTypeMin; i < CDPatternTypeMax; i++) {
        _patternItems[i].patternType = (CDPatternType)i;
        _patternItems[i].patternEndCondition = CDPatternEndConditionOnButtonClick;
        _patternItems[i].intervalCount = 1;
        _patternItems[i].duration = 2000; //  2 seconds
        _patternItems[i].dataLength = 0;
//        _patternItems[i].color = (uint8_t)random(255) << 16 | (uint8_t)random(255) << 8 | (uint8_t)random(255); //        0xFF0000; // red
//        _patternItems[i].color = (uint8_t)random(255) << 16 | (uint8_t)random(255) << 8 | (uint8_t)random(255); //        0xFF0000; // red
        switch (random(3)) { case 0: _patternItems[i].color = 0xFF0000; break; case 1: _patternItems[i].color = 0x00FF00; break; case 2: _patternItems[i].color = 0x0000FF; break; }
        _patternItems[i].data = 0;
    }
    firstPatternItem();
}

static inline bool verifyHeader(CDPatternSequenceHeader *h) {
    // header, and version 0
    return h->marker[0] == 'S' && h->marker[1] == 'Q' && h->marker[2] == 'C' && h->version == 0;
}

void CWPatternSequenceManager::freePatternItems() {
    if (_patternItems) {
        for (int i = 0; i < _numberOfPatternItems; i++) {
            // If it has data, we have to free it
            if (_patternItems[i].dataLength && _patternItems[i].data) {
                free(_patternItems[i].data);
            }
        }
        free(_patternItems);
        _patternItems = NULL;
    }
    
}

bool CWPatternSequenceManager::loadCurrentSequence() {
#if DEBUG
    Serial.println("--- loading current sequence --- ");
#endif
    freePatternItems();

    bool result = true;
    ASSERT(_currentSequenceIndex >= 0 && _currentSequenceIndex < _numberOfAvailableSequences);
    const char *filename = _sequenceNames[_currentSequenceIndex];
#if DEBUG
    Serial.print("loading:");
    Serial.println(filename);
#endif
    File sequenceFile = SD.open(filename);

    // This is reading the file format I created..
    // Header first
    CDPatternSequenceHeader patternHeader;
    sequenceFile.readBytes((char*)&patternHeader, sizeof(CDPatternSequenceHeader));
#if DEBUG
    Serial.println("checking header");
#endif

    // Verify it
    if (verifyHeader(&patternHeader)) {
        // Then read in and store the stock info
        _pixelCount = patternHeader.pixelCount;
        _numberOfPatternItems = patternHeader.patternCount;
#if DEBUG
        Serial.printf("pixelCount: %d, now reading %d items, headerSize: %d\r\n", _pixelCount, _numberOfPatternItems, sizeof(CDPatternItemHeader));
#endif
        // After the header each item follows
        int numBytes = _numberOfPatternItems * sizeof(CDPatternItemHeader);
        _patternItems = (CDPatternItemHeader *)malloc(numBytes);
#if DEBUG
        memset(_patternItems, 0, numBytes); // shouldn't be needed
#endif
        for (int i = 0; i < _numberOfPatternItems; i++ ){
#if DEBUG
            Serial.printf("reading item %d\r\n", i);
#endif
            sequenceFile.readBytes((char*)&_patternItems[i], sizeof(CDPatternItemHeader));
#if DEBUG
            Serial.printf("Header, type: %d, duration: %d, intervalC: %d\r\n", _patternItems[i].patternType, _patternItems[i].duration, _patternItems[i].intervalCount);
#endif
            // Verify it
            ASSERT(_patternItems[i].patternType >= CDPatternTypeMin && _patternItems[i].patternType < CDPatternTypeMax);
            // After the header, is the (optional) image data
            uint32_t dataLength = _patternItems[i].dataLength;
            if (dataLength > 0) {
#if DEBUG
                Serial.printf("reading %d data\r\n", dataLength);
#endif
                // Read in the data that is following the header, and put it in the data pointer...
                _patternItems[i].data = (uint8_t *)malloc(sizeof(uint8_t) * dataLength);
                sequenceFile.readBytes((char*)&_patternItems[i].data, dataLength);
            } else {
                // data pointer should always be NULL
                _patternItems[i].data = NULL;
            }
        }
    }
    sequenceFile.close();

    firstPatternItem();
    return result;
}

bool CWPatternSequenceManager::initSDCard() {
    pinMode(SS, OUTPUT);
    bool result = SD.begin(SD_CARD_CS_PIN);
    int i = 0;
    while (!result) {
        result = SD.begin(SD_CARD_CS_PIN);
        i++;
        if (i == 5) {
            break; // give it 5 chances.. (it is slow to init for some reason...)
        }
    }
#if DEBUG
    if (!result) {
        Serial.println("SD Card initialization failed!");
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

bool CWPatternSequenceManager::init() {
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
#if DEBUG
                Serial.print("found pattern: ");
                Serial.println(filenameBuffer);
#endif
            }
        }
#if DEBUG
        Serial.printf("Found %d sequences\r\n", _numberOfAvailableSequences);
#endif
        
        if (_numberOfAvailableSequences > 0) {
            // Now we can malloc the space to save the names
            _sequenceNames = (char **)malloc(sizeof(char*) * _numberOfAvailableSequences);
            _currentSequenceIndex = 0;
            rootDir.moveToStartOfDirectory();
            while (rootDir.getNextFilename(filenameBuffer)) {
                if (isPatternFile(filenameBuffer)) {
                    // allocate and store the name so we can easily load it later. -- I include the "/" so it is the "full path". + 1 is for the null terminator, and the extra +1 is for the "/"
                    _sequenceNames[_currentSequenceIndex] = (char *)malloc(sizeof(char) * strlen(filenameBuffer) + 2);
                    _sequenceNames[_currentSequenceIndex][0] = '/';
                    char *restFilename = &_sequenceNames[_currentSequenceIndex][1];
                    strcpy(restFilename, filenameBuffer);
                    _currentSequenceIndex++;
                }
            }
        }
        _currentSequenceIndex = 0;
        loadFirstSequence();
        rootDir.close();
    }
    
    return result;
    
}

bool CWPatternSequenceManager::loadFirstSequence() {
    if (_numberOfAvailableSequences > 0) {
        if (loadCurrentSequence()) {
            return true;
        }
    }
    loadDefaultSequence();
    return false;
}

void CWPatternSequenceManager::loadNextSequence() {
    if (_numberOfAvailableSequences > 0) {
        _currentSequenceIndex++;
        if (_currentSequenceIndex >= _numberOfAvailableSequences) {
            _currentSequenceIndex = 0;
        }
        loadCurrentSequence();
    }
}

void CWPatternSequenceManager::process(bool initialProcess) {
    if (_numberOfPatternItems == 0) {
#if DEBUG
        Serial.print("No pattern items to show!");
        return;
#endif
    }
    CDPatternItemHeader *itemHeader = &_patternItems[_currentPatternItemIndex];

    uint32_t now = millis();
    // The inital tick always starts with 0
    uint32_t timePassed = initialProcess ? 0 : now - _patternStartTime;
    
    int timeLeft = itemHeader->duration - timePassed;
    if (timeLeft == 0) {
        // Hit it perfect, go to the next one
        _doOneMoreTick = false;
        _intervalCount++;
        timePassed = 0;
        _patternStartTime = now;
    } else if (timeLeft < 0) {
        if (_doOneMoreTick) {
            // We did one more tick; then go to the next one (if necessary)
            _doOneMoreTick = false;
            _intervalCount++;
            timePassed = 0;
            _patternStartTime = now;
        } else {
            // Do one more tick at the final duration to run it at 100%
            _doOneMoreTick = true;
            timePassed = itemHeader->duration;
        }
    }
    
    // See if we should go to the next pattern
    if (itemHeader->patternEndCondition == CDPatternEndConditionAfterRepeatCount) {
        if (_intervalCount >= itemHeader->intervalCount) {
            nextPatternItem();
            itemHeader = &_patternItems[_currentPatternItemIndex];
        }
    }
    
    stripPatternLoop(itemHeader, _intervalCount, timePassed, initialProcess);
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
    // This would be better suited to a common method...
    _patternStartTime = millis();
    _intervalCount = 0;
    process(true); // Initial process at time 0
#if DEBUG && PATTERN_EDITOR
//    NSLog(@"------ nextPatternItem -------- ");
//    CDPatternItemHeader *itemHeader = &_patternItems[_currentPatternItemIndex];
//    
//    NSLog(@"Duration: %.3f seconds", itemHeader->duration/1000.0);
#endif
}

