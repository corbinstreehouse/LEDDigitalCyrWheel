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

#if DEBUG
    #warning "DEBUG Code is on!!"
#endif


static char *g_defaultFilename = "DEFAULT"; // We compare to this address to know if it is the default pattern
static const char *g_sequencePath = "/";

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
    m_shouldIgnoreButtonClickWhenTimed = true; // TODO: make this an option per sequence...
    ASSERT(sizeof(CDPatternItemHeader) == PATTERN_HEADER_SIZE_v0); // make sure I don't screw stuff up by chaning the size and not updating things again
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
    m_shouldIgnoreButtonClickWhenTimed = false;
    
    // After the header each item follows
    _patternItems = (CDPatternItemHeader *)malloc(_numberOfPatternItems * sizeof(CDPatternItemHeader));
    
    int i = 0;
    for (int p = LEDPatternTypeMin; p < LEDPatternTypeMax; p++) {
        if (p == LEDPatternTypeDoNothing || p == LEDPatternTypeFadeIn || p == LEDPatternTypeImageLinearFade || p == LEDPatternTypeImageEntireStrip || p == LEDPatternTypeCrossfade) {
            continue; // skip a few
        }
        bzero(&_patternItems[i], sizeof(CDPatternItemHeader));
        
        _patternItems[i].patternType = (LEDPatternType)p;
        _patternItems[i].patternEndCondition = CDPatternEndConditionOnButtonClick;
        _patternItems[i].duration = 2000; //  2 seconds
        _patternItems[i].patternDuration = 2000;
        // bzero takes care of these
//        _patternItems[i].patternOptions = 0;
//        _patternItems[i].dataLength = 0;
        _patternItems[i].shouldSetBrightnessByRotationalVelocity = 0; // Nope.
//        _patternItems[i].color = (uint8_t)random(255) << 16 | (uint8_t)random(255) << 8 | (uint8_t)random(255); //        0xFF0000; // red
//        _patternItems[i].color = (uint8_t)random(255) << 16 | (uint8_t)random(255) << 8 | (uint8_t)random(255); //        0xFF0000; // red
        switch (random(3)) { case 0: _patternItems[i].color = 0xFF0000; break; case 1: _patternItems[i].color = 0x00FF00; break; case 2: _patternItems[i].color = 0x0000FF; break; }
//        _patternItems[i].dataOffset = 0;
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

CDPatternItemHeader CWPatternSequenceManager::makeFlashPatternItem(CRGB color) {
    CDPatternItemHeader result;
    result.patternType = LEDPatternTypeBlink;
    result.duration = 500;
    result.patternDuration = 500;
    result.color = color;
    result.patternEndCondition = CDPatternEndConditionOnButtonClick;
    return result;
}

void CWPatternSequenceManager::makeSequenceFlashColor(uint32_t color) {
    _numberOfPatternItems = 2;
    _patternItems = (CDPatternItemHeader *)malloc(sizeof(CDPatternItemHeader) * _numberOfPatternItems);
    _patternItems[0].patternType = LEDPatternTypeSolidColor;
    _patternItems[0].duration = 500;
    _patternItems[0].patternDuration = 500; // Not used
    _patternItems[0].color = color;
    _patternItems[0].patternEndCondition = CDPatternEndConditionAfterDuration;
    
    _patternItems[1].patternType = LEDPatternTypeSolidColor;
    _patternItems[1].duration = 500;
    _patternItems[1].color = CRGB::Black;
    _patternItems[1].patternDuration = 500; // not used..
    _patternItems[1].patternEndCondition = CDPatternEndConditionAfterDuration;
}

void CWPatternSequenceManager::loadSequenceNamed(const char *name) {
    ASSERT(name != NULL);
    ASSERT(g_sequencePath != NULL);
    
    int pathLength = strlen(g_sequencePath);
    int filenameAndPathSize = pathLength + strlen(name) + 1; // NULL terminator
    // TODO: macro this
#define STACK_BUFFER_SIZE 64
    char stackBuffer[STACK_BUFFER_SIZE];
    char *filename;
    if (filenameAndPathSize <= STACK_BUFFER_SIZE) {
        filename = stackBuffer;
    } else {
        filename = (char *)malloc(sizeof(char) * filenameAndPathSize);
    }
    
    // Copy in the path to the file
    strcpy(filename, g_sequencePath);
    strcpy(&filename[pathLength], name);
    
    DEBUG_PRINTF("  loading sequence name: %s at %s\r\n", name, filename);
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
        // Only version 4 now
        if (patternHeader.version == SEQUENCE_VERSION) {
            // Free existing stuff
            if (_patternItems) {
                DEBUG_PRINTLN("ERROR: PATTERN ITEMS SHOULD BE FREE!!!!");
            }
                // Then read in and store the stock info
            _pixelCount = patternHeader.pixelCount;
            _numberOfPatternItems = patternHeader.patternCount;
            m_shouldIgnoreButtonClickWhenTimed = patternHeader.ignoreButtonForTimedPatterns;
            ASSERT(sizeof(CDPatternItemHeader) == PATTERN_HEADER_SIZE_v0); // MAKE SURE IT IS RIGHT..
            DEBUG_PRINTF("pixelCount: %d, now reading %d items, headerSize: %d\r\n", _pixelCount, _numberOfPatternItems, sizeof(CDPatternItemHeader));
            
            // After the header each item follows
            int numBytes = _numberOfPatternItems * sizeof(CDPatternItemHeader);
            _patternItems = (CDPatternItemHeader *)malloc(numBytes);
            memset(_patternItems, 0, numBytes); // shouldn't be needed, but do it anyways

            for (int i = 0; i < _numberOfPatternItems; i++ ){
                DEBUG_PRINTF("reading item %d\r\n", i);
                sequenceFile.readBytes((char*)&_patternItems[i], sizeof(CDPatternItemHeader));
                DEBUG_PRINTF("Header, type: %d, duration: %d, patternDuration %d\r\n", _patternItems[i].patternType, _patternItems[i].duration, _patternItems[i].patternDuration);
                // Verify it
                bool validData = _patternItems[i].patternType >= LEDPatternTypeMin && _patternItems[i].patternType < LEDPatternTypeMax;
                ASSERT(validData);
                if (!validData) {
                    // fill it in with something..
                    _patternItems[i] = makeFlashPatternItem(CRGB::Red);
                } else {
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
    //
    if (filename != stackBuffer) {
        free(filename);
    }
}

void CWPatternSequenceManager::loadCurrentSequence() {
    // Validate state and loop around if needed
    if (_numberOfAvailableSequences > 0) {
        if (_currentSequenceIndex >= _numberOfAvailableSequences) {
            _currentSequenceIndex = 0;
        } else if (_currentSequenceIndex < 0) {
            _currentSequenceIndex = _numberOfAvailableSequences - 1;
        }
    } else {
        _currentSequenceIndex = -1;
    }
    
    DEBUG_PRINTF("--- loading sequence %d of %d --- \r\n", _currentSequenceIndex, _numberOfAvailableSequences);
    freePatternItems();

    if (_currentSequenceIndex >= 0) {
        const char *filename = _sequenceNames[_currentSequenceIndex];
        if (filename == g_defaultFilename) {
            // Load the default
            loadDefaultSequence();
        } else {
            loadSequenceNamed(filename);
        }
    } else {
        loadDefaultSequence();
    }

    firstPatternItem();
}

bool CWPatternSequenceManager::initSDCard() {
    DEBUG_PRINTLN("initSD Card");
#if 0 // DEBUG
//    delay(2000);
    DEBUG_PRINTLN("END: really slow pause... and doing SD init....");
#endif
//    pinMode(SD_CARD_CS_PIN, OUTPUT); // Any pin can be used as SS, but it must remain low
//    digitalWrite(SD_CARD_CS_PIN, LOW);
//    delay(10); // sd card is flipping touchy!!
    bool result = SD.begin(SPI_HALF_SPEED, SD_CARD_CS_PIN); //  was SPI_FULL_SPEED
    int i = 0;
#if DEBUG
    if (!result) {
        DEBUG_PRINTLN("SD Card full speed SPI init failed...trying again (slower doesn't work..)...");
    }
#endif
    while (!result) {
        result = SD.begin(SPI_DIV6_SPEED, SD_CARD_CS_PIN); // was SPI_HALF_SPEED...
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
    DEBUG_PRINTLN("done init strip, starting to init SD Card");
    
    bool result = initSDCard();
    DEBUG_PRINTLN("done init sd card");

    if (result) {
        DEBUG_PRINTLN("opening sd card to read it");
        // Load up the names of available patterns
        File rootDir = SD.open(g_sequencePath);
        _numberOfAvailableSequences = 0;
        // Loop twice; first time count, second time allocate and store
        char filenameBuffer[PATH_COMPONENT_BUFFER_LEN];
        while (rootDir.getNextFilename(filenameBuffer)) {
            if (isPatternFile(filenameBuffer)) {
                _numberOfAvailableSequences++;
                DEBUG_PRINTF("found pattern: %s\r\n", filenameBuffer);
            } else if (strcmp(filenameBuffer, RECORD_INDICATOR_FILENAME) == 0) {
                DEBUG_PRINTF("found record file name: %s\r\n", filenameBuffer);
                _shouldRecordData = true;
            } else {
                DEBUG_PRINTF("found unknown file name: %s\r\n", filenameBuffer);
            }
        }
        DEBUG_PRINTF("Found %d sequences on SD Card\r\n", _numberOfAvailableSequences);
        
        if (_numberOfAvailableSequences > 0) {
            // Now we can malloc the space to save the names
            // One last name is for the "g_defaultFilename" / default sequence
            _sequenceNames = (char **)malloc(sizeof(char*) * (_numberOfAvailableSequences + 1));
            _currentSequenceIndex = 0;
            rootDir.moveToStartOfDirectory();
            while (rootDir.getNextFilename(filenameBuffer)) {
                if (isPatternFile(filenameBuffer)) {
                    // allocate and store the name so we can easily load it later. --  + 1 is for the null terminator, and the extra +1 is for the "/"
                    char *mallocedName = (char *)malloc(sizeof(char) * strlen(filenameBuffer) + 1);
                    _sequenceNames[_currentSequenceIndex] = mallocedName;
                    strcpy(mallocedName, filenameBuffer);
                    DEBUG_PRINTF("copied name: %s len: %d\r\n", _sequenceNames[_currentSequenceIndex], strlen(_sequenceNames[_currentSequenceIndex]));
                    
                    _currentSequenceIndex++;
                }
            }
        } else {
            // leave space for just the default sequence
            _sequenceNames = (char **)malloc(sizeof(char*) * 1);
        }
        
        rootDir.close();
        
        // Always add in the default item
        _sequenceNames[_numberOfAvailableSequences] = g_defaultFilename;
        _numberOfAvailableSequences++;
        
        if (_shouldRecordData) {
            m_ledPatterns.flashThreeTimesWithDelay(CRGB(30,30,30), 150);
        }
        
        // TODO: Read the last sequence we were on to start from there again...but make this optional...
        
    } else {
        _numberOfAvailableSequences = 1;
        _sequenceNames = (char **)malloc(sizeof(char*) * 1);
        _sequenceNames[0] = g_defaultFilename;
        
    }
    _currentSequenceIndex = 0;
    loadCurrentSequence();
    
    if (buttonIsDown) {
        // Go into calibration mode for the accell
        m_orientation.beginCalibration();
        // Override the default sequence to blink
        m_ledPatterns.setPatternDuration(300);
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
        loadCurrentSequence();
    }
}

void CWPatternSequenceManager::restartCurrentSequence() {
    firstPatternItem();
}

void CWPatternSequenceManager::loadPriorSequence() {
    if (_numberOfAvailableSequences > 0) {
        _currentSequenceIndex--;
        loadCurrentSequence();
    }
}

void CWPatternSequenceManager::buttonClick() {
    if (m_orientation.isCalibrating()) {
        m_orientation.endCalibration();
        firstPatternItem();
    } else {
        if (m_shouldIgnoreButtonClickWhenTimed) {
            // Only go to next if it isn't timed; this helps me avoid switching the pattern accidentally when stepping on it.
            CDPatternItemHeader *itemHeader = getCurrentItemHeader();
            if (itemHeader == NULL || itemHeader->patternEndCondition == CDPatternEndConditionOnButtonClick) {
                nextPatternItem();
            }
        } else {
            nextPatternItem();
        }
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
        if (m_shouldIgnoreButtonClickWhenTimed) {
            // Only go to next if it isn't timed; this helps me avoid switching the pattern accidentally when stepping on it.
            CDPatternItemHeader *itemHeader = getCurrentItemHeader();
            if (itemHeader == NULL || itemHeader->patternEndCondition == CDPatternEndConditionOnButtonClick) {
                loadNextSequence();
            }
        } else {
            loadNextSequence();
        }
    }
}

void CWPatternSequenceManager::process() {
    
//    float time = millis()-m_timedPatternStartTime;
//    time = time / 1000.0;
//    Serial.println(time);
    
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
    // See if we should go to the next pattern if enough time passed
    if (itemHeader->patternEndCondition == CDPatternEndConditionAfterDuration) {
        // subtract out how much time has passed from our initial pattern, and when we actually started the first timed pattern. This gives more accurate timings.
        uint32_t timePassed = millis() - m_timedUsedBeforeCurrentPattern - m_timedPatternStartTime;
        if (timePassed >= itemHeader->duration) {
            nextPatternItem();
        }
    } else {
        // It is waiting for some other condition before going to the next pattern (button click is the only condition now)
    }
}


void CWPatternSequenceManager::updateBrightness() {
    CDPatternItemHeader *itemHeader = getCurrentItemHeader();
    if (itemHeader->shouldSetBrightnessByRotationalVelocity) {
        uint8_t brightness = m_orientation.getRotationalVelocityBrightness(m_ledPatterns.getBrightness());
#if DEBUG
        if (brightness < 10){
            DEBUG_PRINTLN("low brightness from velocity based updateBrightness");
        }
#endif
        
        m_ledPatterns.setBrightness(brightness);
    } else {
        m_ledPatterns.setBrightness(m_savedBrightness);
    }
}

void CWPatternSequenceManager::setLowBatteryWarning() {
    m_savedBrightness = 64; // Lower brightness so we can see it get low! it is updated on the update pass
}

void CWPatternSequenceManager::firstPatternItem() {
    _currentPatternItemIndex = 0;
    loadCurrentPatternItem();
}

void CWPatternSequenceManager::priorPatternItem() {
    _currentPatternItemIndex--;
    loadCurrentPatternItem();
}

void CWPatternSequenceManager::nextPatternItem() {
    _currentPatternItemIndex++;
    loadCurrentPatternItem();
}

void CWPatternSequenceManager::loadCurrentPatternItem() {
    ASSERT(_numberOfPatternItems > 0);
    
    // Ensure we always have good data
    if (_currentPatternItemIndex < 0) {
        _currentPatternItemIndex = _numberOfPatternItems - 1;
    } else if (_currentPatternItemIndex >= _numberOfPatternItems) {
        // Loop around
        _currentPatternItemIndex = 0;
    }
    
    CDPatternItemHeader *itemHeader = getCurrentItemHeader();
    // if we are the first timed pattern, then reset m_timedPatternStartTime and how many timed patterns have gone before us
    if (itemHeader->patternEndCondition == CDPatternEndConditionAfterDuration) {
        // We are the first timed one, or the only one, then we have to reset everything
        if (_currentPatternItemIndex == 0 || _numberOfPatternItems == 1) {
            resetStartingTime();
        } else if (getPreviousItemHeader()->patternEndCondition != CDPatternEndConditionAfterDuration) {
            // The last one was NOT timed, so we need to reset, as this is the first timed pattern after a button click, and all timed ones need to go off the start of this one so we get accurate full length timing
            resetStartingTime();
        } else {
            // The last one ran, and was timed, and we need to include its duration in what passed. Based on the first condition, we always have a previous that is not us, and was timed
            ASSERT(getPreviousItemHeader()->patternEndCondition == CDPatternEndConditionAfterDuration);
            m_timedUsedBeforeCurrentPattern += getPreviousItemHeader()->duration;
            // If the actual time passed isn't this long...well, make it so
            if ((millis() - m_timedPatternStartTime) < m_timedUsedBeforeCurrentPattern) {
                m_timedPatternStartTime = millis() - m_timedUsedBeforeCurrentPattern; // go back in the past!
            }
        }
        
    } else if (_currentPatternItemIndex == 0) {
        resetStartingTime();
    }
    
    // Reset the stuff based on the new header
    m_ledPatterns.setPatternType(itemHeader->patternType);
    // Migration help..
    uint32_t duration = itemHeader->patternDuration;
    if (duration == 0) {
        duration = itemHeader->duration;
    }
    m_ledPatterns.setPatternDuration(duration);
    m_ledPatterns.setPatternColor(itemHeader->color);
    m_ledPatterns.setDataInfo(itemHeader->dataFilename, itemHeader->dataLength);
#if DEBUG
    DEBUG_PRINTF("--------- Next pattern Item (patternType: %d): %d of %d, %d long\r\n", itemHeader->patternType, _currentPatternItemIndex, _numberOfPatternItems, itemHeader->duration);
#endif
    
    // Crossfade patterns need to know the next one!
    if (itemHeader->patternType == LEDPatternTypeCrossfade) {
        // Load the next... and set its color (cross fade doesn't use it)
        CDPatternItemHeader *nextItemHeader = getNextItemHeader();
        if (nextItemHeader->patternType != LEDPatternTypeCrossfade) {
            m_ledPatterns.setNextPatternType(nextItemHeader->patternType);
            m_ledPatterns.setPatternColor(nextItemHeader->color);
        } else {
            // We can't crossfade to crossfade; pick red as an error
            m_ledPatterns.setNextPatternType(LEDPatternTypeSolidColor);
            m_ledPatterns.setPatternColor(CRGB::Red);
        }
    }
    
    m_orientation.setFirstPass(true); // why do I need this??
}



