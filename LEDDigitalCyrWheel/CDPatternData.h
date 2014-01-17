//
//  CDPatternData.h
//  CyrWheelPatternEditor
//
//  Created by Corbin Dunn on 1/16/14 .
//  Copyright (c) 2014 Corbin Dunn. All rights reserved.
//

#ifndef CyrWheelPatternEditor_CDPatternData_h
#define CyrWheelPatternEditor_CDPatternData_h

enum CDPatternEncodingType {
    CDPatternEncodingTypeRGB24 = 0, // Uncompressed 24-bit data
    CDPatternEncodingTypeRLE = 1, // RLE encoded (not done yet..)
};

typedef struct  __attribute__((__packed__)) _CDPatternDataHeader {
//    uint8_t patternEncodingType; // CDPatternEncodingType, but sized specifically
//    uint16 duration; // TODO: use it..
//    uint8_t patternSide; // TODO:
    uint16_t height; // aka: the number of pixels. width is just read in till the end of the data..
    uint16_t dataLength; // how long the data is following
} CDPatternDataHeader;


typedef struct __attribute__((__packed__)) _CGPatternData {
    void *data; // Variable....and inline
    CDPatternDataHeader header;
} CGPatternData;


#endif
