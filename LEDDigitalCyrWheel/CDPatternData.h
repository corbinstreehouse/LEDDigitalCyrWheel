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
    CDPatternEndConditionAfterRepeatCount,
    CDPatternEndConditionOnButtonClick,
} CDPatternEndCondition;

typedef enum ENUM_SIZE {
    CDPatternEncodingTypeRGB24 = 0, // Uncompressed 24-bit data
//    CDPatternEncodingTypeRLE = 1, // RLE encoded (not done yet..)
} CDPatternEncodingType;


// NOTE: update g_patternTypeNames when this changes!!
typedef enum ENUM_SIZE {
    CDPatternTypeMin = 0,
    
    CDPatternTypeRotatingRainbow = 0,
    CDPatternTypeRotatingMiniRainbows,
    
    CDPatternTypeFadeOut,
    CDPatternTypeFadeIn,
    CDPatternTypeColorWipe,
    CDPatternTypeDoNothing,
    CDPatternTypeTheaterChase,
    
    CDPatternTypeGradient,
    CDPatternTypePluseGradientEffect,
    CDPatternTypeRandomGradients,

    // Patterns defined by an image
    CDPatternTypeImageLinearFade, // smooth traverse over pixels
    CDPatternTypeImageEntireStrip, // one strip piece at a time defined
    
    // the next set is ordered specifically
    CDPatternTypeWarmWhiteShimmer,
    CDPatternTypeRandomColorWalk,
    CDPatternTypeTraditionalColors,
    CDPatternTypeColorExplosion,
    CDPatternTypeRWGradient,
    
    CDPatternTypeWhiteBrightTwinkle,
    CDPatternTypeWhiteRedBrightTwinkle,
    CDPatternTypeRedGreenBrightTwinkle,
    CDPatternTypeColorTwinkle,
    
    CDPatternTypeCollision,
    
    CDPatternTypeWave, // 4 wave
    CDPatternTypeBottomGlow,
    CDPatternTypeRotatingBottomGlow,
    
    CDPatternTypeSolidColor,
    CDPatternTypeSolidRainbow,
    CDPatternTypeRainbowWithSpaces,

    CDPatternTypeMax,
    CDPatternTypeAllOff = CDPatternTypeMax,
} CDPatternType;

typedef union {
    struct {
        /// TODO: GRB format for perf... but RGB matches Adafruit packing little indian style  (hence BGR)
        uint8_t blue, green, red, _buff;
    };
    uint32_t color;
} PackedColorUnion;

// 28 bytes on device with buffer padding...28 on mac.
typedef struct  __attribute__((__packed__)) {
    CDPatternType patternType; // 1
#if !PATTERN_EDITOR
    char __buffer; // 1
#endif
    uint32_t duration; //4
    uint32_t intervalCount; //4
    CDPatternEndCondition patternEndCondition; //1
#if !PATTERN_EDITOR
    char __buffer2;// 1
#endif
    uint32_t color; //4
    uint32_t dataLength; // how long the data is following // 4
    union {
        uint8_t *data; // When loaded, points to the data
        uint64_t __unused_size; // creates 64-bits pointer size; so I can use the same struct size in 32-bit and 64-bit // 8
    };
} CDPatternItemHeader;

// A given sequence file starts with this header
typedef struct  __attribute__((__packed__)) {
    uint8_t marker[3]; // 'SQC' -- to identify a valid header
    uint8_t version; // in case i need to do versioning of the header
    uint16_t patternCount; // Following the header will be this number of patterns
    uint32_t pixelCount; // What was designed against
} CDPatternSequenceHeader;

#endif
