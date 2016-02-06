//
//  CDPatternSequenceManagerShared.h
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 12/3/15 .
//
//

#ifndef CDPatternSequenceManagerShared_h
#define CDPatternSequenceManagerShared_h

#include <stdint.h>

#include "LEDPatternType.h"

// Stuff shared with the app side of things

// Make this more portable with ARM compilers
//#ifndef __has_feature
//#define __has_feature(a) 0
//#endif
//
//#ifndef __has_extension
//#define __has_extension(a) 0
//#endif

// TODO: share the enum defines..
//#if (__cplusplus && __cplusplus >= 201103L && (__has_extension(cxx_strong_enums) || __has_feature(objc_fixed_enum))) || (!__cplusplus && __has_feature(objc_fixed_enum))
//    #define CD_ENUM(_type, _name)     enum _name : _type _name; enum _name : _type // for swift..
////    #define CD_ENUM(_type, _name)     enum _name : _type // for compiler to work in arduino
//#else
//    #define CD_ENUM(_type, _name)     enum _name : _type
//#endif


typedef CD_ENUM(int16_t, CDWheelCommand)  {
    CDWheelCommandFirst = 0,
    
    CDWheelCommandNextPattern = CDWheelCommandFirst,
    CDWheelCommandPriorPattern = 1,
    CDWheelCommandNextSequence = 2,
    CDWheelCommandPriorSequence = 3,
    CDWheelCommandRestartSequence = 4,
    CDWheelCommandStartCalibrating = 5,
    CDWheelCommandEndCalibrating = 6,
    CDWheelCommandCancelCalibrating = 7,
    CDWheelCommandStartSavingGyroData = 8,
    CDWheelCommandEndSavingGyroData = 9,
    
    CDWheelCommandPlay = 10,
    CDWheelCommandPause = 11,
    
    CDWheelCommandLast = CDWheelCommandPause,
    CDWheelCommandCount = CDWheelCommandLast + 1,
};

typedef CD_ENUM(int16_t, CDWheelState)  {
    CDWheelStatePlaying,
    CDWheelStatePaused,
    
};

// When sending data over serial UART, I first send a command, and then the rest of the data to be processed (ie: uploaded file)
// It is much easier if this is an 8 bit byte; the UART command and wheel command can be set in one BLE packet.
typedef CD_ENUM(int8_t, CDWheelUARTCommand)  {
    CDWheelUARTCommandWheelCommand, //
    CDWheelUARTCommandSetBrightness,
    CDWheelUARTCommandSetCurrentPatternSpeed,
    CDWheelUARTCommandPlayProgrammedPattern,
    CDWheelUARTCommandPlayImagePattern,
    
    // Other things....like get a list of files or upload a new pattern, or "paint" pixels.
    
    CDWheelUARTCommandLastValue = CDWheelUARTCommandPlayImagePattern,

};


#endif /* CDPatternSequenceManagerShared_h */
