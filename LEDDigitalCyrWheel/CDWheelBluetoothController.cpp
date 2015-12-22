//
//  CDWheelBluetoothController.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 11/22/15 .
//
//

#include "LEDCommon.h" // defines BLUETOOTH

#if BLUETOOTH

#if DEBUG

#define VERBOSE_MODE 0 // 1 for debugging more stuff

#else

#define VERBOSE_MODE 0

#endif

#include "CDWheelBluetoothController.h"
#include "EEPROM.h"

CDWheelBluetoothController::CDWheelBluetoothController() : m_ble(BLUETOOTH_CS, BLUETOOTH_IRQ, BLUETOOTH_RST), m_initialized(false) {
    
}

// Abstract code that isn't going to depend on the BT backend
void _WheelChangedHandler(CDWheelChangeReason reason, void *data) {
    DEBUG_PRINTF("wheel changes\r\n");
    ((CDWheelBluetoothController*)data)->wheelChanged(reason);
}

void CDWheelBluetoothController::wheelChanged(CDWheelChangeReason reason) {
    switch (reason) {
        case CDWheelChangeReasonStateChanged: {
            setCharacteristic16BitValue(m_wheelStateID, m_manager->getWheelState());
            break;
        }
        case CDWheelChangeReasonBrightnessChanged: {
            setCharacteristic16BitValue(m_brightnessID, m_manager->getBrightness());
        }
    }
}

//0x02 - Read
//0x04 - Write Without Response
//0x08 - Write
//0x10 - Notify
//0x20 - Indicate

#define CHAR_PROP_READ 0x02
#define CHAR_PROP_WRITE_WITHOUT_RESPONSE 0x04
#define CHAR_PROP_WRITE 0x08
#define CHAR_PROP_NOTIFY 0x10
#define CHAR_PROP_INDICATE 0x20


// Sort of generic..
void CDWheelBluetoothController::registerWheelCharacteristics() {
    DEBUG_PRINTLN("Registering cyr wheel characteristics");
    
    // only write seems to work!!
    _addCharacteristic(kLEDWheelCharSendCommandUUID_AdaFruit, CHAR_PROP_WRITE, BLUETOOTH_EEPROM_WHEEL_COMMAND_CHAR, &m_wheelCommandCharactersticID, -1); // -1 means not set
    _addCharacteristic(kLEDWheelCharGetWheelStateUUID_AdaFruit, CHAR_PROP_READ|CHAR_PROP_NOTIFY, BLUETOOTH_EEPROM_WHEEL_STATE_CHAR, &m_wheelStateID, m_manager->getWheelState());
    _addCharacteristic(kLEDWheelBrightnessCharacteristicReadUUID_AdaFruit, CHAR_PROP_READ, BLUETOOTH_EEPROM_BRIGHTNESS_CHAR, &m_brightnessID, m_manager->getBrightness());

    // CHAR_PROP_WRITE_WITHOUT_RESPONSE fails
    _addCharacteristic(kLEDWheelBrightnessCharacteristicWriteUUID_AdaFruit, CHAR_PROP_WRITE, BLUETOOTH_EEPROM_BRIGHTNESS_WRITE_CHAR, &m_brightnessWriteID, m_manager->getBrightness());
    
}

// specific to adafruit be

static const char *kAddServiceCmd = "AT+GATTADDSERVICE";
//static const char *kAddCharacteristicCmd = "AT+GATTADDCHAR";
static const char *kUUID128 = "UUID128=";
static const char *kClearCmd = "AT+GATTCLEAR";
static const char *kCharacteristicCmd = "AT+GATTCHAR";

void CDWheelBluetoothController::init(CWPatternSequenceManager *manager, bool buttonIsDown) {
    m_manager = manager;
    
    // See if we should even initialize; only initialize if set to init on startup, OR the buttonIsDown
    bool shouldInit = buttonIsDown;
    if (!shouldInit) {
        shouldInit = true;
        // TODO: after initialized at least once store the init state...
//        char tmp = 0;
//        EEPROM.get(EEPROM_START_BLUETOOTH_AUTOMATICALLY_ADDRESS, tmp);
//        if (tmp == 0 || tmp == 1) {
//            shouldInit = tmp;
//        } else {
//            // ignore bad values
//        }
    }
    if (!shouldInit) {
        return;
    }
    
    m_manager->setDynamicPatternType(LEDPatternTypeSolidColor, CRGB::Blue);
    m_manager->process(); // Shows blue while we are loading bluetooth

    DEBUG_PRINTLN("initializing bluetooth");

    // Ugh, they hardcoded this to wait a second!
    m_initialized = m_ble.begin(VERBOSE_MODE);
    
    if (!m_initialized) {
        DEBUG_PRINTLN("bluetooth failed to init");
        // failed to init bluetooth
        m_manager->flashThreeTimes(CRGB::Blue);
        return;
    }
    
    m_manager->setWheelChangeHandler(_WheelChangedHandler, this);
    
    DEBUG_PRINTLN("bluetooth initialized, seeing if we have to burn in initial state");
    
#if DEBUG
    // Enable echo
    m_ble.echo(false);
    m_ble.sendCommandCheckOK("ATI"); // print version information

    
#else
    m_ble.echo(false);
#endif
    
    if (servicesAreRegistered()) {
        // Attempt to read in existing values for m_wheelServiceID...
        EEPROM.get(BLUETOOTH_EEPROM_WHEEL_SERVICE, m_wheelServiceID);
        EEPROM.get(BLUETOOTH_EEPROM_WHEEL_COMMAND_CHAR, m_wheelCommandCharactersticID);
        // TODO: validate m_wheelServiceID..
        DEBUG_PRINTF("restored: m_wheelServiceID %d, m_wheelCommandCharactersticID: %d\r\n", m_wheelServiceID, m_wheelCommandCharactersticID);
        
    } else {
        if (!registerServices()) {
            m_manager->flashThreeTimes(CRGB::BlueViolet);
        }
    }
}

bool CDWheelBluetoothController::servicesAreRegistered() {
#warning this always returns false for now....
    return false;
    DEBUG_PRINTLN("Requesting services");
//    m_ble.sendCommandCheckOK("AT+GATTLIST");
    m_ble.println("AT+GATTLIST");
    while ( m_ble.readline() ) {
        DEBUG_PRINTLN("..read...");
        //SerialDebug.println(buffer);
        if ( strcmp(m_ble.buffer, "OK") == 0 ) break;
        if ( strcmp(m_ble.buffer, "ERROR") == 0 ) break;
        DEBUG_PRINTF("buff: %s\n", m_ble.buffer);
    }
    
    DEBUG_PRINTLN(" ----- corbin -");
    DEBUG_PRINTF("buff: %s", m_ble.buffer);
    
//    m_ble.println("AT+GATTLIST");
//
//    while (1) {
//        // wait for a response...
//        while (m_ble.available() == 0) {
//            delay(0);
//        }
//        if (m_ble.readline()) {
//            if (currentResponseCode() != BTResponseCodeUnknown) {
//                DEBUG_PRINTLN(m_ble.buffer);
//            } else {
//                break; // Got an OK or ERROR response
//            }
//        }
//    }
//    
    return false;
    /*
    // maybe check eeprom for faster starting?
    // EEPROM.get(EEPROM_START_BLUETOOTH_AUTOMATICALLY_ADDRESS, tmp);
    bool result = false;
    m_ble.println("AT+GATTLIST");
    while (m_ble.available() && m_ble.readline()) {
        if (currentResponseCode() != BTResponseCodeUnknown) {
            DEBUG_PRINTLN(m_ble.buffer);
        } else {
            break; // Got an OK or ERROR response
        }
    }
    // TODO: check the services string once I know the format
    
    return result;
     */
}

void CDWheelBluetoothController::sendCommandWithUUID128(const char *cmd, const char *uuid, const char *properties, int32_t *serviceID, int eepromIndex) {
    m_ble.printf("%s=UUID128=%s%s\r\n", cmd, uuid, properties ? properties : "");
    if (m_ble.sendCommandWithIntReply("", serviceID)) {
        // save in EEPROM for later
        EEPROM.put(eepromIndex, *serviceID);
        DEBUG_PRINTF("   Succeeded register, id: %d\r\n", *serviceID);
    } else {
        m_initialized = false;
    }
}

void CDWheelBluetoothController::registerWheelLEDService() {
    DEBUG_PRINTLN("Registering cyr wheels service");
    sendCommandWithUUID128(kAddServiceCmd, kLEDWheelServiceUUID_AdaFruit, NULL, &m_wheelServiceID, BLUETOOTH_EEPROM_WHEEL_SERVICE);
    // Make it the service that we are set to show up as..this is wrong, but this is how i can customize it... i just need to figure it out
    
    // https://www.bluetooth.org/en-us/specification/adopted-specifications

    // GAP  - Generic Access Profile
    // 0x06 : Incomplete List of 128-bit Service Class UUIDs
    // 16 bytes plus one byte for the info (06) ^ above == 17, in hex: 0x11, 0x11 bytes
    // but the value is little endian 
    m_ble.sendCommandCheckOK("AT+GAPSETADVDATA=11-06-"kLEDWheelServiceUUID_AdaFruit_INVERTED);
}

static const char *kAddCharacteristicFormat = "AT+GATTADDCHAR=UUID128=%s,PROPERTIES=0x%x,MIN_LEN=2,MAX_LEN=2,VALUE=0x%x\r\n";

void CDWheelBluetoothController::_addCharacteristic(const char *characteristicStr, int propertyType, int eepromLocation, int32_t *characteristicID, uint16_t value) {
    DEBUG_PRINTF(kAddCharacteristicFormat, characteristicStr, propertyType, value);
    m_ble.printf(kAddCharacteristicFormat, characteristicStr, propertyType, value);
    if (m_ble.sendCommandWithIntReply("", characteristicID)) {
        // save in EEPROM for later (once I am done..)
        EEPROM.put(eepromLocation, *characteristicID);
        DEBUG_PRINTF("   %s: succeeded register, id: %d\r\n", characteristicStr, *characteristicID);
    } else {
        // any one failing makes us not initialized...
        DEBUG_PRINTLN("BT CHAR FAILED TO INIT!");
        m_initialized = false;
    }
}

// this only has to be done once for initialiation of the chip
bool CDWheelBluetoothController::registerServices() {
    m_initialized = true; // Set to false on errors
    // First time initialization
    // Set the initial name
    setName(NULL);
    
    m_ble.sendCommandCheckOK(kClearCmd);
    
    // Add the cyr wheel service first
    registerWheelLEDService();
    registerWheelCharacteristics();

    // TODO: register gap intervals. default: 20,100,100,30
//     Minimum connection interval (in milliseconds) Maximum connection interval (in milliseconds) Advertising interval (in milliseconds) Advertising timeout (in milliseconds)
    /// SEee https://developer.mbed.org/forum/team-63-Bluetooth-Low-Energy-community/topic/5082/
    // They recommend to try min 20, max 100, bu tthat is already being used!
//    m_ble.sendCommandCheckOK("AT+GAPINTERVALS=20,100,100,30"));
//
//    while ( m_ble.readline() ) {
//        if ( !strcmp(m_ble.buffer, "OK") || !strcmp(m_ble.buffer, "ERROR")  ) break;
//        SerialDebug.println(m_ble.buffer);
//    }

    
    m_ble.reset();

    return m_initialized;
}

void CDWheelBluetoothController::setName(char *name) {
    if (name == NULL) {
        name = "LED Cyr Wheel";
    }
    m_ble.print("AT+GAPDEVNAME=");
    m_ble.println(name);
    
    if (!m_ble.waitForOK()) {
        DEBUG_PRINTLN("Could not set device name");
    }
}

void CDWheelBluetoothController::setCharacteristic16BitValue(const int cmdID, const uint16_t value) {
    DEBUG_PRINTF("%s=%d,0x%x\r\n", kCharacteristicCmd, cmdID, value);
    m_ble.printf("%s=%d,0x%x\r\n", kCharacteristicCmd, cmdID, value);
    m_ble.waitForOK();
}

bool CDWheelBluetoothController::readChar16BitValue(int index, int16_t *result) {
    // See if we have a wheel command
    m_ble.printf("%s=%d\r\n", kCharacteristicCmd, index);
    m_ble.readline(); // Read the response
    // if we didn't get "OK" or "ERROR", then we got data.
    if (currentResponseCode() == BTResponseCodeUnknown) {
//        DEBUG_PRINTF(" got buffer: %s\r\n", m_ble.buffer);
        // god damn stupid..
        
        // parse the buffer for the int. it is in the hex/byte format: "FF-FF"
        char *endPtr = m_ble.buffer;
        
        int16_t commandAsInt = strtol(endPtr, &endPtr, 16);
//        DEBUG_PRINTF("    read v: %d\r\n", commandAsInt);
        if (*endPtr == '-') {
            endPtr++; // go past the dash..
        }
        commandAsInt = (commandAsInt << 8) | strtol(endPtr, &endPtr, 16);
        
        *result = commandAsInt;
        
#if DEBUG
//        if (index == 3) {
//            DEBUG_PRINTF(" got buffer: %s\r\n", m_ble.buffer);
//        }
#endif
        
        return true;
    } else {
        return false;
    }
}




void CDWheelBluetoothController::process() {
    
    if (!m_initialized) {
        return;
    }
    
    // TODO: check ever X time interval to avoid slowing down other things.

    if (!m_ble.isConnected()) {
        return;
    }

    int16_t result;
    if (readChar16BitValue(m_wheelCommandCharactersticID, &result)) {
        if (result == -1) {
            // No command set; we set it to -1 initially, and also after we process a command given
        } else if (result >= CDWheelCommandFirst && result < CDWheelCommandCount) {
            DEBUG_PRINTF("   command given: %d\r\n", result);
            // Reset the value to -1
            CDWheelCommand command = (CDWheelCommand)result;
            m_manager->processCommand(command);
            // reset the value, so we stop processing it
            setCharacteristic16BitValue(m_wheelCommandCharactersticID, -1);
        } else {
            // out of band error
            DEBUG_PRINTF("   bad command number: %d\r\n", result);
            setCharacteristic16BitValue(m_wheelCommandCharactersticID, -1);
        }
    }
    
    if (readChar16BitValue(m_brightnessWriteID, &result)) {
        m_manager->setBrightness(result);
    }
    
#define BT_FILE_READ_TIMEOUT 500 // ms
    m_ble.flush();
    m_ble.setMode(BLUEFRUIT_MODE_DATA);
    // TODO: drive the LEDs while we are loading this..
    if (m_ble.available()) {
        int count = 0;
        uint32_t start = millis();
        // Read in the size
        int32_t totalBytes = 0;
        int i = 0;
        m_ble.readBytes((char*)&totalBytes, sizeof(totalBytes)); // skanky!
        
#define bufferSize 1024
        uint8_t buffer[bufferSize];
        
        DEBUG_PRINTF("totalBytes: %d\r\n", totalBytes);
        if (totalBytes > 0) {
            int sizeLeft = totalBytes;
            uint32_t lastReadTime = millis();
            while (sizeLeft > 0) {
                while (!m_ble.available()) {
//                    DEBUG_PRINTF("busy wait..., %d\r\n", millis());
                    if (millis() - lastReadTime > BT_FILE_READ_TIMEOUT) {
                        DEBUG_PRINTF("timeout! read: %d/%d\r\n", count, totalBytes);
                        sizeLeft = 0;
                        break;
                        // TODO: errors
                    }
                }
//                DEBUG_PRINTF("available......, %d\r\n", millis());

                int read = 0;
                while (m_ble.available()) {
                    int amountToRead = min(sizeLeft, bufferSize);
                    int amountRead = m_ble.readBytes(buffer, amountToRead);
                    
//                    DEBUG_PRINTF("%c", m_ble.read());
                    count += amountRead;
                    sizeLeft -= amountRead;
                    lastReadTime = millis();
                    read += amountRead;
                }
                
//                DEBUG_PRINTF("  %d READ: %d/%d \r\n", i, read, count);
//                if (read != 20 && count != totalBytes) {
//                    DEBUG_PRINTLN("                 BAD FOOD!!!!!!!!!!!!!!!!!!");
//                }
                i++;
            }
        }
        uint32_t time = millis() - start;
        DEBUG_PRINTF("  read %d, time took: %d ms, %g s\r\n", count, time, time /1000.0 );
    }
    
    

    m_ble.setMode(BLUEFRUIT_MODE_COMMAND);
    
}

#endif

