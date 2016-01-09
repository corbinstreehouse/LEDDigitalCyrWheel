
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
#include "SdFat.h"
#include "EEPROM.h"

#define RECORD_INDICATOR_FILENAME "record.txt" // If this file exists, we record data in other files.
#define BITMAP_FILE_EXTENSION "bmp"

#if DEBUG
    #warning "DEBUG Code is on!!"
#include "SdFatUtil.h"
#endif

#define ACCELEROMETER 1

#define FILENAME_MAX_LENGTH MAX_PATH


#if !PATTERN_EDITOR
static const char *g_sequencePath = "/"; // warning..changing this may require different buffers
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

CWPatternSequenceManager::CWPatternSequenceManager() : m_ledPatterns(STRIP_LENGTH), m_changeHandler(NULL)
{
    _patternItems = NULL;
    bzero(&m_rootFileInfo, sizeof(CDPatternFileInfo));
    m_currentFileInfo = NULL;
    m_shouldIgnoreButtonClickWhenTimed = true; // TODO: make this an option per sequence...
    ASSERT(sizeof(CDPatternItemHeader) == PATTERN_HEADER_SIZE_v0); // make sure I don't screw stuff up by chaning the size and not updating things again
    ASSERT(sizeof(LEDPatternOptions) == PATTERN_OPTIONS_SIZE_v0); // make sure I don't screw stuff up by chaning the size and not updating things again
    
    bzero(&m_defaultBitmapHeader, sizeof(CDPatternItemHeader));
    m_defaultBitmapHeader.patternType = LEDPatternTypeBitmap;
    m_defaultBitmapHeader.duration = 50;
    m_defaultBitmapHeader.patternDuration = 35;
    m_defaultBitmapHeader.patternEndCondition = CDPatternEndConditionOnButtonClick;
    m_defaultBitmapHeader.patternOptions = LEDPatternOptions(LEDBitmapPatternOptions(true, false));
    
    m_lowBattery = false;
}

static void freePatternFileInfoAndChildren(CDPatternFileInfo *fileInfo) {
    // Just reset the parent on the default item
    if (fileInfo) {
        if (fileInfo->children) {
            for (int i = 0; i < fileInfo->numberOfChildren; i++) {
                freePatternFileInfoAndChildren(&fileInfo->children[i]);
            }
            free(fileInfo->children);
#if DEBUG
            fileInfo->children = NULL;
#endif
        }
    }
}

static void freePatternFileInfoChildren(CDPatternFileInfo *fileInfo) {
    if (fileInfo) {
        if (fileInfo->children) {
            for (int i = 0; i < fileInfo->numberOfChildren; i++) {
                freePatternFileInfoAndChildren(&fileInfo->children[i]);
            }
            free(fileInfo->children);
            fileInfo->children = NULL; // leave in!!
        }
    }
}

void CWPatternSequenceManager::freeRootFileInfo() {
    freePatternFileInfoAndChildren(&m_rootFileInfo);
    m_currentFileInfo = NULL;
}

#if PATTERN_EDITOR
CWPatternSequenceManager::~CWPatternSequenceManager() {
    m_baseURL = nil;
//    m_patternDirectoryURL = nil;
    freePatternItems();
    freeRootFileInfo();
}
    
void CWPatternSequenceManager::setCyrWheelView(CDCyrWheelView *view) {
    m_ledPatterns.setCyrWheelView(view);
}

void CWPatternSequenceManager::setBaseURL(NSURL *url) {
    m_baseURL = url;
    loadSequencesFromRootDirectory();
}

//void CWPatternSequenceManager::setPatternDirectoryURL(NSURL *url) {
//    m_patternDirectoryURL = url;
//}

#endif

void CWPatternSequenceManager::loadDefaultSequence() {
    DEBUG_PRINTLN("       --- loading default sequence because the name was NULL --- ");
    freePatternItems();

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

//bool CWPatternSequenceManager::isSequenceEditableAtIndex(int index) {
//    if (index >= 0 && index <= _numberOfAvailableSequences) {
//        return _sequenceNames[index] != g_defaultFilename;
//    } else {
//        return false;
//    }
//}

//int CWPatternSequenceManager::getIndexOfSequenceName(char *name) {
//    for (int i = 0; i < _numberOfAvailableSequences; i++) {
//        if (strcmp(name, getSequenceNameAtIndex(i)) == 0) {
//            return i;
//        }
//    }
//    return -1;
//}

void CWPatternSequenceManager::makeSequenceFlashColor(CRGB color) {
    freePatternItems();
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

void CWPatternSequenceManager::setDynamicPatternType(LEDPatternType type, CRGB color) {
    CDPatternItemHeader header;
    header.patternType = type;
    header.duration = 500;
    header.patternDuration = 500;
    header.color = color;
    header.patternEndCondition = CDPatternEndConditionOnButtonClick;
    setSingleItemPatternHeader(&header);
}

void CWPatternSequenceManager::flashThreeTimes(CRGB color, uint32_t delayAmount) {
    m_ledPatterns.flashThreeTimes(color);
}

// if stackBufferSize == 0, it always allocates (yeah, bad mem mangement techniques)
// This walks to the parent, adds its path, and then adds the child's path to get a complete path
// returns SFN
size_t _recursiveGetFullpathName(const char *rootDirName, CDPatternFileInfo *fileInfo, char *buffer, size_t size, size_t startingOffset) {
    if (fileInfo->parent) {
        startingOffset = _recursiveGetFullpathName(rootDirName, fileInfo->parent, buffer, size, startingOffset);
    }
    // Add in our path, include seperators
    if (startingOffset > 0 && buffer[startingOffset-1] != '/') {
        buffer[startingOffset] = '/';
        startingOffset++;
        buffer[startingOffset] = 0; // So we can open it...
    }
    
    char *nameLocation = &buffer[startingOffset];
    if (fileInfo->parent == NULL) {
        // Root
        strcpy(nameLocation, rootDirName);
    } else {
        FatFile parentDirectory = FatFile(buffer, O_READ);
        FatFile file;
        if (file.open(&parentDirectory, fileInfo->dirIndex, O_READ)) {
//    #if DEBUG
//            DEBUG_PRINTF("  opened and copying the name. buffer: %s, name: ", buffer);
//            file.printName();
//            DEBUG_PRINTLN(" <-- name printed");
//    #endif
            if (!file.getSFN(nameLocation)) {
            } else {
            }
            file.close();
        } else {
            DEBUG_PRINTLN("  open failed");
        }
        parentDirectory.close();
    }
    return startingOffset + strlen(nameLocation);
}


void _getFullpathName(const char *rootDirName, CDPatternFileInfo *fileInfo, char *buffer, size_t size) {
    buffer[0] = 0;
//    DEBUG_PRINTF(" start to get name for dir: %d \r\n", fileInfo->dirIndex);
    _recursiveGetFullpathName(rootDirName, fileInfo, buffer, size, 0);
//    DEBUG_PRINTF(" computed filename: %s\r\n", buffer);
}

void CWPatternSequenceManager::updateLEDPatternBitmapFilename() {
    char fullFilenamePath[MAX_PATH];
    _getFullpathName(_getRootDirectory(), m_currentFileInfo, fullFilenamePath, MAX_PATH);
    m_ledPatterns.setBitmapFilename(fullFilenamePath);
}

void CWPatternSequenceManager::loadAsBitmapFileInfo(CDPatternFileInfo *fileInfo) {
    updateLEDPatternBitmapFilename();
    m_shouldIgnoreButtonClickWhenTimed = false; // This could be made an option that is settable/dynamically changable.
    if (m_ledPatterns.getBitmap() == NULL || !m_ledPatterns.getBitmap()->getIsValid()) {
        // Invalid bitmap..
        makeSequenceFlashColor(CRGB::DarkMagenta);
    } else {
        setSingleItemPatternHeader(&m_defaultBitmapHeader);
    }
}

void CWPatternSequenceManager::loadAsSequenceFileInfo(CDPatternFileInfo *fileInfo) {
    ASSERT(fileInfo->patternFileType == CDPatternFileTypeSequenceFile);
    
    char fullFilenamePath[MAX_PATH];
    _getFullpathName(_getRootDirectory(), fileInfo, fullFilenamePath, MAX_PATH);
    
    DEBUG_PRINTF("  loadAsSequenceFileInfo: %d at %s\r\n", fileInfo->dirIndex, fullFilenamePath);
    
    // open the file
    FatFile sequenceFile = FatFile(fullFilenamePath, O_READ);
#if DEBUG
    DEBUG_PRINTF(" OPENED file:");
    sequenceFile.printName();
    DEBUG_PRINTLN();
#endif
    
//    if (!sequenceFile.available()) {
//        sequenceFile.close(); // well, shouldn't be needed
//        // Try again??? Frequently I have to try twice.. this is stupid, but the SD card isn't consistent
//        sequenceFile = SD.open(fullFilenamePath);
//        DEBUG_PRINTF(" try again!! file: %s\r\n", sequenceFile.name());
//    }
    
    // This is reading the file format I created..
    // Header first
    CDPatternSequenceHeader patternHeader;
    if (sequenceFile.available()) {
        sequenceFile.read((char*)&patternHeader, sizeof(CDPatternSequenceHeader));
    } else {
        bzero(&patternHeader, sizeof(CDPatternSequenceHeader));
    }
    
    // Verify it
    if (verifyHeader(&patternHeader)) {
        // Only version 4 now
        if (patternHeader.version == SEQUENCE_VERSION) {
            // Free existing stuff
            ASSERT(_patternItems == NULL);
            // Then read in and store the stock info
            //            _pixelCount = patternHeader.pixelCount;
            
            // TODO: I could eliminate several variables and just store these
            _numberOfPatternItems = patternHeader.patternCount;
            m_shouldIgnoreButtonClickWhenTimed = patternHeader.ignoreButtonForTimedPatterns;
            
            ASSERT(sizeof(CDPatternItemHeader) == PATTERN_HEADER_SIZE_v0); // MAKE SURE IT IS RIGHT..
            DEBUG_PRINTF("now reading %d items, headerSize: %d\r\n", _numberOfPatternItems, sizeof(CDPatternItemHeader));
            
            // After the header each item follows
            _patternItems = (CDPatternItemHeader *)calloc(_numberOfPatternItems, sizeof(CDPatternItemHeader));
            //            memset(_patternItems, 0, numBytes); // shouldn't be needed, but do it anyways
            
            for (int i = 0; i < _numberOfPatternItems; i++ ){
                DEBUG_PRINTF("reading item %d\r\n", i);
                sequenceFile.read((char*)&_patternItems[i], sizeof(CDPatternItemHeader));
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
                        _patternItems[i].dataOffset = sequenceFile.curPosition();
                        //                        _patternItems[i].dataFilename = filename; // idiot..this points to stuff on the stack now!
                        //                _patternItems[i].data = (uint8_t *)malloc(sizeof(uint8_t) * dataLength);
                        //                sequenceFile.readBytes((char*)_patternItems[i].data, dataLength);
                        // seek past the data
                        sequenceFile.seekCur(dataLength);
                    } else {
                        // data pointer should always be NULL
                        _patternItems[i].dataOffset = 0;
                        //                        _patternItems[i].dataFilename = NULL;
                    }
                }
            }
            DEBUG_PRINTLN("DONE");
        } else {
            // We don't support this version... flash purple
            makeSequenceFlashColor(CRGB::Purple);
        }
    } else {
        // Bad data...flash yellow
        makeSequenceFlashColor(CRGB::Yellow);
    }
    
    sequenceFile.close();
}

void CWPatternSequenceManager::loadFileInfo(CDPatternFileInfo *fileInfo) {
    ASSERT(fileInfo != NULL);
    ASSERT(fileInfo->patternFileType != CDPatternFileTypeDirectory);

//    m_currentPatternItemsAreDynamic = false;
    // in case we get the root for some reason (shouldn't happen!)
    if (fileInfo == &m_rootFileInfo) {
        loadDefaultSequence();
        return;
    }
    
    freePatternItems();
    
    switch (fileInfo->patternFileType) {
        case CDPatternFileTypeSequenceFile: {
            loadAsSequenceFileInfo(fileInfo);
            break;
        }
        case CDPatternFileTypeBitmapImage: {
            loadAsBitmapFileInfo(fileInfo);
            break;
        }
        case CDPatternFileTypeDefaultSequence: {
            loadDefaultSequence();
            break;
        }
        default: {
            makeSequenceFlashColor(CRGB::Red); // error of some sort
            break;
        }
    }
    // Send the changed note at the end
    sendWheelChanged(CDWheelChangeReasonSequenceChanged);
}

// TODO: rewrite this
//bool CWPatternSequenceManager::deleteSequenceAtIndex(int index) {
//    bool result = false;
//    int lastIndex = _currentSequenceIndex;
//    if (index >= 0 && index < _numberOfAvailableSequences) {
//        char *name = getSequenceNameAtIndex(index);
//        if (name != g_defaultFilename) {
//#define STACK_BUFFER_SIZE 32
//            char stackBuffer[STACK_BUFFER_SIZE];
//            char *filename = getFullpathName(name, stackBuffer, STACK_BUFFER_SIZE);
//            DEBUG_PRINTF("deleting %s\n", filename);
//            
//            result = SD.remove(filename);
//            
//            if (filename != stackBuffer) {
//                free(filename);
//            }
//            if (lastIndex >= index) {
//                lastIndex--;
//            }
//        }
//    }
//    
//    if (result) {
//        loadSequencesFromRootDirectory();
//        if (_currentSequenceIndex != lastIndex) {
//            loadCurrentSequence();
//        }
//    }
//    return result;
//}

void CWPatternSequenceManager::loadFirstSequence() {
    
    setCurrentSequenceAtIndex(0);
//    _currentSequenceIndex = 0;
//    loadCurrentSequence(); // validates the index here
}

CDPatternFileInfo *CWPatternSequenceManager::_findFirstLoadableChild(CDPatternFileInfo *fileInfo, bool forwards) {
    if (fileInfo->children == NULL && fileInfo->patternFileType == CDPatternFileTypeDirectory) {
        // Lazy load of children
        loadPatternFileInfoChildren(fileInfo);
    }
    if (fileInfo->children) {
        if (forwards) {
            for (int i = 0; i < fileInfo->numberOfChildren; i++) {
                CDPatternFileInfo *potentialChild = _findFirstLoadable(&fileInfo->children[i], forwards);
                if (potentialChild) {
                    return potentialChild;
                }
            }
        } else {
            for (int i = fileInfo->numberOfChildren - 1; i >= 0; i--) {
                CDPatternFileInfo *potentialChild = _findFirstLoadable(&fileInfo->children[i], forwards);
                if (potentialChild) {
                    return potentialChild;
                }
            }
        }
    }
    return NULL;
}

CDPatternFileInfo *CWPatternSequenceManager::_findFirstLoadable(CDPatternFileInfo *fileInfo, bool forwards) {
    if (fileInfo->patternFileType != CDPatternFileTypeDirectory) {
        return fileInfo; // This item is loadable
    }
    return _findFirstLoadableChild(fileInfo, forwards);
}

CDPatternFileInfo *CWPatternSequenceManager::_findNextLoadableChild(CDPatternFileInfo *fromChild, bool forwards) {
    CDPatternFileInfo *result = NULL;
    CDPatternFileInfo *parent = fromChild->parent;
    if (parent) {
        // Go through the parent's children, finding the first loadable one
        int end, inc;
        if (forwards) {
            end = parent->numberOfChildren;
            inc = 1;
        } else {
            end = -1;
            inc = -1;
        }
        int start = fromChild->indexInParent + inc;

        for (int i = start; i != end; i += inc) {
            result = _findFirstLoadable(&parent->children[i], forwards);
            if (result) {
                return result;
            }
        }
        // We went through all the parent's children, and nothing is loadable. Go to our parent's next sibling
        result = _findNextLoadableChild(parent, forwards);
    } else {
        // We have reached the root; this should always have something to return; if it doesn't, we return the root, which should be loadable..
        result = _findFirstLoadableChild(fromChild, forwards);
        if (result == NULL) {
            DEBUG_PRINTLN("************* No loadable children?????");
            result = parent;
        }
    }
    return result;
}

void CWPatternSequenceManager::loadNextDirectory() {
    m_currentFileInfo = _findNextLoadableChild(m_currentFileInfo->parent, true);
    loadCurrentSequence();
}

void CWPatternSequenceManager::loadPriorDirectory() {
    m_currentFileInfo = _findNextLoadableChild(m_currentFileInfo->parent, false);
    loadCurrentSequence();
}


void CWPatternSequenceManager::_ensureCurrentFileInfo() {
    if (m_currentFileInfo == NULL) {
        m_currentFileInfo = _findFirstLoadableChild(&m_rootFileInfo, true);
        if (m_currentFileInfo == NULL) {
            m_currentFileInfo = &m_rootFileInfo;
        }
    }
}

void CWPatternSequenceManager::setCurrentSequenceAtIndex(int index) {
    _ensureCurrentFileInfo();
    if (m_currentFileInfo) {
        CDPatternFileInfo *parent = m_currentFileInfo->parent;
        if (parent && index < parent->numberOfChildren) {
            m_currentFileInfo = &parent->children[index];
        }
    }
    loadCurrentSequence();
}

static CDPatternFileInfo *_findInfoWithName(CDPatternFileInfo *info, const char *name) {
    // TODO: "full path" name compare search...
#warning broken
    /*
    
    if (strcmp(info->filename, name) == 0) {
        return info;
    }

    if (info->children != NULL) {
        for (int i = 0; i < info->numberOfChildren; i++) {
            CDPatternFileInfo *tmp = _findInfoWithName(&info->children[i], name);
            if (tmp) {
                return tmp;
            }
        }
    }
     */
    return NULL;
}

void CWPatternSequenceManager::setCurrentSequenceWithName(const char *name) {
    m_currentFileInfo = _findInfoWithName(&m_rootFileInfo, name);
    loadCurrentSequence();
}

void CWPatternSequenceManager::setSingleItemPatternHeader(CDPatternItemHeader *header) {
//    m_currentFileInfo = NULL;
//    m_currentPatternItemsAreDynamic = true;

    freePatternItems();
    _numberOfPatternItems = 1;

    _patternItems = (CDPatternItemHeader *)malloc(sizeof(CDPatternItemHeader) * _numberOfPatternItems);
    _patternItems[0] = *header;
}

void CWPatternSequenceManager::loadCurrentSequence() {
    // Validate the current info
    _ensureCurrentFileInfo();
    
    // If it is still NULL, load the default info.
    loadFileInfo(m_currentFileInfo);
    
    firstPatternItem();
}

bool CWPatternSequenceManager::getCurrentPatternFileName(char *buffer, size_t bufferSize) {
    _ensureCurrentFileInfo();
    
    bool result;
    if (m_currentFileInfo->patternFileType == CDPatternFileTypeDefaultSequence) {
        strcpy(buffer, "Default Sequence");
        result = true;
    } else {
        _getFullpathName(_getRootDirectory(), m_currentFileInfo, buffer, bufferSize);
        FatFile file = FatFile(buffer, O_READ);
        result = file.getName(buffer, bufferSize);
        file.close();
    }
    return result;
}


#if SD_CARD_SUPPORT

bool CWPatternSequenceManager::initSDCard() {
    DEBUG_PRINTLN("initSD Card");
#if 0 // DEBUG
//    delay(2000);
    DEBUG_PRINTLN("END: really slow pause... and doing SD init....");
#endif
//    pinMode(SD_CARD_CS_PIN, OUTPUT); // Any pin can be used as SS, but it must remain low
//    digitalWrite(SD_CARD_CS_PIN, LOW);
//    delay(10); // sd card is flipping touchy!!
//    bool result = SD.begin(SPI_HALF_SPEED, SD_CARD_CS_PIN); //  was SPI_FULL_SPEED
    bool result = m_sd.begin(SD_CARD_CS_PIN, SPI_FULL_SPEED); //  was SPI_FULL_SPEED
    
    int i = 0;
#if DEBUG
    if (!result) {
        DEBUG_PRINTLN("SD Card full speed SPI init failed...trying again (slower doesn't work..)...");
    }
#endif
    while (!result) {
        result = m_sd.begin(SD_CARD_CS_PIN, SPI_HALF_SPEED); // was SPI_HALF_SPEED...
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

#endif

bool CWPatternSequenceManager::initStrip() {
    m_ledPatterns.begin();
    m_ledPatterns.setBrightness(m_brightness);
    return true;
}

void CWPatternSequenceManager::loadSettings() {
    // Maybe reset to the default if the button is held down?
    m_brightness = EEPROM.read(EEPROM_BRIGHTNESS_ADDRESS);
    if (m_brightness < MIN_BRIGHTNESS || m_brightness > MAX_BRIGHTNESS) {
        m_brightness = DEFAULT_BRIGHTNESS;
        DEBUG_PRINTF("SAVED BRIGHTNESS setting to default..out of band\r\n");
    } else {
        DEBUG_PRINTF("SAVED BRIGHTNESS: %d\r\n", m_brightness);
    }
}



const char *CWPatternSequenceManager::_getRootDirectory() {
#if PATTERN_EDITOR
    if (m_baseURL != nil) {
        return [m_baseURL fileSystemRepresentation];
    } else {
        return "/var/tmp/.patternEditor/"; // Just something that doesn't exist
    }
#else
    return g_sequencePath;
#endif
}

static void _setupFileInfo(CDPatternFileInfo *info, CDPatternFileInfo *parent, int indexInParent, uint16_t dirIndex, CDPatternFileType type) {
    info->dirIndex = dirIndex;
    info->patternFileType = type;
    info->children = NULL;
    info->numberOfChildren = 0;
    info->parent = parent;
    info->indexInParent = indexInParent;
}

void CWPatternSequenceManager::loadSequencesFromRootDirectory() {
    freeRootFileInfo();
    
    _setupFileInfo(&m_rootFileInfo, NULL, -1, 0, CDPatternFileTypeDirectory);
    loadPatternFileInfoChildren(&m_rootFileInfo);
    
    // check for the record filename when loading the root
    m_sd.chdir(_getRootDirectory());
    if (m_sd.exists(RECORD_INDICATOR_FILENAME)) {
        m_shouldRecordData = true;
        m_ledPatterns.flashThreeTimes(CRGB(30,30,30));
    }
    
    loadFirstSequence();
}

void CWPatternSequenceManager::loadPatternFileInfoChildren(CDPatternFileInfo *parent) {
    ASSERT(parent);
    bool isRoot = isRootFileInfo(parent);
    if (m_sdCardWorks && parent->patternFileType == CDPatternFileTypeDirectory) {
        DEBUG_PRINTF("loadPatternFileInfoChildren start\r\n");

// corbin, better name getting..
#define STACK_BUFFER_SIZE PATH_COMPONENT_BUFFER_LEN
        char filenameBuffer[FILENAME_MAX_LENGTH];
        //  parent directory needs a larger buffer, and does dynamic malloc if needed
        _getFullpathName(_getRootDirectory(), parent, filenameBuffer, FILENAME_MAX_LENGTH);
#undef STACK_BUFFER_SIZE
        DEBUG_PRINTF("loadPatternFileInfoChildren: %s\r\n", filenameBuffer);
        
        FatFile directoryFile = FatFile(filenameBuffer, O_READ);
 
        // #define SHORT_FILENAME_LENGTH 13 // 12 + 1 for NULL, and needs to be 256 or so for the simulator!!
//        char shortFilenameBuffer[SHORT_FILENAME_LENGTH]; // 8.3 Short file name (SFN)
        char *shortFilenameBuffer = filenameBuffer; // reuse the same buffer for now. NOTE: This must be larger than 13 for the sim, which needs the full path stored!!

        CDPatternFileInfo *potentialChildren = NULL;
        int potentialChildrenSize = 0;
#define CHILDREN_GROW_SIZE 16 // 16 entries at a time. too much? extra memory?
        int childCount = 0;

        FatFile file;
        while (file.openNext(&directoryFile, O_READ)) {
            CDPatternFileType patternFileType = CDPatternFileTypeUnknown;
            if (file.isHidden()) {
                // Ignore hidden files
            } else if (file.isDir()) {
                patternFileType = CDPatternFileTypeDirectory;
            } else {
                // Record it..don't record the type! figure that out when we load it!!
                if (file.getSFN(shortFilenameBuffer)) {
                    char *ext = getExtension(shortFilenameBuffer);
                    if (ext) {
                        // strcasecmp available??
                        if (strcasecmp(ext, SEQUENCE_FILE_EXTENSION) == 0) {
                            patternFileType = CDPatternFileTypeSequenceFile;
                        } else if (strcasecmp(ext, BITMAP_FILE_EXTENSION) == 0) {
                            patternFileType = CDPatternFileTypeBitmapImage;
                        }
                    }
                }
            }
            
            if (patternFileType != CDPatternFileTypeUnknown) {
#if DEBUG
                DEBUG_PRINTF("Found file/folder: ");
                file.printName();
                DEBUG_PRINTLN("");
#endif

                // Save it!
                if (potentialChildren == NULL) {
                    potentialChildrenSize = CHILDREN_GROW_SIZE;
                    potentialChildren = (CDPatternFileInfo *)malloc(sizeof(CDPatternFileInfo) * potentialChildrenSize);
                } else if (childCount >= potentialChildrenSize) {
                    DEBUG_PRINTF("reallocing children, childCount: %d, free: %d\r\n", childCount, SdFatUtil::FreeRam());
                    potentialChildrenSize += CHILDREN_GROW_SIZE;
                    potentialChildren = (CDPatternFileInfo *)realloc(potentialChildren, sizeof(CDPatternFileInfo) * potentialChildrenSize);
                    DEBUG_PRINTF("  -- done children, childCount: %d, free: %d\r\n", childCount, SdFatUtil::FreeRam());
                }
                _setupFileInfo(&potentialChildren[childCount], parent, childCount, file.dirIndex(), patternFileType);

                childCount++;
            }
            file.close();
        }
        
        DEBUG_PRINTF("Found %d sequences/files/dirs on SD Card\r\n", childCount);

        // Add one for the default sequence
        if (isRoot) {
            // Make space for it, and set it up
            if (potentialChildren == NULL) {
                potentialChildrenSize = 1;
                potentialChildren = (CDPatternFileInfo *)malloc(sizeof(CDPatternFileInfo) * potentialChildrenSize);
            } else if (childCount >= potentialChildrenSize) {
                potentialChildrenSize += 1;
                potentialChildren = (CDPatternFileInfo *)realloc(potentialChildren, sizeof(CDPatternFileInfo) * potentialChildrenSize);
            }
            // setup the default item for the root item
            _setupFileInfo(&potentialChildren[childCount], parent, childCount, -1/*not used*/, CDPatternFileTypeDefaultSequence);
            childCount++;
        }
        
        freePatternFileInfoChildren(parent);
        // realloc array to save memory if we have a lot of extra space.. I'm not sure if this is worth it..
        if (potentialChildren && childCount <= (potentialChildrenSize /*+2*/)) {
            potentialChildren = (CDPatternFileInfo *)realloc(potentialChildren, sizeof(CDPatternFileInfo) * childCount);
        }
        
        parent->children = potentialChildren;
        parent->numberOfChildren = childCount;
        
        directoryFile.close();
    } else if (isRoot) {
        // Add one child..for the default sequence
        parent->numberOfChildren = 1;
        parent->children = (CDPatternFileInfo *)malloc(sizeof(CDPatternFileInfo));
        _setupFileInfo(&parent->children[0], parent, 0, -1/*not used*/, CDPatternFileTypeDefaultSequence);
    } else {
        parent->numberOfChildren = 0;
        parent->children = NULL;
    }
}

void CWPatternSequenceManager::init() {
    DEBUG_PRINTLN("::init");
    m_shouldRecordData = false;
    
    loadSettings();
#if ACCELEROMETER
    DEBUG_PRINTLN("init orientation");
    initOrientation();
#endif
    DEBUG_PRINTLN("init strip");
    initStrip();
    DEBUG_PRINTLN("done init strip, starting to init SD Card");
    
#if SD_CARD_SUPPORT
    m_sdCardWorks = initSDCard();
    DEBUG_PRINTLN("done init sd card");
#else
    m_sdCardWorks = false;
#endif
    
    loadSequencesFromRootDirectory();
}

void CWPatternSequenceManager::startRecordingData() {
    if (!m_orientation.isSavingData()) {
        m_ledPatterns.flashThreeTimes(CRGB::Green);
        m_orientation.beginSavingData();
    }
}

void CWPatternSequenceManager::endRecordingData() {
    if (m_orientation.isSavingData()) {
        m_orientation.endSavingData();
        m_ledPatterns.flashThreeTimes(CRGB::Blue);
    }
}

void CWPatternSequenceManager::startCalibration() {
    // Go into calibration mode for the accell
    if (!m_orientation.isCalibrating()) {
        m_orientation.beginCalibration();
        // Override the default sequence to blink
        m_ledPatterns.setPatternDuration(600);
        m_ledPatterns.setPatternColor(CRGB::Pink);
        m_ledPatterns.setPatternType(LEDPatternTypeBlink);
    }
}

void CWPatternSequenceManager::endCalibration() {
    DEBUG_PRINTLN("about to end!");
    if (m_orientation.isCalibrating()) {
        m_orientation.endCalibration();
        // Flash to let you know it is done
        m_ledPatterns.flashThreeTimes(CRGB::Green);
        firstPatternItem();
    } else {
        DEBUG_PRINTLN("not calibration!!!");

    }
}

void CWPatternSequenceManager::cancelCalibration() {
    DEBUG_PRINTLN("about to cancel!");
    if (m_orientation.isCalibrating()) {
        m_orientation.cancelCalibration();
    }
}

bool CWPatternSequenceManager::initOrientation() {
    DEBUG_PRINTLN("init orientation");
    bool result = m_orientation.init(); // TODO: return if it failed???
    
    DEBUG_PRINTLN("DONE init orientation");
    
    return result;
}

void CWPatternSequenceManager::loadNextSequence() {
    DEBUG_PRINTLN("loadNextSequence");
    _ensureCurrentFileInfo();
    m_currentFileInfo = _findNextLoadableChild(m_currentFileInfo, true);
    loadCurrentSequence();
}

void CWPatternSequenceManager::restartCurrentSequence() {
    firstPatternItem();
}

void CWPatternSequenceManager::loadPriorSequence() {
    DEBUG_PRINTLN("loadPriorSequence");
    _ensureCurrentFileInfo();
    m_currentFileInfo = _findNextLoadableChild(m_currentFileInfo, false);
    loadCurrentSequence();
}

void CWPatternSequenceManager::buttonClick() {
    if (m_orientation.isCalibrating()) {
        m_orientation.endCalibration();
        firstPatternItem();
    } else {
        // TODO: Maybe a "start on button click" option...in addition to m_shouldIgnoreButtonClickWhenTimed
        // Then, I could just call the next command..
        
        // bitmap images just go to the next sequence..
        if (currentFileInfoIsBitmapImage()) {
            loadNextSequence();
        } else if (m_shouldIgnoreButtonClickWhenTimed) {
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
    if (m_shouldRecordData) {
        if (!m_orientation.isSavingData()) {
            startRecordingData();
        } else {
            endRecordingData();
        }
    } else {
        if (currentFileInfoIsBitmapImage()) {
            loadNextDirectory();
        } else if (m_shouldIgnoreButtonClickWhenTimed) {
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
        m_ledPatterns.flashThreeTimes(CRGB::Yellow);
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
    if (m_lowBattery) {
        // Harcoded to have a low brightness
        m_ledPatterns.setBrightness(64);
    } else {
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
            m_ledPatterns.setBrightness(m_brightness);
        }
    }
}

void CWPatternSequenceManager::setBrightness(uint8_t brightness) {
    if (m_brightness != brightness) {
        DEBUG_PRINTF("old bright: %d, new: %d\r\n", m_brightness, brightness);
        m_brightness = brightness;
        updateBrightness();
        sendWheelChanged(CDWheelChangeReasonBrightnessChanged);
        EEPROM.write(EEPROM_BRIGHTNESS_ADDRESS, m_brightness);
    }
}

void CWPatternSequenceManager::setLowBatteryWarning() {
    m_lowBattery = true;
    updateBrightness();
#if BLUETOOTH
     // TODO: maybe turn off bluetooth to save energy??
#endif
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
    m_ledPatterns.setPatternOptions(itemHeader->patternOptions);
    // Assume it is the current file..
    
#if SD_CARD_SUPPORT
    if (itemHeader->patternType == LEDPatternTypeBitmap) {
        if (m_ledPatterns.getBitmap() == NULL){
            updateLEDPatternBitmapFilename();
        }
    } else if (itemHeader->patternType == LEDPatternTypeImageLinearFade || itemHeader->patternType == LEDPatternTypeImageEntireStrip) {
        char fullFilenamePath[MAX_PATH];
        _getFullpathName(_getRootDirectory(), m_currentFileInfo, fullFilenamePath, MAX_PATH);
        m_ledPatterns.setDataInfo(fullFilenamePath, itemHeader->dataLength); // datainfo retains the memory
        m_ledPatterns.setBitmapFilename(NULL);
    } else {
        m_ledPatterns.setDataInfo(NULL, 0);
        m_ledPatterns.setBitmapFilename(NULL);
    }
#endif
    
    
#if DEBUG
    DEBUG_PRINTF("--------- loadCurrentPatternItem (patternType: %d): %d of %d, %d long\r\n", itemHeader->patternType, _currentPatternItemIndex + 1, _numberOfPatternItems, itemHeader->duration);
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
    sendWheelChanged(CDWheelChangeReasonPatternChanged);
}

void CWPatternSequenceManager::processCommand(CDWheelCommand command) {
    // The commands are handled slightly differently depending on the current pattern we are playing.
    switch (command) {
        case CDWheelCommandNextPattern: {
            // Bitmaps can't really do a next item; there is only one, so they do the next sequence in the directory
            if (currentFileInfoIsBitmapImage()) {
                loadNextSequence();
            } else {
                nextPatternItem();
            }
            break;
        }
        case CDWheelCommandPriorPattern: {
            if (currentFileInfoIsBitmapImage()) {
                loadPriorSequence();
            } else {
                priorPatternItem();
            }
            break;
        }
        case CDWheelCommandNextSequence: {
            if (currentFileInfoIsBitmapImage()) {
                loadNextDirectory();
            } else {
                loadNextSequence();
            }
            break;
        }
        case CDWheelCommandPriorSequence: {
            if (currentFileInfoIsBitmapImage()) {
                loadPriorDirectory();
            } else {
                loadPriorSequence();
            }
            break;
        }
        case CDWheelCommandRestartSequence: {
            restartCurrentSequence();
            break;
        }
        case CDWheelCommandStartCalibrating: {
            startCalibration();
            break;
        }
        case CDWheelCommandEndCalibrating: {
            endCalibration();
            break;
        }
        case CDWheelCommandCancelCalibrating: {
            cancelCalibration();
            break;
        }
        case CDWheelCommandStartSavingGyroData: {
            startRecordingData();
            break;
        }
        case CDWheelCommandEndSavingGyroData: {
            endRecordingData();
            break;
        }
        case CDWheelCommandPlay: {
            play();
            break;
        }
        case CDWheelCommandPause: {
            pause();
            break;
        }
            
        default: {
            // Ignore unknown things
        }
    }
}



