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
    

typedef CD_OPTIONS(uint16_t, CDWheelState)  {
    CDWheelStateNone = 0,
    CDWheelStatePlaying = 1 << 0,
    CDWheelStateNextPatternAvailable = 1 << 1,
    CDWheelStatePriorPatternAvailable = 1 << 2,
    CDWheelStateNextSequenceAvailable = 1 << 3,
    CDWheelStatePriorSequenceAvailable = 1 << 4,
};
    

    
 

#endif /* CDPatternSequenceManagerShared_h */
