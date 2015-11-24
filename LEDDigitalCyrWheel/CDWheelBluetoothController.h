//
//  CDWheelBluetoothController.h
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 11/22/15 .
//
//

#ifndef BluetoothController_hpp
#define BluetoothController_hpp

#include "CWPatternSequenceManager.h"
#include "CDWheelBluetoothShared.h"
#include "Adafruit_BluefruitLE_SPI.h"

#include <inttypes.h>

#define BLUETOOTH_CS 6 // chip select pin
#define BLUETOOTH_IRQ 5 // interrupt pin
#define BLUETOOTH_RST 7

#define BLUETOOTH_EEPROM_AUTOSTART 12
#define BLUETOOTH_EEPROM_WHEEL_SERVICE 13 // int32_t value, ending: 17
#define BLUETOOTH_EEPROM_WHEEL_COMMAND_CHAR 17 // int32_t value, ending: 21

typedef enum {
    BTResponseCodeOK = 0,
    BTResponseCodeError,
    BTResponseCodeUnknown,
} BTResponseCode;

class CDWheelBluetoothController {
private:
    CWPatternSequenceManager *m_manager;
    Adafruit_BluefruitLE_SPI m_ble;
    bool m_initialized;
    int32_t m_wheelServiceID;
    int32_t m_wheelCommandCharactersticID;

    bool servicesAreRegistered();
    bool registerServices();
    // properties can be NULL, and it isn't sent
    void sendCommandWithUUID128(const char *cmd, const char *uuid, const char *properties, int32_t *serviceID, int eepromIndex);
    
    void registerWheelLEDService();
    void registerWheelCharacteristics();
    
    void setName(char *name);
    
    BTResponseCode currentResponseCode() {
        if ( strcmp(m_ble.buffer, "OK") == 0 ) return BTResponseCodeOK;
        if ( strcmp(m_ble.buffer, "ERROR") == 0 ) return BTResponseCodeError;
        return BTResponseCodeUnknown;
    }
    
public:
    CDWheelBluetoothController();
    void init(CWPatternSequenceManager *manager, bool buttonIsDown);
    void process();
};


#endif /* BluetoothController_hpp */
