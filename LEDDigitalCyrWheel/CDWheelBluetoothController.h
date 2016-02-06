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
#define BLUETOOTH_RST 4

#define BLUETOOTH_EEPROM_AUTOSTART 12  //not used yet, make sure i read one byte only!
#define BLUETOOTH_EEPROM_WHEEL_SERVICE (BLUETOOTH_EEPROM_AUTOSTART+1) // int32_t value, ending: 17
#define BLUETOOTH_EEPROM_WHEEL_COMMAND_CHAR (BLUETOOTH_EEPROM_WHEEL_SERVICE+4) // int32_t value, ending: 21        // NOT USED
#define BLUETOOTH_EEPROM_WHEEL_STATE_CHAR (BLUETOOTH_EEPROM_WHEEL_COMMAND_CHAR+4) // int32_t value
#define BLUETOOTH_EEPROM_BRIGHTNESS_CHAR (BLUETOOTH_EEPROM_WHEEL_STATE_CHAR+4) // int32_t value
#define BLUETOOTH_EEPROM_BRIGHTNESS_WRITE_CHAR (BLUETOOTH_EEPROM_BRIGHTNESS_CHAR+4) // int32_t value
#define BLUETOOTH_EEPROM_SERVICES_ARE_REGISTERED (BLUETOOTH_EEPROM_BRIGHTNESS_WRITE_CHAR+4) // 1 byte value

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
    int32_t m_wheelStateID;
    int32_t m_brightnessID;
    uint32_t m_lastProcessTime;
    
    bool servicesAreRegistered();
    bool registerServices();
    // properties can be NULL, and it isn't sent
    void sendCommandWithUUID128(const char *cmd, const char *uuid, const char *properties, int32_t *serviceID, int eepromIndex);
    
    void registerWheelLEDService();
    void registerWheelCharacteristics();
    
    void setName(char *name);
    
    void setCharacteristic16BitValue(const int cmdID, const uint16_t value);
    
    BTResponseCode currentResponseCode() {
        if ( strcmp(m_ble.buffer, "OK") == 0 ) return BTResponseCodeOK;
        if ( strcmp(m_ble.buffer, "ERROR") == 0 ) return BTResponseCodeError;
        return BTResponseCodeUnknown;
    }
    
    void _addCharacteristic(const char *characteristicStr, int propertyType, int eepromLocation, int32_t *characteristicID, uint16_t value);
public:
    CDWheelBluetoothController();
    void init(CWPatternSequenceManager *manager, bool buttonIsDown);
    void process();
    
    void wheelChanged(CDWheelChangeReason reason);
};


#endif /* BluetoothController_hpp */
