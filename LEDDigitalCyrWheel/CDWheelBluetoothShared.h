//
//  BluetoothShared.h
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 11/22/15 .
//
//

#ifndef BluetoothShared_h
#define BluetoothShared_h

#define kLEDWheelServiceUUID "592BEE7E-6749-4128-8A10-B98F21ED9049"
// Damn AdaFruit does different formatting..
#define kLEDWheelServiceUUID_AdaFruit "59-2B-EE-7E-67-49-41-28-8A-10-B9-8F-21-ED-90-49"

#define kLEDWheelCharSendCommandUUID "4F6E71B6-55F9-4ED8-9720-FABC1B4B8CFD"
#define kLEDWheelCharSendCommandUUID_AdaFruit "4F-6E-71-B6-55-F9-4E-D8-97-20-FA-BC-1B-4B-8C-FD"

#define kLEDWheelCharRequestSequencesUUID "86DEF442-3770-4C2E-BB7E-0735311FA99A" //When sent out, expect to recieve an answer on kLEDWheelCharRecieveSequencesUUID

#define kLEDWheelBrightnessCharacteristicUUID "796DEF442-3770-4C2E-BB7E-0735311FA99B"
#define kLEDWheelCharGetSequencesUUID "86DEF442-3770-4C2E-BB7E-0735311FA99B"

#define kLEDWheelDeleteCharacteristicUUID "96DEF442-3770-4C2E-BB7E-0735311FA99B"




#endif /* BluetoothShared_h */