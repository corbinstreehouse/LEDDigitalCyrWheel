//
//  BluetoothShared.h
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 11/22/15 .
//
//

#ifndef BluetoothShared_h
#define BluetoothShared_h

// Stuff shared with the app side of things
typedef enum {
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


#define kCLEDWheelServiceUUID "592BEE7E-6749-4128-8A10-B98F21ED9049"
// Damn AdaFruit does different formatting..
#define kLEDWheelServiceUUID_AdaFruit "59-2B-EE-7E-67-49-41-28-8A-10-B9-8F-21-ED-90-49"

#define kLEDWheelCharSendCommandUUID "4F6E71B6-55F9-4ED8-9720-FABC1B4B8CFD"
#define kLEDWheelCharSendCommandUUID_AdaFruit "4F-6E-71-B6-55-F9-4E-D8-97-20-FA-BC-1B-4B-8C-FD"


#endif /* BluetoothShared_h */
