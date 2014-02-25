//
//  CWPatternSequence.cpp
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/5/14.
//
//

#include "CWPatternSequenceManager.h"
#include "CDLEDStripPatterns.h"

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
#if PATTERN_EDITOR
    freeSequenceNames();
#endif
    freePatternItems();

    _currentSequenceIndex = 0;
    _numberOfAvailableSequences = 0;
    
    _pixelCount = 0; // Well..whatever
    _numberOfPatternItems = CDPatternTypeMax;
    // After the header each item follows
    _patternItems = (CDPatternItemHeader *)malloc(_numberOfPatternItems * sizeof(CDPatternItemHeader));
    
    for (int i = CDPatternTypeMin; i < CDPatternTypeMax; i++) {
        _patternItems[i].patternType = (CDPatternType)i;
        _patternItems[i].durationType = CDDurationTypeSeconds;
        _patternItems[i].duration = 10;
        _patternItems[i].dataLength = 0;
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
    freePatternItems();

    bool result = true;
    ASSERT(_currentSequenceIndex >= 0 && _currentSequenceIndex < _numberOfAvailableSequences);
    const char *filename = _sequenceNames[_currentSequenceIndex];
    File sequenceFile = SD.open(filename);

    // This is reading the file format I created..
    // Header first
    CDPatternSequenceHeader patternHeader;
    sequenceFile.readBytes((char*)&patternHeader, sizeof(CDPatternSequenceHeader));

    // Verify it
    if (verifyHeader(&patternHeader)) {
        // Then read in and store the stock info
        _pixelCount = patternHeader.pixelCount;
        _numberOfPatternItems = patternHeader.patternCount;
        // After the header each item follows
        int numBytes = _numberOfPatternItems * sizeof(CDPatternItemHeader);
        _patternItems = (CDPatternItemHeader *)malloc(numBytes);
#if DEBUG
        memset(_patternItems, 0, numBytes); // shouldn't be needed
#endif
        for (int i = 0; i < _numberOfPatternItems; i++ ){
            sequenceFile.readBytes((char*)&_patternItems[i], sizeof(CDPatternItemHeader));
            // Verify it
            ASSERT(_patternItems[i].patternType >= CDPatternTypeMin && _patternItems[i].patternType < CDPatternTypeMax);
            // After the header, is the (optional) image data
            uint32_t dataLength = _patternItems[i].dataLength;
            if (dataLength > 0) {
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
    bool result = SD.begin(SS);
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

void CWPatternSequenceManager::process() {
    if (_numberOfPatternItems == 0) {
#if DEBUG
        Serial.print("No pattern items to show!");
        return;
#endif
    }
    CDPatternItemHeader *itemHeader = &_patternItems[_currentPatternItemIndex];

    uint32_t now = millis();
    uint32_t timePassed = now - _patternStartTime;
    // First see if we should go to the next pattern, then start it..
    if (itemHeader->durationType == CDDurationTypeSeconds) {
        if (timePassed >= (itemHeader->duration * 1000)) {
            nextPatternItem();
            timePassed = 0;
        }
    } else if (itemHeader->durationType == CDDurationTypeIntervals) {
        // TODO: figure out how to handle an interval for each item..as it is unique to each run..
#warning corbin figure this part out for interval support
    }
    
    stripPatternLoop(itemHeader->patternType, timePassed);
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
#if DEBUG && PATTERN_EDITOR
    NSLog(@"------ nextPatternItem -------- ");
    CDPatternItemHeader *itemHeader = &_patternItems[_currentPatternItemIndex];
    
    NSLog(@"Duration: %d seconds", itemHeader->duration);
#endif
}

