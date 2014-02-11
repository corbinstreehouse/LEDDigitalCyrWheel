//
//  CWPatternSequence.cpp
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/5/14.
//
//

#include "CWPatternSequenceManager.h"

#define DEBUG 0

#if DEBUG
#define ASSERT(a) if (!(a)) { \
    Serial.print("ASSERT ");  \
    Serial.print(__FILE__); Serial.print(" : "); \
    Serial.print(__LINE__); }
#else
#define ASSERT(a) ((void)0)
#endif

char *getExtension(char *filename) {
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


void CWPatternSequenceManager::freeSequenceNames() {
#if PATTERN_EDITOR
    if (_sequenceNames) {
        for (int i = 0; i < _numberOfAvailableSequences; i++) {
            free(_sequenceNames[i]);
        }
        free(_sequenceNames);
        _sequenceNames = NULL;
    }
#endif
}

// not needed if i ever only have one instance
#if PATTERN_EDITOR
CWPatternSequenceManager::~CWPatternSequenceManager() {
    freePatternItems();
    freeSequenceNames();
}
#endif

void CWPatternSequenceManager::loadDefaultSequence() {
    freeSequenceNames();
    freePatternItems();

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
    ASSERT(_currentSequenceIndex < _numberOfAvailableSequences);
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
        _patternItems = (CDPatternItemHeader *)malloc(_numberOfPatternItems * sizeof(CDPatternItemHeader));
        for (int i = 0; i < _numberOfPatternItems; i++ ){
            sequenceFile.readBytes((char*)&_patternItems[i], sizeof(CDPatternItemHeader));
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

bool CWPatternSequenceManager::init() {
    bool result = initSDCard();

    if (result) {
        // Load up the names of available patterns
        File rootDir = SD.open("/");
        _numberOfAvailableSequences = 0;
        // Loop twice; first time count, second time allocate and store
        char filenameBuffer[PATH_COMPONENT_BUFFER_LEN];
        while (rootDir.getNextFilename(filenameBuffer)) {
            char *ext = getExtension(filenameBuffer);
            if (ext) {
                if (strcmp(ext, "PAT") == 0 || strcmp(ext, "pat") == 0) {
                    _numberOfAvailableSequences++;
#if DEBUG
                    Serial.print("found pattern: ");
                    Serial.println(filenameBuffer);
#endif
                }
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
                char *ext = getExtension(filenameBuffer);
                if (ext && (strcmp(ext, "PAT") == 0 || strcmp(ext, "pat") == 0)) {
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
        _currentSequenceIndex = 0;
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

