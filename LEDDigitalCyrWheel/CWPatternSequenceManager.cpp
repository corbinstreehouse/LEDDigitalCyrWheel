
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
#ifndef PATTERN_EDITOR
#include "EEPROM.h"
#endif

#define RECORD_INDICATOR_FILENAME "RECORD.TXT" // If this file exists, we record data in other files.
#define PATTERN_FILE_EXTENSION "BMP"
#define PATTERN_FILE_EXTENSION_LC "bmp"

#if DEBUG
    #warning "DEBUG Code is on!!"
#endif

static const char *g_defaultFilename = "Default"; // We compare to this address to know if it is the default pattern
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

CWPatternSequenceManager::CWPatternSequenceManager() : m_ledPatterns(STRIP_LENGTH), m_state(CDWheelStatePlaying)
{
    _patternItems = NULL;
    bzero(&m_rootFileInfo, sizeof(CDPatternFileInfo));
    m_currentFileInfo = NULL;
    m_shouldIgnoreButtonClickWhenTimed = true; // TODO: make this an option per sequence...
    ASSERT(sizeof(CDPatternItemHeader) == PATTERN_HEADER_SIZE_v0); // make sure I don't screw stuff up by chaning the size and not updating things again
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
        // Don't delete the default filename!
        if (fileInfo->filename) {
            free(fileInfo->filename);
#if DEBUG
            fileInfo->filename = NULL;
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
    m_patternDirectoryURL = nil;
    freePatternItems();
    freeRootFileInfo();
}
    
void CWPatternSequenceManager::setCyrWheelView(CDCyrWheelView *view) {
    m_ledPatterns.setCyrWheelView(view);
}

void CWPatternSequenceManager::setBaseURL(NSURL *url) {
    m_baseURL = url;
}

void CWPatternSequenceManager::setPatternDirectoryURL(NSURL *url) {
    m_patternDirectoryURL = url;
}

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
    setDynamicPatternWithHeader(&header);
}

void CWPatternSequenceManager::flashThreeTimes(CRGB color, uint32_t delayAmount) {
    m_ledPatterns.flashThreeTimes(color);
}

// mallocs memory if bufferSize isn't large enough
// if stackBufferSize == 0, it always allocates (yeah, bad mem mangement techniques)
// This walks to the parent, adds its path, and then adds the child's path to get a complete path
char *_getFullpathName(CDPatternFileInfo *fileInfo, char *buffer, int stackBufferSize) {
    // Find the size by walking the chain once
    size_t filenameLength = strlen(fileInfo->filename);
    size_t totalCountNeeded = filenameLength;
    CDPatternFileInfo *directory = fileInfo->parent;
    while (directory) {
        size_t length = strlen(directory->filename);
        bool needsSep = directory->filename[length - 1] != '/';
        if (needsSep) {
            length++;
        }
        totalCountNeeded += length;
        directory = directory->parent;
    }
    
    totalCountNeeded++; // NULL terminator
    
    char *filename;
    if (totalCountNeeded <= stackBufferSize) {
        filename = buffer;
    } else {
        filename = (char *)malloc(sizeof(char) * totalCountNeeded);
    }
    
    // Now copy it, starting from the end working to the front
    size_t offset = totalCountNeeded - filenameLength - 1; // -1 goes past the NULL terminator
    strcpy(&filename[offset], fileInfo->filename); // adds NULL
    
    directory = fileInfo->parent;
    while (directory) {
        size_t length = strlen(directory->filename);
        bool needsSep = directory->filename[length - 1] != '/';
        if (needsSep) {
            offset--;
            filename[offset] = '/';
        }
        
        offset -= length;
        strncpy(&filename[offset], directory->filename, length); // Doesn't include the NULL terminator
        directory = directory->parent;
    }
    // We should be at the start
    ASSERT(offset == 0);

    return filename;
}

void CWPatternSequenceManager::_loadSequenceFile(File *sequenceFile) {
    // This is reading the file format I created..
    // Header first
    CDPatternSequenceHeader patternHeader;
    if (sequenceFile->available()) {
        sequenceFile->readBytes((char*)&patternHeader, sizeof(CDPatternSequenceHeader));
    } else {
        patternHeader.version = 0; // Fail
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
            int numBytes = _numberOfPatternItems * sizeof(CDPatternItemHeader);
            _patternItems = (CDPatternItemHeader *)malloc(numBytes);
            memset(_patternItems, 0, numBytes); // shouldn't be needed, but do it anyways
            
            for (int i = 0; i < _numberOfPatternItems; i++ ){
                DEBUG_PRINTF("reading item %d\r\n", i);
                sequenceFile->readBytes((char*)&_patternItems[i], sizeof(CDPatternItemHeader));
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
                        _patternItems[i].dataOffset = sequenceFile->position();
                        //                        _patternItems[i].dataFilename = filename; // idiot..this points to stuff on the stack now!
                        //                _patternItems[i].data = (uint8_t *)malloc(sizeof(uint8_t) * dataLength);
                        //                sequenceFile->readBytes((char*)_patternItems[i].data, dataLength);
                        // seek past the data
                        sequenceFile->seek(sequenceFile->position() + dataLength);
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
}

void CWPatternSequenceManager::_loadPatternBmpFile(File *file) {
#warning corbin, implement
}

void CWPatternSequenceManager::_loadPatternFileInfo(CDPatternFileInfo *fileInfo) {
    ASSERT(fileInfo != NULL);
    ASSERT(fileInfo->patternFileType != CDPatternFileTypeDirectory);

//    m_currentPatternItemsAreDynamic = false;
    
    // NULL filename, or the root is the default sequence
    if (fileInfo->filename == NULL || fileInfo == &m_rootFileInfo || fileInfo->patternFileType == CDPatternFileTypeDefaultSequence) {
        loadDefaultSequence();
        return;
    }
    
    freePatternItems();
    
    
#define STACK_BUFFER_SIZE 64 // A bit bigger now that I support directories
    char stackBuffer[64];
    char *fullFilenamePath = _getFullpathName(fileInfo, stackBuffer, STACK_BUFFER_SIZE);
#undef STACK_BUFFER_SIZE
    
    DEBUG_PRINTF("  _loadPatternFileInfo: %s at %s\r\n", fileInfo->filename, fullFilenamePath);
    
    // open the file
    File sequenceFile = SD.open(fullFilenamePath);
    DEBUG_PRINTF(" OPENED file: %s\r\n", sequenceFile.name());
    
    if (!sequenceFile.available()) {
        sequenceFile.close(); // well, shouldn't be needed
        // Try again??? Frequently I have to try twice.. this is stupid, but the SD card isn't consistent
        sequenceFile = SD.open(fullFilenamePath);
        DEBUG_PRINTF(" try again!! file: %s\r\n", sequenceFile.name());
    }
    
    switch (fileInfo->patternFileType) {
        case CDPatternFileTypeSequenceFile: {
            _loadSequenceFile(&sequenceFile);
            break;
        }
        case CDPatternFileTypePatternFile: {
            _loadPatternBmpFile(&sequenceFile);
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
    
    sequenceFile.close();
    // Deal withs tack memory, if necessary
    if (fullFilenamePath != stackBuffer) {
        free(fullFilenamePath);
    }
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

void CWPatternSequenceManager::setDynamicPatternWithHeader(CDPatternItemHeader *header) {
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
    _loadPatternFileInfo(m_currentFileInfo);
    
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
        result = SD.begin(SPI_HALF_SPEED, SD_CARD_CS_PIN); // was SPI_HALF_SPEED...
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

static CDPatternFileType _getPatternFileType(File *file) {
    if (file->isDirectory()) {
        return CDPatternFileTypeDirectory;
    }
    char *filename = file->name();
    if (filename) {
        char *ext = getExtension(filename);
        if (ext) {
            if (strcmp(ext, SEQUENCE_FILE_EXTENSION) == 0 || strcmp(ext, SEQUENCE_FILE_EXTENSION_LC) == 0) {
                return CDPatternFileTypeSequenceFile;
            }
            if (strcmp(ext, PATTERN_FILE_EXTENSION) == 0 || strcmp(ext, PATTERN_FILE_EXTENSION_LC) == 0) {
                return CDPatternFileTypePatternFile;
            }
        }
    }
    return CDPatternFileTypeUnknown;
}

bool CWPatternSequenceManager::initStrip() {
    m_ledPatterns.begin();
    m_ledPatterns.setBrightness(m_savedBrightness);
    return true;
}

void CWPatternSequenceManager::loadSettings() {
#if !PATTERN_EDITOR
    // TODO: how to burn the initial state??
    // TODO: make the state settable via bluetooth
    EEPROM.get(EEPROM_BRIGHTNESS_ADDRESS, m_savedBrightness);
    if (m_savedBrightness < MIN_BRIGHTNESS || m_savedBrightness > MAX_BRIGHTNESS) {
        m_savedBrightness = DEFAULT_BRIGHTNESS;
        DEBUG_PRINTF("SAVED BRIGHTNESS setting to default..out of band\r\n");
    } else {
        DEBUG_PRINTF("SAVED BRIGHTNESS: %d\r\n", m_savedBrightness);
    }
#else
    m_savedBrightness = DEFAULT_BRIGHTNESS; // Default value?? this is still super bright. maybe the algorithm is wrong..
#endif
}

const char *CWPatternSequenceManager::_getRootDirectory() {
#if PATTERN_EDITOR
    NSCAssert(m_baseURL != nil, @"need the base URL");
    return [m_baseURL fileSystemRepresentation];
#else
    return g_sequencePath;
#endif
}

static void _setupFileInfo(CDPatternFileInfo *info, CDPatternFileInfo *parent, int indexInParent, const char *filename, CDPatternFileType type) {
    info->filename = strdup(filename);
    info->patternFileType = type;
    info->children = NULL;
    info->numberOfChildren = 0;
    info->parent = parent;
    info->indexInParent = indexInParent;
}

void CWPatternSequenceManager::loadSequencesFromRootDirectory() {
    freeRootFileInfo();
    
    _setupFileInfo(&m_rootFileInfo, NULL, -1, _getRootDirectory(), CDPatternFileTypeDefaultSequence);
    loadPatternFileInfoChildren(&m_rootFileInfo);

    // That has side effect sfor the root, so check the other state it set
    if (m_shouldRecordData) {
        m_ledPatterns.flashThreeTimes(CRGB(30,30,30));
    }
    
    loadFirstSequence();
}

void CWPatternSequenceManager::loadPatternFileInfoChildren(CDPatternFileInfo *parent) {
    ASSERT(parent);
    ASSERT(parent->filename);
    bool isRoot = parent == &m_rootFileInfo;
    if (m_sdCardWorks) {
#define STACK_BUFFER_SIZE PATH_COMPONENT_BUFFER_LEN
        char filenameBuffer[STACK_BUFFER_SIZE];
        //  parent directory needs a larger buffer, and does dynamic malloc if needed
        char *parentDirectoryPath = _getFullpathName(parent, filenameBuffer, STACK_BUFFER_SIZE);
#undef STACK_BUFFER_SIZE
        
        DEBUG_PRINTF("loadPatternFileInfoChildren: %s\r\n", parentDirectoryPath);
        File parentFile = SD.open(parentDirectoryPath); // strcpy's it
        if (parentDirectoryPath != filenameBuffer) {
            free(parentDirectoryPath);
        }
        parentDirectoryPath = NULL; // Done; copied by SD if needed
        
        parentFile.moveToStartOfDirectory();
        // filenameBuffer doesn't have to be as large because it is just the filename, and really is limted to DOS
        int childCount = 0;
        while (true) {
            File f = parentFile.openNextFile();
            if (!f.isValid()) {
                break;
            }
            CDPatternFileType fileType = _getPatternFileType(&f);
            if (fileType != CDPatternFileTypeUnknown) {
                childCount++;
            } else {
                // Check to see if it is the record file..
                if (isRoot) {
                    if (strcmp(f.name(), RECORD_INDICATOR_FILENAME) == 0) {
                        m_shouldRecordData = true; // Strange to do this here..but I don't want to do another pass over the files on startup
                    }
                }
            }
            f.close();
        }
        
        DEBUG_PRINTF("Found %d sequences/files/dirs on SD Card\r\n", childCount);
        
        // Add one for the default sequence
        if (isRoot) {
            childCount++;
        }
        
        freePatternFileInfoChildren(parent);
        
        int childIndex = 0;
        if (childCount > 0) {
            parent->children = (CDPatternFileInfo *)malloc(sizeof(CDPatternFileInfo) * childCount);
            
            parentFile.moveToStartOfDirectory();
            while (true) {
                File f = parentFile.openNextFile();
                if (!f.isValid() || childCount == 0) {
                    break;
                }
                CDPatternFileType fileType = _getPatternFileType(&f);
                if (fileType != CDPatternFileTypeUnknown) {
                    CDPatternFileInfo *child = &parent->children[childIndex];
                    _setupFileInfo(child, parent, childIndex, f.name(), fileType);

                    DEBUG_PRINTF("copied name: %s len: %d\r\n", f.name(), strlen(f.name()));
                    childCount--; // Makes sure we don't blow the total count if something changed on the file system (shouldn't happen)
                    childIndex++;
                }
            }
        }
        
        if (isRoot && childCount > 0) {
            // Always add in the default item
            CDPatternFileInfo *child = &parent->children[childIndex];
            _setupFileInfo(child, parent, childIndex, g_defaultFilename, CDPatternFileTypeDefaultSequence);
            childIndex++;
        }
        
        parent->numberOfChildren = childIndex;
        
        parentFile.close();
        
    } else if (isRoot) {
        // Add one child..for the default sequence
        parent->numberOfChildren = 1;
        parent->children = (CDPatternFileInfo *)malloc(sizeof(CDPatternFileInfo));
        _setupFileInfo(&parent->children[0], parent, 0, g_defaultFilename, CDPatternFileTypeDefaultSequence);
    } else {
        parent->numberOfChildren = 0;
        parent->children = NULL;
    }
}

void CWPatternSequenceManager::init() {
    DEBUG_PRINTLN("::init");
    m_shouldRecordData = false;
    
    loadSettings();

    DEBUG_PRINTLN("init orientation");
    initOrientation();
    DEBUG_PRINTLN("init strip");
    initStrip();
    DEBUG_PRINTLN("done init strip, starting to init SD Card");
    
    m_sdCardWorks = initSDCard();
    DEBUG_PRINTLN("done init sd card");
    
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
    
#if DEBUG
    if (isPaused()) {
        play();
        nextPatternItem();
    } else {
        pause();
    }
    
#endif
    
    
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
    if (m_shouldRecordData) {
        if (!m_orientation.isSavingData()) {
            startRecordingData();
        } else {
            endRecordingData();
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

    // Turn off bluetooth to save power
#if BLUETOOTH
     // TODO:...
    
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
    // Assume it is the current file..
    
    char *fullFilenamePath = _getFullpathName(m_currentFileInfo, NULL, 0);
    m_ledPatterns.setDataInfo(fullFilenamePath, itemHeader->dataLength);
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

void CWPatternSequenceManager::processCommand(CDWheelCommand command) {
    switch (command) {
        case CDWheelCommandNextPattern: {
            nextPatternItem();
            break;
        }
        case CDWheelCommandPriorPattern: {
            priorPatternItem();
            break;
        }
        case CDWheelCommandNextSequence: {
            loadNextSequence();
            break;
        }
        case CDWheelCommandPriorSequence: {
            loadPriorSequence();
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



