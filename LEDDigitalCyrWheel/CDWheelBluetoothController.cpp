//
//  CDWheelBluetoothController.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 11/22/15 .
//
//

#include "LEDCommon.h" // defines BLUETOOTH

#include "CDPatternSequenceManagerShared.h"
#include "CDWheelBluetoothController.h"
#include "EEPROM.h"

#if BLUETOOTH

#if DEBUG
    #define VERBOSE_MODE 0 // 1 for debugging more stuff
#else
    #define VERBOSE_MODE 0
#endif

#define BT_REFRESH_RATE 20 // ever X ms



CDWheelBluetoothController::CDWheelBluetoothController() : m_ble(BLUETOOTH_CS, BLUETOOTH_IRQ, BLUETOOTH_RST), m_initialized(false) {
    
}

// Abstract code that isn't going to depend on the BT backend
void _WheelChangedHandler(CDWheelChangeReason reason, void *data) {
    DEBUG_PRINTF("Pinging bluetooth: wheel changed reason: %d\r\n", reason);
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
            break;
        }
        case CDWheelChangeReasonPatternChanged: {
            // TODO: make sure this doesn't slow down things....
            if (m_ble.isConnected()) {
                _sendCurrentPatternInfo();
            }
            break;
        }
        case CDWheelChangeReasonSequenceChanged: {
            
            break;
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
    
    _addCharacteristic(kLEDWheelCharGetWheelStateUUID_AdaFruit, CHAR_PROP_READ|CHAR_PROP_NOTIFY, BLUETOOTH_EEPROM_WHEEL_STATE_CHAR, &m_wheelStateID, m_manager->getWheelState());
    _addCharacteristic(kLEDWheelBrightnessCharacteristicReadUUID_AdaFruit, CHAR_PROP_READ, BLUETOOTH_EEPROM_BRIGHTNESS_CHAR, &m_brightnessID, m_manager->getBrightness());
    _addCharacteristic(kLEDWheelFPSCharacteristicUUID_AdaFruit, CHAR_PROP_READ|CHAR_PROP_NOTIFY, BLUETOOTH_EEPROM_FPS_CHAR, &m_FPSCharID, m_manager->getFPS());
    
    // CHAR_PROP_WRITE_WITHOUT_RESPONSE fails
//    _addCharacteristic(kLEDWheelBrightnessCharacteristicWriteUUID_AdaFruit, CHAR_PROP_WRITE, BLUETOOTH_EEPROM_BRIGHTNESS_WRITE_CHAR, &m_brightnessWriteID, m_manager->getBrightness());
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
        // Make BLE an option to startup??
    }
    if (!shouldInit) {
        return;
    }
    
    DEBUG_PRINTLN("initializing bluetooth");

    // Ugh, they hardcoded this to wait a second!
    m_initialized = m_ble.begin(VERBOSE_MODE);

    // 1 second post-boot wait; I removed the delay in begin so we can show progress..
    uint32_t start = millis();
    while ((millis() - start) < 1000) {
        m_manager->incBootProgress();
        delay(100);
    }
    
    m_manager->incBootProgress();
    
    if (!m_initialized) {
        DEBUG_PRINTLN("bluetooth failed to init");
        // failed to init bluetooth
        m_manager->flashThreeTimes(CRGB::Blue);
        return;
    }
    
    m_manager->setWheelChangeHandler(_WheelChangedHandler, this);
    
    DEBUG_PRINTLN("bluetooth initialized, seeing if we have to burn in initial state");
    
    m_ble.echo(false);
#if DEBUG
    // Enable echo
//    m_ble.sendCommandCheckOK("ATI"); // print version information
#else
    m_ble.echo(false);
#endif
    
    m_manager->incBootProgress();
    
    if (servicesAreRegistered()) {
        m_manager->incBootProgress();
        
        // Load initial state
        setCharacteristic16BitValue(m_brightnessID, m_manager->getBrightness());
        m_manager->incBootProgress();

        setCharacteristic16BitValue(m_wheelStateID, m_manager->getWheelState());
        m_manager->incBootProgress();

        // FPS can start out as 0
//        setCharacteristic16BitValue(m_FPSCharID, m_manager->getFPS());
//        m_manager->incBootProgress();
        
        // Attempt to read in existing values for m_wheelServiceID...
        DEBUG_PRINTF("restored: m_wheelServiceID %d, m_wheelCommandCharactersticID: %d\r\n", m_wheelServiceID, m_wheelCommandCharactersticID);
    } else {
        bool registered = registerServices();
        // Show some failure state
        if (!registered) {
            m_manager->flashThreeTimes(CRGB::BlueViolet);
            m_manager->incBootProgress();
        }
    }
}

bool CDWheelBluetoothController::servicesAreRegistered() {
    uint8_t servicesAreRegistered = 0;
    EEPROM.get(BLUETOOTH_EEPROM_SERVICES_ARE_REGISTERED, servicesAreRegistered);
    
    DEBUG_PRINTF("BT services are registered (EEPROM read): %d\r\n", servicesAreRegistered);

    // Bad data..or initial state..
    if (servicesAreRegistered != 0 && servicesAreRegistered != 1) {
        return false;
    }

    // Read values and validate
    // TODO: validation...they seem to be incremented rather systematically.
    EEPROM.get(BLUETOOTH_EEPROM_WHEEL_SERVICE, m_wheelServiceID);
    EEPROM.get(BLUETOOTH_EEPROM_WHEEL_STATE_CHAR, m_wheelStateID);
    EEPROM.get(BLUETOOTH_EEPROM_BRIGHTNESS_CHAR, m_brightnessID);
    EEPROM.get(BLUETOOTH_EEPROM_FPS_CHAR, m_FPSCharID);
    
//    EEPROM.get(BLUETOOTH_EEPROM_BRIGHTNESS_WRITE_CHAR, m_brightnessWriteID);
    
    DEBUG_PRINTF("EEPROM services/chars: m_wheelServiceID %d, m_wheelCommandCharactersticID %d, m_wheelStateID %d, m_brightnessID %d\r\n", m_wheelServiceID, m_wheelCommandCharactersticID, m_wheelStateID, m_brightnessID);
    //TODO: validate each item??
    if (m_wheelServiceID < 0 || m_wheelServiceID > 10) {
        return false;
    } else if (m_wheelCommandCharactersticID < 0 || m_wheelCommandCharactersticID > 10) {
        return false;
    }
    return true;
    
    
    /*
// parsing the GATTLIST seems a waste of time.. just store once if I did it, and re-flash if I need to do it again, and allow BT to reset the value so I can reflash it
//    m_ble.sendCommandCheckOK("AT+GATTLIST");
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
    m_ble.setMode(BLUEFRUIT_MODE_COMMAND);
    DEBUG_PRINTF("   registering all services..setting the name\r\n");
    m_initialized = true; // Set to false on errors
    // First time initialization
    // Set the initial name
    setName(NULL);
    
    DEBUG_PRINTF("  name set; sending clear\r\n");
    m_ble.sendCommandCheckOK(kClearCmd);
    m_manager->incBootProgress();

    // Add the cyr wheel service first
    registerWheelLEDService();
    m_manager->incBootProgress();
    
    registerWheelCharacteristics();
    m_manager->incBootProgress();

//    m_ble.sendCommandCheckOK("AT+GAPINTERVALS=16,32,25,500"); // much slower w/these values
    // Lower doesn't help much; getting ~1.17s w/ these values for 1kb:
    m_ble.sendCommandCheckOK("AT+GAPINTERVALS=8,15,25,500"); // This is, actually quite a bit faster...
    m_ble.sendCommandCheckOK("AT+BLEPOWERLEVEL=4"); // Highest transmit power (0 might work well too). Doesn't affect speed, but might affect range.
    
    m_manager->incBootProgress();
    m_ble.reset();
    
    // 1 second delay after reset...(i removed it in BT so we can show progress)
    uint32_t start = millis();
    while ((millis() - start) < 1000) {
        m_manager->incBootProgress();
        delay(100);
    }
    
    DEBUG_PRINTF("Writing to EEPROM that we are registerd\r\n");
    EEPROM.write(BLUETOOTH_EEPROM_SERVICES_ARE_REGISTERED, (uint8_t)m_initialized);

    return m_initialized;
}

void CDWheelBluetoothController::setName(char *name) {
    m_ble.setMode(BLUEFRUIT_MODE_COMMAND);
    if (name == NULL) {
        name = "LED Cyr Wheel AFv3";
    }
    m_ble.print("AT+GAPDEVNAME=");
    m_ble.println(name);
    
    if (!m_ble.waitForOK()) {
        DEBUG_PRINTLN("Could not set device name");
    }
}

void CDWheelBluetoothController::setCharacteristic16BitValue(const int cmdID, const uint16_t value) {
    m_ble.setMode(BLUEFRUIT_MODE_COMMAND);
//    DEBUG_PRINTF("%s=%d,0x%x\r\n", kCharacteristicCmd, cmdID, value);
    m_ble.printf("%s=%d,0x%x\r\n", kCharacteristicCmd, cmdID, value);
    m_ble.waitForOK();
}

// Reading a characteristic value every instance is just too stupid slow. We need a callback when it changes! So, all my chars are read only. 
//bool CDWheelBluetoothController::readChar16BitValue(int index, int16_t *result) {
//    // See if we have a wheel command
//    m_ble.printf("%s=%d\r\n", kCharacteristicCmd, index);
//    m_ble.readline(); // Read the response
//    // if we didn't get "OK" or "ERROR", then we got data.
//    if (currentResponseCode() == BTResponseCodeUnknown) {
////        DEBUG_PRINTF(" got buffer: %s\r\n", m_ble.buffer);
//        // god damn stupid..
//        
//        // parse the buffer for the int. it is in the hex/byte format: "FF-FF"
//        char *endPtr = m_ble.buffer;
//        
//        int16_t commandAsInt = strtol(endPtr, &endPtr, 16);
////        DEBUG_PRINTF("    read v: %d\r\n", commandAsInt);
//        if (*endPtr == '-') {
//            endPtr++; // go past the dash..
//        }
//        commandAsInt = (commandAsInt << 8) | strtol(endPtr, &endPtr, 16);
//        
//        *result = commandAsInt;
//        
//#if DEBUG
////        if (index == 3) {
////            DEBUG_PRINTF(" got buffer: %s\r\n", m_ble.buffer);
////        }
//#endif
//        
//        return true;
//    } else {
//        return false;
//    }
//}
//


static void _printBytes(char *b, int size) {
#if DEBUG
    DEBUG_PRINTF(" sending bytes: ");
    char *c = b;
    for (int i = 0; i < size; i++ ) {
        if (i >=4 && (i %4) == 0) {
            DEBUG_PRINTF(" ");
        }
        DEBUG_PRINTF("%02X", *c);
        c++;
    }
    DEBUG_PRINTF("\r\n");
#endif
}

void CDWheelBluetoothController::_sendCurrentPatternInfo() {
    char filename[MAX_PATH];
    char *filenameToWrite = NULL;
    DEBUG_PRINTLN(" **** sending pattern info...");

    // Figre out what we want to write
    CDPatternItemHeader header;
    CDPatternItemHeader *headerPtr = m_manager->getCurrentItemHeader();
    if (headerPtr != NULL) {
        header = *headerPtr;
        // We write out the filename
        if (headerPtr->filename != NULL) {
            filenameToWrite = headerPtr->filename;
        } else if (header.patternType == LEDPatternTypeBitmap) {
            // Non-referenced images; the file we are showing is what we are playing. We want the full filename..
            m_manager->getCurrentPatternFileName(filename, MAX_PATH, false);
            filenameToWrite = filename;
        }
        
        if (filenameToWrite) {
            header.filenameLength = strlen(filenameToWrite);
        } else {
            header.filenameLength = 0; // Just make sure
        }
        
        DEBUG_PRINTF(" sending header.patternType: %d (0x%2x), filelength: %d\r\n", header.patternType, header.patternType, header.filenameLength);
    } else {
        // Write a header still indicating no pattern is playing by noting the count...
        bzero(&header, sizeof(CDPatternItemHeader));
        header.patternType = LEDPatternTypeCount;
    }
    
    // _printBytes((char*)&header, sizeof(CDPatternItemHeader))
    
    int bytesSent = 0;
    // Write to the BLE all at once
    m_ble.setMode(BLUEFRUIT_MODE_DATA);
    // Write the UART command we are sending
    m_ble.write((int8_t)CDWheelUARTRecieveCommandCurrentPatternInfo);
    // Maybe write how much data we are going to write....
    bytesSent += sizeof(int8_t);
    
    // Write the header..then the filename (if available)
    m_ble.write((char*)&header, sizeof(CDPatternItemHeader));
    bytesSent += sizeof(CDPatternItemHeader);
    if (filenameToWrite != NULL) {
//        DEBUG_PRINTLN(" **** writing relative filename: ");
        // _printBytes(filenameToWrite, header.filenameLength)
        m_ble.write(filenameToWrite, header.filenameLength); // No NULL terminator anymore
        bytesSent += header.filenameLength;
    }
    
    DEBUG_PRINTF(" **** done sending pattern info, sent: %d bytes\r\n", bytesSent);
}

void CDWheelBluetoothController::_writeSequenceFilenameForFileInfo(const CDPatternFileInfo *fileInfo) {
    if (fileInfo->patternFileType == CDPatternFileTypeSequenceFile) {
        char filename[MAX_PATH];
        m_manager->getFilenameForPatternFileInfo(fileInfo, filename, MAX_PATH);
        uint32_t filenameLength = strlen(filename); // We don't write a NULL terminator because we have a size byte
        m_ble.write((char*)&filenameLength, sizeof(filenameLength));
        m_ble.write((char*)filename, filenameLength);
//        _printBytes((char*)&filenameLength, sizeof(filenameLength));
//        _printBytes((char*)filename, sizeof(filenameLength));
    }
}

void CDWheelBluetoothController::_countSequencesForFileInfo(const CDPatternFileInfo *fileInfo) {
    if (fileInfo->patternFileType == CDPatternFileTypeSequenceFile) {
        m_counter++;
    }
}

void CDWheelBluetoothController::_sendCustomSequencesFromDirectory(const char *dir) {
    m_manager->changeToDirectory(dir); // not implemented!
    
    // Two passes..one to count, one to write the names
    m_counter = 0;
    m_manager->enumerateCurrentPatternFileInfoChildren(this, &CDWheelBluetoothController::_countSequencesForFileInfo);
    
    m_ble.setMode(BLUEFRUIT_MODE_DATA);
    // Write the count of them, and then for each one the filename and length
    
    CDWheelUARTCustomSequenceData d;
    d.command = CDWheelUARTRecieveCommandCustomSequences;
    d.count = m_counter;
    ASSERT(sizeof(CDWheelUARTCustomSequenceData) == 3);
    
    // Write out the header... and then each item.
    m_ble.setMode(BLUEFRUIT_MODE_DATA);
    m_ble.write((char*)&d, sizeof(CDWheelUARTCustomSequenceData));
//    _printBytes((char*)&d, sizeof(CDWheelUARTCustomSequenceData));
    
    m_manager->enumerateCurrentPatternFileInfoChildren(this, &CDWheelBluetoothController::_writeSequenceFilenameForFileInfo);
}

void CDWheelBluetoothController::process() {
    
    if (!m_initialized) {
        return;
    }
    
    // This refresh rate keeps my FPS at 600 or so. Otherwise we drop down to like 400 when just checkin isConnected(), and 180 when we check characteristics and such.
    if ((millis() - m_lastProcessTime) < BT_REFRESH_RATE) {
        return;
    }
    
    // update the FPS more slowly?
    setCharacteristic16BitValue(m_FPSCharID, m_manager->getFPS());


    // So, I don't use command mode anymore except for setting characteristic values
    m_ble.setMode(BLUEFRUIT_MODE_DATA);
    // I also don't check if we are connected, and just read...
    if (m_ble.available()) {
        DEBUG_PRINTLN(" .. BTE data available");
        // First read what the request is, and then do the request.
        CDWheelUARTCommand command;
        ASSERT(sizeof(command) <= 1);
        // I could use read(), which returns the int8 as an int, but this is more size explicit
        m_ble.readBytes((char*)&command, 1);
        DEBUG_PRINTF("BLUETOOTH Command: %d\r\n", command);
        if (command >= 0 && command <= CDWheelUARTCommandLastValue) {
            switch (command) {
                case CDWheelUARTCommandWheelCommand: {
                    // Read the next 16-bits for the value..i wish the size was better than this hack (ie: sizeof so much easier and future proof)
                    CDWheelCommand wheelCommand = (CDWheelCommand)-1;
                    ASSERT(sizeof(CDWheelCommand) == sizeof(uint16_t));
                    m_ble.readBytes((char*)&wheelCommand, sizeof(CDWheelCommand));
                    DEBUG_PRINTF("wheel command: %d\r\n", wheelCommand);
                    m_manager->processCommand(wheelCommand); // may re-enter!
                    break;
                }
                case CDWheelUARTCommandSetBrightness: {
                    // 16 bit brightness
                    uint16_t brightness = 0;
                    m_ble.readBytes((char*)&brightness, sizeof(uint16_t));
//                    DEBUG_PRINTF("bright command: %d\r\n", brightness);
                    // FOR NOW, this is an 8-bit value.
                    if (brightness > MAX_BRIGHTNESS) {
                        brightness = MAX_BRIGHTNESS;
                    }
                    m_manager->setBrightness(brightness);
                    break;
                }
                case CDWheelUARTCommandSetCurrentPatternSpeed: {
                    uint32_t speed = 32;
                    m_ble.readBytes((char*)&speed, sizeof(uint32_t));
                    m_manager->setCurrentPatternSpeed(speed);
                    break;
                }
                case CDWheelUARTCommandSetCurrentPatternColor: {
                    uint32_t colorCode = 1234; // random not black..
                    m_ble.readBytes((char*)&colorCode, sizeof(uint32_t));
                    CRGB color = CRGB(colorCode);
                    m_manager->setCurrentPatternColor(color);
                    break;
                }
                case CDWheelUARTCommandSetCurrentPatternBrightnessByRotationalVelocity: {
                    uint32_t value = 0;
                    m_ble.readBytes((char*)&value, sizeof(uint32_t));
                    m_manager->setCurrentPattenShouldSetBrightnessByRotationalVelocity(value);
                    break;
                }
                case CDWheelUARTCommandPlayProgrammedPattern: {
                    ASSERT(sizeof(LEDPatternType) == 4); // Make sure the size didn't change
                    // Type, speed, color
                    LEDPatternType patternType = LEDPatternTypeBlink;
                    uint32_t duration = 100;
                    CRGB color = CRGB::Red;
                    m_ble.readBytes((char*)&patternType, sizeof(LEDPatternType));
                    m_ble.readBytes((char*)&duration, sizeof(uint32_t));
                    m_ble.readBytes((char*)&color, sizeof(CRGB));
                    DEBUG_PRINTF("pattern type: %d\r\n", patternType);
                    
                    m_manager->setDynamicPatternType(patternType, duration, color);
                    // start playing if we are paused
                    if (m_manager->isPaused()) {
                        m_manager->play();
                    }
                    break;
                }
                case CDWheelUARTCommandPlayImagePattern: {
                    DEBUG_PRINTF("PROGRAMMED PATTERN ...");
                    
                    // Read in the speed/duration
                    uint32_t duration = 32;
                    m_ble.readBytes((char*)&duration, sizeof(uint32_t));

                    // Read in LEDBitmapPatternOptions
                    ASSERT(sizeof(LEDBitmapPatternOptions) == 4);
                    LEDBitmapPatternOptions options = LEDBitmapPatternOptions(false, false, false);
                    m_ble.readBytes((char*)&options, sizeof(LEDBitmapPatternOptions));
                    
                    // Read the 32-bit size of the filename to play (relative to root), and then read that in (including NULL terminator)
                    uint32_t filenameSize = 0;
                    m_ble.readBytes((char*)&filenameSize, sizeof(uint32_t));
                    DEBUG_PRINTF("reading: %d bytes for filename\r\n", filenameSize); // includes NULL term in size sent..

                    // Sanity check
                    if (filenameSize <= MAX_PATH) {
                        char filename[MAX_PATH];
                        m_ble.readBytes(filename, filenameSize);
                        DEBUG_PRINTF("setDynamicBitmapPatternType: %s  - duration: %d\r\n", filename, duration);
                        m_manager->setDynamicBitmapPatternType((const char*)filename, duration, options);
                        // start playing if we are paused
                        if (m_manager->isPaused()) {
                            m_manager->play();
                        }
                    } else {
                        DEBUG_PRINTLN("BAD SIZE");
                    }
                    
                    break;
                }
                case CDWheelUARTCommandRequestPatternInfo: {
                    // Nothing else is sent; we start sending back the pattern info..
                    _sendCurrentPatternInfo();
                    break;
                }
                case CDWheelUARTCommandRequestCustomSequences: {
                    // TODO: Read from what directory they are being requested from
                    _sendCustomSequencesFromDirectory(m_manager->_getRootDirectory());
                    
                    break;
                }
            }
        } else {
            // Invalid command; ugh..ignore it?
            DEBUG_PRINTF("invalid command: %d", command);
        }
    }
    // Maybe: If we have more data, process it on the next tick to make it go faster; otherwise, wait...
    m_ble.setMode(BLUEFRUIT_MODE_DATA);
//    if (!m_ble.available())
    {
        m_lastProcessTime = millis();
    }

    
    
#if SPEED_TEST
    m_ble.setMode(BLUEFRUIT_MODE_DATA);
    if (m_ble.available()) {
#define BT_FILE_READ_TIMEOUT 500 // ms
        DEBUG_PRINTF("available!\r\n");
        m_ble.setTimeout(500); // Larger timeout during this period of time..
        
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
//                    delay(50); // Give it some time; this is faster so we aren't making it do lots of SPI work??
//                    DEBUG_PRINTF("busy wait..., %d\r\n", millis());
                    if (millis() - lastReadTime > BT_FILE_READ_TIMEOUT) {
                        DEBUG_PRINTF("timeout! read: %d/%d\r\n", count, totalBytes);
                        sizeLeft = 0;
                        break;
                        // TODO: errors
                    }
                }
                DEBUG_PRINTF("available at %d\r\n", millis());

                int read = 0;
                while (m_ble.available()) {
                    int amountToRead = min(sizeLeft, bufferSize);
                    int amountRead = m_ble.readBytes(buffer, amountToRead);
//                    for (int tt = 0; tt < amountRead; tt++) {
//                        DEBUG_PRINTF("%c", buffer[tt]);
//                    }
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
#endif
    
}

#endif

