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

// stuff shared with the editor

#if PATTERN_EDITOR
    #define ENUM_SIZE : int16_t
#else
    #define ENUM_SIZE
#endif
// TODO: make sure the enum size for the teensy matches

typedef enum ENUM_SIZE {
    CDDurationTypeSeconds,
    CDDurationTypeMilliSeconds, // yeah, could be in terms of seconds, but easier to represent this way
    CDDurationTypeIntervals,
} CDDurationType;


typedef enum ENUM_SIZE {
    CDPatternEncodingTypeRGB24 = 0, // Uncompressed 24-bit data
//    CDPatternEncodingTypeRLE = 1, // RLE encoded (not done yet..)
} CDPatternEncodingType;

typedef enum ENUM_SIZE {
    CDPatternTypeMin = 0,
    
    // Standard patterns predifined in code
    CDPatternTypeRainbow = 0,
    CDPatternTypeRainbow2,
    CDPatternTypeColorWipe,
    CDPatternTypeImageLEDGradient,
    CDPatternTypePluseGradientEffect,

    // Patterns defined by an image
    CDPatternTypeImageFade,
    
    // the next set is ordered specifically
    CDPatternTypeWarmWhiteShimmer,
    CDPatternTypeRandomColorWalk,
    CDPatternTypeTraditionalColors,
    CDPatternTypeColorExplosion,
    CDPatternTypeGradient,
    CDPatternTypeBrightTwinkle,
    CDPatternTypeCollision,
    
    CDPatternTypeMax,
    CDPatternTypeAllOff = CDPatternTypeMax,
} CDPatternType;


typedef struct  __attribute__((__packed__)) {
    CDPatternType patternType;
    CDDurationType durationType;
    int32_t duration; // Signed...but not really. shouldn't be negative
    uint32_t dataLength; // how long the data is following
    uint8_t *data; // When loaded, points to the data
} CDPatternItemHeader;
// Data of dataLength follows the header


// A given sequence file starts with this header
typedef struct  __attribute__((__packed__)) {
    uint8_t marker[3]; // 'SQC' -- to identify a valid header
    uint8_t version; // in case i need to do versioning of the header
    uint16_t patternCount; // Following the header will be this number of patterns
    uint32_t pixelCount; // What was designed against
} CDPatternSequenceHeader;

#endif
