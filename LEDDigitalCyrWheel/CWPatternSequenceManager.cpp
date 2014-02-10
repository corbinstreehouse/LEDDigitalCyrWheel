//
//  CWPatternSequence.cpp
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/5/14.
//
//

#include "CWPatternSequenceManager.h"

#define DEBUG 1

#if DEBUG
#define ASSERT(a) if (!(a)) { \
    Serial.print("ASSERT ");  \
    Serial.print(__FILE__); Serial.print(" : "); \
    Serial.print(__LINE__); }
#else
#define ASSERT(a)
#endif

const int g_SDCardChipSelect = SS;

char *getExtension(char *filename) {
    char *ext = strchr(filename, '.');
    if (ext) {
        ext++; // go past the dot...
        return ext;
    } else {
        return NULL;
    }
}

void CWPatternSequenceManager::loadDefaultSequence() {
    
    // TODO: corbin code
}

void CWPatternSequenceManager::loadCurrentSequence() {
    ASSERT(_currentSequenceIndex < _numberOfAvailableSequences);
    const char *filename = _sequenceNames[_currentSequenceIndex];
    File sequenceFile = SD.open(filename);
//    sequenceFile.re
    
    
    sequenceFile.close();
}


bool CWPatternSequenceManager::initSDCard() {
    pinMode(g_SDCardChipSelect, OUTPUT);
    bool result = SD.begin(g_SDCardChipSelect);
#if DEBUG
    if (!result) {
        Serial.println("SD Card initialization failed!");
#endif
    }
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
        rootDir.close();
    }
    
    return result;
    
}

bool CWPatternSequenceManager::loadFirstSequence() {
    if (_numberOfAvailableSequences > 0) {
        _currentSequenceIndex = 0;
        loadCurrentSequence();
        return true;
    } else {
        loadDefaultSequence();
        return false;
    }
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

