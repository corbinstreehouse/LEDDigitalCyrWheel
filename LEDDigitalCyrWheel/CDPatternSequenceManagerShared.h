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

// Stuff shared with the app side of things
typedef enum : int16_t {
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
} CDWheelCommand;

typedef enum : int16_t {
    CDWheelStatePlaying,
    CDWheelStatePaused,
    
} CDWheelState;



#endif /* CDPatternSequenceManagerShared_h */
