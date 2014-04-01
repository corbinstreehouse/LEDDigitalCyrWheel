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
#if DEBUG
    Serial.println("       --- loading default sequence because the name was NULL --- ");
#endif
    freePatternItems();

    _pixelCount = 300; // Well..whatever;it is too late at this point, and I don't even use it..
    _numberOfPatternItems = CDPatternTypeMax;
    
    // After the header each item follows
    _patternItems = (CDPatternItemHeader *)malloc(_numberOfPatternItems * sizeof(CDPatternItemHeader));
    
    int i = 0;
    for (int p = CDPatternTypeMin; p < CDPatternTypeMax; p++) {
        if (p == CDPatternTypeDoNothing || p == CDPatternTypeFadeIn || p == CDPatternTypeImageFade) {
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
        _patternItems[i].data = 0;
        i++;
    }
    _numberOfPatternItems = i;
#if DEBUG
    Serial.printf(" --- default pattern count _numberOfPatternItems: %d\r\n", _numberOfPatternItems);
#endif
    
}

static inline bool verifyHeader(CDPatternSequenceHeader *h) {
    // header, and version 0
    return h->marker[0] == 'S' && h->marker[1] == 'Q' && h->marker[2] == 'C' && h->version == 0;
}

void CWPatternSequenceManager::freePatternItems() {
    if (_patternItems) {
#if DEBUG
      //  Serial.println(" --- free pattern Items");
#endif
        for (int i = 0; i < _numberOfPatternItems; i++) {
            // If it has data, we have to free it
            if (_patternItems[i].dataLength && _patternItems[i].data) {
                free(_patternItems[i].data);
            }
        }
        free(_patternItems);
        _patternItems = NULL;
        _numberOfPatternItems = 0; // can be removed
    }
    
}

void CWPatternSequenceManager::loadSequenceNamed(const char *filename) {
#if DEBUG
   // Serial.printf("  loading sequence name %s\r\n:", filename);
#endif
    File sequenceFile = SD.open(filename);
    
    // This is reading the file format I created..
    // Header first
    CDPatternSequenceHeader patternHeader;
    sequenceFile.readBytes((char*)&patternHeader, sizeof(CDPatternSequenceHeader));
#if DEBUG
//    Serial.println("checking header");
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
}

bool CWPatternSequenceManager::loadCurrentSequence() {
#if DEBUG
    Serial.printf("--- loading sequence %d of %d --- ", _currentSequenceIndex, _numberOfAvailableSequences);
#endif
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
    bool result = SD.begin(SD_CARD_CS_PIN);
    int i = 0;
    while (!result) {
        result = SD.begin(SD_CARD_CS_PIN);
        i++;
        if (i == 1) {
            break; // give it 1 more chance.. (it is slow to init for some reason...)
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
//    _doOneMoreTick = false;
    
    initCompass();

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
            // One last name is for the "NULL" / default sequence
            _sequenceNames = (char **)malloc(sizeof(char*) * (_numberOfAvailableSequences + 1));
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
        } else {
            // NULL default sequence is always there...
            _sequenceNames = (char **)malloc(sizeof(char*) * 1);
        }
        
        rootDir.close();
        // Null sequecne last...
        _sequenceNames[_numberOfAvailableSequences] = NULL;
        
        _numberOfAvailableSequences++;
        
        _currentSequenceIndex = 0;
        loadCurrentSequence();
    }
    
    return result;
}

bool CWPatternSequenceManager::initCompass() {
#if ACCELEROMETER_SUPPORT
    
#if DEBUG
    Serial.println("attempting initCompass");
#endif
    bool result = _compass.init();
    int i = 0;
    while (!result && i < 2) {
        result = _compass.init(); // keep trying a few times
        i++;
    }
    Serial.println("initCompass DONE");
    if (result) {
        _compass.enableDefault();
        _compass.m_min = {  -345,   -707,   -115};
        _compass.m_max = {  +310,   +206,   +463};
#if DEBUG
        Serial.print("Compass initied type: ");
        Serial.println(_compass.getDeviceType());
#endif
    } else {
#if DEBUG
        Serial.println("Compass FAILED init");
#endif

    }
#else
    return false;
#endif
}

void CWPatternSequenceManager::loadNextSequence() {
    Serial.printf("+++ load next sequence (at: %d of %d\r\n", _currentSequenceIndex, _numberOfAvailableSequences);
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
        case CDPatternTypeRainbow:
        case CDPatternTypeLotsOfRainbows:
        case CDPatternTypeTheaterChase:
        case CDPatternTypeGradient:
        case CDPatternTypePluseGradientEffect:
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
        case CDPatternTypeImageFade:
        case CDPatternTypeWave:
            return false;
        case CDPatternTypeBottomGlow:
            return true; // Doesn't do anything
        case CDPatternTypeRotatingBottomGlow:
            return false; // repeats after a rotation
            
        case CDPatternTypeMax:
            return false;
    }
}

void CWPatternSequenceManager::process(bool initialProcess) {
    if (_numberOfPatternItems == 0) {
#if DEBUG
        Serial.println("No pattern items to show!");
#endif
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

    
#if 0
    char report[256];
    _compass.read();
    snprintf(report, sizeof(report), "A: %6d %6d %6d    M: %6d %6d %6d  head:%d",
             _compass.a.x, _compass.a.y, _compass.a.z,
             _compass.m.x, _compass.m.y, _compass.m.z, _compass.heading());
  //  Serial.println(report);

    float z  = _compass.heading((LSM303::vector<int>){1, 0, 0});
    float x  = _compass.heading((LSM303::vector<int>){0, 1, 0});
    float y  = _compass.heading((LSM303::vector<int>){0, 0, 1});
    
    Serial.printf("x: %.3f y: %.3f z: %.3f heading: %.3f deg\r\n", x, y, z, _compass.heading());
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
//    Serial.printf("--------- Next pattern Item: %d of %d\r\n", _currentPatternItemIndex, _numberOfPatternItems);
//    CDPatternItemHeader *itemHeader = &_patternItems[_currentPatternItemIndex];
//    
//    NSLog(@"Duration: %.3f seconds", itemHeader->duration/1000.0);
#endif
}

