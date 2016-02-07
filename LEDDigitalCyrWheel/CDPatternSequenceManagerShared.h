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

#include "LEDPatternType.h" // Defines CD_ENUM

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
    

// When sending data over serial UART to the wheel, I first send a command, and then the rest of the data to be processed (ie: uploaded file)
// It is much easier if this is an 8 bit byte; the UART command and wheel command can be set in one BLE packet.
typedef CD_ENUM(int8_t, CDWheelUARTCommand)  {
    CDWheelUARTCommandWheelCommand, //
    CDWheelUARTCommandSetBrightness,
    CDWheelUARTCommandSetCurrentPatternSpeed,
    CDWheelUARTCommandPlayProgrammedPattern,
    CDWheelUARTCommandPlayImagePattern,
    CDWheelUARTCommandRequestPatternInfo,
    
    
    // Other things....like get a list of files or upload a new pattern, or "paint" pixels.
    
    CDWheelUARTCommandLastValue = CDWheelUARTCommandRequestPatternInfo,
};
    
    
// When receiving data over serial URART fromm the wheel
typedef CD_ENUM(int8_t, CDWheelUARTRecieveCommand)  {
    CDWheelUARTRecieveCommandInvalid, // So we don't interpret 0...
    CDWheelUARTRecieveCommandCurrentPatternInfo, //
    
    
    
};
    
// We recieve a struct of data depending on the command, and if i can really get it packed..if it isn't packed, it might not be worth it..
//typedef struct  __attribute__((__packed__)) {
//    CDWheelUARTRecieveCommand command; // CDWheelUARTRecieveCommandCurrentPatternInfo
//    CDPatternItemHeader itemHeader
//} CDWheelUARTRecievePatternInfoData; // Followed by the filename (optional)
    
    
    
    
 

#endif /* CDPatternSequenceManagerShared_h */
