//
//  CWPatternSequence.cpp
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/5/14.
//
//

#include "CWPatternSequenceManager.h"
#include "CDLEDStripPatterns.h"
#include "LEDDigitalCyrWheel.h"

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
    DEBUG_PRINTLN("       --- loading default sequence because the name was NULL --- ");
    freePatternItems();

    _pixelCount = 300; // Well..whatever;it is too late at this point, and I don't even use it..
    _numberOfPatternItems = CDPatternTypeMax;
    
    // After the header each item follows
    _patternItems = (CDPatternItemHeader *)malloc(_numberOfPatternItems * sizeof(CDPatternItemHeader));
    
    int i = 0;
    for (int p = CDPatternTypeMin; p < CDPatternTypeMax; p++) {
        if (p == CDPatternTypeDoNothing || p == CDPatternTypeFadeIn || p == CDPatternTypeImageLinearFade || p == CDPatternTypeImageEntireStrip) {
            continue; // skip a few
        }
        _patternItems[i].patternType = (CDPatternType)p;
        _patternItems[i].patternEndCondition = CDPatternEndConditionOnButtonClick;
        _patternItems[i].intervalCount = 1;
        _patternItems[i].duration = 2000; //  2 seconds
        _patternItems[i].dataLength = 0;
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
    return h->marker[0] == 'S' && h->marker[1] == 'Q' && h->marker[2] == 'C' && h->version == SEQUENCE_VERSION;
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
            ASSERT(_patternItems[i].patternType >= CDPatternTypeMin && _patternItems[i].patternType < CDPatternTypeMax);
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
        // Bad data...flash yellow
        _numberOfPatternItems = 2;
        _patternItems = (CDPatternItemHeader *)malloc(sizeof(CDPatternItemHeader) * _numberOfPatternItems);
        _patternItems[0].patternType = CDPatternTypeSolidColor;
        _patternItems[0].duration = 500;
        _patternItems[0].color = 0xFFFF00; // TODO: better representation of colors.. this is yellow..
        _patternItems[0].intervalCount = 1;
        _patternItems[0].patternEndCondition = CDPatternEndConditionAfterRepeatCount;
        
        _patternItems[1].patternType = CDPatternTypeSolidColor;
        _patternItems[1].duration = 500;
        _patternItems[1].color = 0x000000; // TODO: better representation of colors..
        _patternItems[1].intervalCount = 1;
        _patternItems[1].patternEndCondition = CDPatternEndConditionAfterRepeatCount;
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

bool CWPatternSequenceManager::init() {
    DEBUG_PRINTLN("::init");

    initOrientation();

    delay(2); // why do I have this delay in here?
    
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
    } else {
        _numberOfAvailableSequences = 1;
        _sequenceNames = (char **)malloc(sizeof(char*) * 1);
        _sequenceNames[0] = NULL;
        
    }
    _currentSequenceIndex = 0;
    loadCurrentSequence();
    
    return result;
}

bool CWPatternSequenceManager::initOrientation() {
#if ACCELEROMETER_SUPPORT

    DEBUG_PRINTLN("init orientation");
    bool result = _orientation.init(); // TODO: return if it failed???
    DEBUG_PRINTLN("DONE init orientation");
    
    return result;
#else
    return true;
#endif
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

static inline bool PatternIsContinuous(CDPatternType p) {
    switch (p) {
        case CDPatternTypeRotatingRainbow:
        case CDPatternTypeRotatingMiniRainbows:
        case CDPatternTypeTheaterChase:
        case CDPatternTypeGradient:
        case CDPatternTypePluseGradientEffect:
        case CDPatternTypeSolidRainbow:
        case CDPatternTypeRainbowWithSpaces:
        case CDPatternTypeRandomGradients:
            return true;
        case CDPatternTypeWarmWhiteShimmer:
        case CDPatternTypeRandomColorWalk:
        case CDPatternTypeTraditionalColors:
        case CDPatternTypeColorExplosion:
        case CDPatternTypeRWGradient:
        case CDPatternTypeWhiteBrightTwinkle:
        case CDPatternTypeWhiteRedBrightTwinkle:
        case CDPatternTypeRedGreenBrightTwinkle:
        case CDPatternTypeColorTwinkle:
            return false; // i think
        case CDPatternTypeCollision:
            return false;
        case CDPatternTypeFadeOut:
        case CDPatternTypeFadeIn:
        case CDPatternTypeColorWipe:
            return false;
            
        case CDPatternTypeDoNothing:
            return true;
        case CDPatternTypeImageLinearFade:
        case CDPatternTypeWave:
            return false;
        case CDPatternTypeImageEntireStrip:
            return true;
        case CDPatternTypeBottomGlow:
            return true; // Doesn't do anything
        case CDPatternTypeRotatingBottomGlow:
        case CDPatternTypeSolidColor:
            return false; // repeats after a rotation
        case CDPatternTypeMax:
            return false;
    }
}

void CWPatternSequenceManager::process(bool initialProcess) {
    if (_numberOfPatternItems == 0) {
        DEBUG_PRINTLN("No pattern items to show!");
        flashThreeTimes(255, 255, 0, 150);
        delay(1000);
        return;
    }
    CDPatternItemHeader *itemHeader = &_patternItems[_currentPatternItemIndex];
    
    uint32_t now = millis();
    // The inital tick always starts with 0
    uint32_t timePassed = initialProcess ? 0 : now - _patternStartTime;
    
    // How many intervals have passed?
    int intervalCount = timePassed / itemHeader->duration;
    if (!PatternIsContinuous(itemHeader->patternType)) {
        // Since it isn't continuous, modify the timePassed here
        if (intervalCount > 0) {
            timePassed = timePassed - intervalCount*itemHeader->duration;
        }
    }

    // See if we should go to the next pattern
    if (itemHeader->patternEndCondition == CDPatternEndConditionAfterRepeatCount) {
        if (intervalCount >= itemHeader->intervalCount) {
            nextPatternItem();
            itemHeader = &_patternItems[_currentPatternItemIndex];
            // reset the state I pass to the loop pattern
            timePassed = 0;
            intervalCount = 0;
            initialProcess = true;
        }
    }
    
#if ACCELEROMETER_SUPPORT
    _orientation.process();

#if DEBUG
//    _orientation.print();
#endif
    
#endif
    
    stripPatternLoop(itemHeader, intervalCount, timePassed, initialProcess);
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
//    _intervalCount = 0;
    process(true); // Initial process at time 0
#if DEBUG 
//    DEBUG_PRINTF("--------- Next pattern Item: %d of %d\r\n", _currentPatternItemIndex, _numberOfPatternItems);
//    CDPatternItemHeader *itemHeader = &_patternItems[_currentPatternItemIndex];
//    
//    NSLog(@"Duration: %.3f seconds", itemHeader->duration/1000.0);
#endif
}

