//
//  CDPatternData.h
//  CyrWheelPatternEditor
//
//  Created by Corbin Dunn on 1/16/14 .
//  Copyright (c) 2014 Corbin Dunn. All rights reserved.
//

#ifndef CyrWheelPatternEditor_CDPatternData_h
#define CyrWheelPatternEditor_CDPatternData_h

typedef enum : int16_t {
    CDPatternEncodingTypeRGB24 = 0, // Uncompressed 24-bit data
//    CDPatternEncodingTypeRLE = 1, // RLE encoded (not done yet..)
} CDPatternEncodingType;

typedef enum : int16_t {
    CDPatternTypeFade, // Playsback a fade pattern; the pixels in the image are processed sequentially and "feed" from a single source pixel
} CDPatternType;

typedef struct  __attribute__((__packed__)) _CDPatternDataHeader {
//    uint8_t patternEncodingType; // CDPatternEncodingType, but sized specifically
//    uint8_t patternSide; // TODO:
    CDPatternType patternType;
    uint32_t duration; // How it is interpreted varies on the pattern type. Value is in ms
    uint32_t pixels; // May or may not be applicable to the pattern type. The length, or number of pixels that the pattern was designed against. Design for the largest pixels you want to support, and smaller wheels will truncate or drop off the bottom pixels, but still maintain the same duration in order to allow synchronizing of multiple wheels (long term goal)
    uint32_t dataLength; // how long the data is following
} CDPatternDataHeader;


typedef struct __attribute__((__packed__)) _CGPatternData {
    void *data; // Variable....and inline
    CDPatternDataHeader header;
} CGPatternData;


#endif
