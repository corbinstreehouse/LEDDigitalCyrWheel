//
//  BluetoothShared.h
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 11/22/15 .
//
//

#ifndef BluetoothShared_h
#define BluetoothShared_h

#define kLEDWheelServiceUUID "592BEE70-6749-4128-8A10-B98F21ED9049"
// Damn AdaFruit does different formatting because they send it a "byte" at a time..
#define kLEDWheelServiceUUID_AdaFruit "59-2B-EE-70-67-49-41-28-8A-10-B9-8F-21-ED-90-49"
// CUZ...yeah, stupid... little endian
#define kLEDWheelServiceUUID_AdaFruit_INVERTED "49-90-ED-21-8F-B9-10-8A-28-41-49-67-70-EE-2B-59"

//#define kLEDWheelCharSendCommandUUID "592BEE71-6749-4128-8A10-B98F21ED9049"
//#define kLEDWheelCharSendCommandUUID_AdaFruit "59-2B-EE-71-67-49-41-28-8A-10-B9-8F-21-ED-90-49"

// readonly characteristic
#define kLEDWheelCharGetWheelStateUUID "592BEE72-6749-4128-8A10-B98F21ED9049"
#define kLEDWheelCharGetWheelStateUUID_AdaFruit "59-2B-EE-72-67-49-41-28-8A-10-B9-8F-21-ED-90-49"

// Ugh -- these should be read/write...
#define kLEDWheelBrightnessCharacteristicReadUUID "592BEE73-6749-4128-8A10-B98F21ED9049"
#define kLEDWheelBrightnessCharacteristicReadUUID_AdaFruit "59-2B-EE-73-67-49-41-28-8A-10-B9-8F-21-ED-90-49"

//#define kLEDWheelBrightnessCharacteristicWriteUUID "592BEE74-6749-4128-8A10-B98F21ED9049"
//#define kLEDWheelBrightnessCharacteristicWriteUUID_AdaFruit "59-2B-EE-74-67-49-41-28-8A-10-B9-8F-21-ED-90-49"


#define kLEDWheelCharGetSequencesUUID "592BEE75-6749-4128-8A10-B98F21ED9049"
#define kLEDWheelDeleteCharacteristicUUID "592BEE76-6749-4128-8A10-B98F21ED9049"
#define kLEDWheelCharRequestSequencesUUID "592BEE77-6749-4128-8A10-B98F21ED9049"




#endif /* BluetoothShared_h */
