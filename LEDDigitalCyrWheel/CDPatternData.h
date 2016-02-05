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
#include "LEDPatternType.h"

#if __cplusplus

#include "FastLED.h"

#else

typedef struct CRGB {
    union {
        struct {
            union {
                uint8_t r;
                uint8_t red;
            };
            union {
                uint8_t g;
                uint8_t green;
            };
            union {
                uint8_t b;
                uint8_t blue;
            };
        };
        uint8_t raw[3];
    };
} CRGB;

#endif

typedef CD_ENUM(uint32_t, CDPatternEndCondition) {
    CDPatternEndConditionAfterDuration,
    CDPatternEndConditionOnButtonClick,
};


#define SEQUENCE_VERSION_v5 5
#define SEQUENCE_VERSION_SIZE 10

#define PATTERN_HEADER_SIZE_v5 36
#define PATTERN_OPTIONS_SIZE_v5 4

// Okay, going for 32-bit alignment here.. (4 bytes)
typedef struct  __attribute__((__packed__)) {
    LEDPatternType patternType; // 4

    // time is in milliseconds (ms)
    uint32_t duration; //4
    uint32_t patternDuration; //4 -- not used by all patterns, but how "fast" each interval in the pattern should run if it has some variable in how fast it runs
    LEDPatternOptions patternOptions; // 4 -- not used by all patterns -- if I change the size, PATTERN_OPTIONS_SIZE_v0 has to be incremented..
    
    CDPatternEndCondition patternEndCondition; //
    
    CRGB color;
    uint8_t _colorAlignment;
    
    uint32_t shouldSetBrightnessByRotationalVelocity:1;
    uint32_t __reservedOptions:30;

    // 8 bytes (64-bit value always for pointers to work out)
    union {
        uint32_t filenameLength; // When writing to a file, the filenameLength is written, and then following the struct is the filename
        char *filename; // After reading, the filename is allocated and copied here (it MUST be freed!)
        uint64_t __pointerSize; // Fixes pointers for 32/64-bit
    };
} CDPatternItemHeader; // Following the header is the filename to reference, if filenameLength != 0


// A given sequence file starts with this header
typedef struct  __attribute__((__packed__)) {
    uint8_t marker[3]; // 'SQC' -- to identify a valid header
    uint8_t version; // in case i need to do versioning of the header
    uint16_t patternCount; // Following the header will be this number of patterns
    uint32_t ignoreButtonForTimedPatterns:1;
    uint32_t _unused:31;
} CDPatternSequenceHeader; // maybe rename to "CDPatternFileHeader"

#endif
