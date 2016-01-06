//
//  CWPatternData.h
//  CyrWheelPatternEditor
//
//  Created by Corbin Dunn on 1/16/14 .
//  Copyright (c) 2014 Corbin Dunn. All rights reserved.
//

#ifndef CyrWheelPatternEditor_CDPatternData_h
#define CyrWheelPatternEditor_CDPatternData_h

#include <stdint.h>
#include "LEDPatterns.h"

// stuff shared with the editor

#if PATTERN_EDITOR
    #define ENUM_SIZE : int16_t
#else
    #define ENUM_SIZE
#endif
// TODO: make sure the enum size for the teensy matches

typedef enum ENUM_SIZE {
    CDPatternEndConditionAfterDuration,
    CDPatternEndConditionOnButtonClick,
} CDPatternEndCondition;

typedef enum ENUM_SIZE {
    CDPatternEncodingTypeRGB24 = 0, // Uncompressed 24-bit data
//    CDPatternEncodingTypeRLE = 1, // RLE encoded (not done yet..)
} CDPatternEncodingType;


#define SEQUENCE_VERSION 4

// Update v3: 36 bytes...what happened??
// Update v4: 40??
#define PATTERN_HEADER_SIZE_v0 40
#define PATTERN_OPTIONS_SIZE_v0 4
typedef struct  __attribute__((__packed__)) {
    LEDPatternType patternType; // 2 (was 1...but went past 16, so it went to 2 bytes)
#if !PATTERN_EDITOR
//    char __buffer; // 1 now used
#endif
    uint32_t duration; //4
    uint32_t patternDuration; //4 -- not used by all patterns, but how "fast" each interval in the pattern should run if it has some variable in how fast it runs
    LEDPatternOptions patternOptions; //4 -- not used by all patterns -- if I change the size, PATTERN_OPTIONS_SIZE_v0 has to be incremnted..
    
    CDPatternEndCondition patternEndCondition; //1 (2 on desktop)
#if !PATTERN_EDITOR
    char __buffer2;// 1
#endif
    CRGB color; // 3 // TODO: wait, did this fuck up the ref of the offsets?
    uint8_t __buffer3; // 1 - I guess the compiler isn't packing, and is aligning on 4 byte boundaries..
    
    uint32_t dataLength; // how long the data is following // 4
    uint32_t dataOffset; // When loaded, points to the offset in the file where we should load from.
    uint32_t shouldSetBrightnessByRotationalVelocity:1; // (and next) 4
    uint32_t _unused:31;
    // ^^ TODO: move these to patternOptions
    
    union {
//        const char *dataFilename;  // A reference to the string....we don't own it! // NO longer needing this..
        uint64_t __unused_size; // creates 64-bits pointer size; so I can use the same struct size in 32-bit and 64-bit // 8
    };
    // ^^ eliminate if I make a new version

} CDPatternItemHeader;

// A given sequence file starts with this header
typedef struct  __attribute__((__packed__)) {
    uint8_t marker[3]; // 'SQC' -- to identify a valid header
    uint8_t version; // in case i need to do versioning of the header
    uint16_t patternCount; // Following the header will be this number of patterns
    uint32_t pixelCount; // Not used... I should remove it
    uint32_t ignoreButtonForTimedPatterns:1;
    uint32_t _unused:31;
} CDPatternSequenceHeader; // maybe rename to "CDPatternFileHeader"

#endif
