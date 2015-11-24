//
//  CDWheelBluetoothController.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 11/22/15 .
//
//

#include "LEDCommon.h" // defines BLUETOOTH

#if BLUETOOTH

#include "CDWheelBluetoothController.h"
#include "EEPROM.h"

CDWheelBluetoothController::CDWheelBluetoothController() : m_ble(BLUETOOTH_CS, BLUETOOTH_IRQ, BLUETOOTH_RST), m_initialized(false) {
    
}

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

    m_initialized = m_ble.begin(DEBUG);
    if (!m_initialized) {
        DEBUG_PRINTLN("bluetoothf failed to init");
        // failed to init bluetooth
        m_manager->flashThreeTimes(CRGB::Blue);
        return;
    }
    
    DEBUG_PRINTLN("bluetooth initialized, seeing if we have to burn in initial state");
    
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
    // maybe check eeprom for faster starting?
    // EEPROM.get(EEPROM_START_BLUETOOTH_AUTOMATICALLY_ADDRESS, tmp);
    bool result = false;
    m_ble.println("AT+GATTLIST");
    while (m_ble.readline()) {
        if (currentResponseCode() != BTResponseCodeUnknown) {
            DEBUG_PRINTLN(m_ble.buffer);
        } else {
            break; // Got an OK or ERROR response
        }
    }
    // TODO: check the services string once I know the format
    
    return result;
}

static const char *kAddServiceCmd = "AT+GATTADDSERVICE=";
static const char *kAddCharacteristicCmd = "AT+GATTADDCHAR=";
static const char *kUUID128 = "UUID128=";
static const char *kClearCmd = "AT+GATTCLEAR";
static const char *kCharacteristicCmd = "AT+GATTCHAR=";

void CDWheelBluetoothController::sendCommandWithUUID128(const char *cmd, const char *uuid, const char *properties, int32_t *serviceID, int eepromIndex) {
    m_ble.print(cmd);
    m_ble.print(kUUID128);
    m_ble.print(uuid);
    if (properties) {
        m_ble.print(", ");
        m_ble.print(properties);
    }
    m_ble.println();
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
}

void CDWheelBluetoothController::registerWheelCharacteristics() {
    DEBUG_PRINTLN("Registering cyr wheel characteristics");

    // write only property for the command characteristic
    const char *properties = "PROPERTIES=0x08, MIN_LEN=2, MAX_LEN=2, VALUE=0";
    sendCommandWithUUID128(kAddCharacteristicCmd, kLEDWheelCharSendCommandUUID_AdaFruit, properties, &m_wheelCommandCharactersticID, BLUETOOTH_EEPROM_WHEEL_COMMAND_CHAR);
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
    
    if (m_initialized) {
        m_ble.reset();
    }

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

void CDWheelBluetoothController::process() {
    if (!m_initialized) {
        return;
    }
    if (!m_ble.isConnected()) {
        return;
    }
    
    // See if we have a wheel command
    m_ble.print(kCharacteristicCmd);
    m_ble.print(m_wheelCommandCharactersticID);
    m_ble.println(); // Send
    m_ble.readline(); // Read the response
    // if we didn't get "OK" or "ERROR", then we got data.
    if (currentResponseCode() == BTResponseCodeUnknown) {
        // parse the buffer for the int
        int32_t commandAsInt = strtol(m_ble.buffer, NULL, 0);
        if (commandAsInt >= CDWheelCommandFirst && commandAsInt < CDWheelCommandCount) {
            DEBUG_PRINTF("   command given: %d\r\n", commandAsInt);
        } else {
            // out of band error
            DEBUG_PRINTF("   bad commadn number: %d\r\n", commandAsInt);
        }
    }
    
    /*
    bool hasChanged = false;
    
    
    //check writable characteristic for a new color
    ble.println("AT+GATTCHAR=2");
    ble.readline();
    if (strcmp(ble.buffer, "OK") == 0) {
        // no data
        return;
    }
    // Some data was found, its in the buffer
    Serial.print(F("[Recv] "));
    Serial.println(ble.buffer); //format FF-FF-FF R-G-B
    
    String buffer = String(ble.buffer);
    //check if the color should be changed
    if(buffer!=currentColorValue){
        hasChanged = true;
        currentColorValue = buffer;
        String redBuff = buffer.substring(0,2);
        String greenBuff = buffer.substring(3,5);
        String blueBuff = buffer.substring(6);
        const char* red = redBuff.c_str();
        const char* green =greenBuff.c_str();
        const char* blue = blueBuff.c_str();
        redVal = strtoul(red,NULL,16);
        greenVal = strtoul(green, NULL, 16);
        blueVal = strtoul(blue,NULL,16);
        setPixelColor();
    }
    ble.waitForOK();
    
    if(hasChanged){
        Serial.println("Notify of color change");
        //write to notifiable characteristic
        String notifyCommand = "AT+GATTCHAR=1,"+ currentColorValue;
        ble.println( notifyCommand );
        if ( !ble.waitForOK() )
        {
            error(F("Failed to get response from notify property update"));
        }
        
        //write to readable characteristic for current color
        String readableCommand = "AT+GATTCHAR=3,"+ currentColorValue;
        ble.println(readableCommand);
        
        if ( !ble.waitForOK() )
        {
            error(F("Failed to get response from readable property update"));
        }
    }
    delay(1000);
     */
}


/*
 // first, create the service
String serviceString = "AT+GATTADDSERVICE=";
serviceString += "UUID=0x6900";

int serviceID = BLE_print_with_reply(serviceString, 200);

// second, create the characteristic
String charString = "AT+GATTADDCHAR=";
charString += "UUID=0x1110";
charString += ", PROPERTIES=0x02"; // read only
charString += ", MIN_LEN=1";
charString += ", MAX_LEN=20";
charString += ", VALUE=hello";

int charID = BLE_print_with_reply(charString, 200);

// below is a helper function to send dynamic strings to the module
// and return the integer value sent back

int BLE_print_with_reply(String msg, int delayTime) {
  delay(delayTime); // optional delay, because it seemed to help communication
  uint32_t reply = -1;
  ble.println(msg);
  ble.sendCommandWithIntReply(F(""), &reply);
  return reply;
}
 */


#endif

