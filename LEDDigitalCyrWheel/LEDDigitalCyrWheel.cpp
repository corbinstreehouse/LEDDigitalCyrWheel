// LED Digital Cyr Wheel
//  Created by corbin dunn, january 14, 2014

/* NOTES: For code completion to work:
    1. Select the Project
    2. Select the target named "Index"
    3. Select Build Phase
    4. Expand it; hit plus at the bottom 
    5. Add all *.c, *.cpp files to it
*/

#include  "Arduino.h"

#include "Button.h"
#include "Wire.h"
//#include <HardwareSerial.h>

#include <stdint.h> // We can compile without this, but it kills xcode completion without it! it took me a while to discover that..

#include "CWPatternSequenceManager.h"
#include "CDPatternData.h"
//#include "CDLEDStripPatterns.h"
#include "LEDDigitalCyrWheel.h"

#include <SPI.h>
#if BLUETOOTH
#include "Adafruit_BLE_UART.h"
#endif


#define IGNORE_VOLTAGE 1 //for hardware testing w/out a battery

// 2 cell LiPO, 4.2v each: 8.4v max. 3.0v should be the min, 3.0*2=6v min
#define LOW_VOLTAGE_VALUE 6.3 // min voltage for 2 cells....I was seeing values "normally" from 7.57+ on up...probably due to voltage sag when illuminating things. I might have to average the voltage over time to see what i am really getting, or lower the min value.
#define TIME_BETWEEN_VOLTAGE_READS 1000 // read every second..
#define MAX_INPUT_VOLTAGE 10.0 // max voltage we can read

#define REF_VOLTAGE 3.3

#if USE_NEW_V_REFERENCE // for v3 wheel

// The values below are what I measured on the individual resistors
#define RESISTOR_Z1_VALUE 17870 // 18k
#define RESISTOR_Z2_VALUE 9920 // 10k resistor

#else
// in v1 wheel

#define RESISTOR_Z1_VALUE 10000.0 // 10k resistor
#define RESISTOR_Z2_VALUE 4680.0 // MEASURED value of 4.7k resistor

#endif

static Button g_button = Button(BUTTON_PIN);
static CWPatternSequenceManager g_sequenceManager;


void buttonClicked(Button &b){
    g_sequenceManager.buttonClick();
}

void buttonHeld(Button &b) {
    g_sequenceManager.buttonLongClick();
}

#define DEBUG_VOLTAGE 0
// corbin!! voltage debug on..

static float readBatteryVoltage() {
    
    float readValue = analogRead(g_batteryVoltagePin); // returns 0 - [max resolution]; 0 is 0v, and 65535 is ~9.?v
    
    // It is a percentage of the resolution; I have it set to:analogReadRes(16) , or 16-bits...plus 0 (the addition of 1)
    float readValuePercetnage = readValue / (UINT16_MAX + 1);
    float readValueInVoltage = readValuePercetnage * REF_VOLTAGE; // Over REF_VOLTAGE max volts input
    float voltage = readValueInVoltage * (RESISTOR_Z1_VALUE + RESISTOR_Z2_VALUE)/RESISTOR_Z2_VALUE;

#if DEBUG_VOLTAGE
    Serial.printf("vRead: %.3f, readValuePercetnage: %.3f, readValueInVoltage: %.3f, voltage: %.3f\r\n",readValue, readValuePercetnage, readValueInVoltage, voltage);
#endif

    return voltage;
}

#if BLUETOOTH
Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);
#endif


void setup() {
#if DEBUG
    Serial.begin(9600);
    delay(1000);
    Serial.println("--- begin serial --- (WARNING: delay on start!!)");
#endif
    // we NEED to do this to avoid conflicting with the other chip selects. TODO: set the OTHER ones I use on SPI as neccessary. This is REQUIRED. Wifi had it missing, and could have been an issue.
    pinMode(SD_CARD_CS_PIN, OUTPUT);
    digitalWrite(SD_CARD_CS_PIN, HIGH);
    
    
#if BLUETOOTH
    pinMode(ADAFRUITBLE_REQ, OUTPUT);
    digitalWrite(ADAFRUITBLE_REQ, HIGH);
#endif
    
    pinMode(g_batteryVoltagePin, INPUT);
//    pinMode(g_batteryRefPin, INPUT); // TODO: I just re-enabled this. Make sure it works again

    analogReadAveraging(16); // longer averaging of reads; drastically stabilizes my battery voltage read compared to the default of 4
    analogReadRes(16); // 16 bit analog read resolution

    Wire.begin();
    
    SPI.begin(); // Do this early??
    
    g_button.clickHandler(buttonClicked);
    g_button.holdHandler(buttonHeld);

    g_button.process();
    g_sequenceManager.init(g_button.isPressed());

#if DEBUG
#if IGNORE_VOLTAGE
    // Having this on could be bad..flash red
    g_sequenceManager.getLEDPatterns()->flashThreeTimesWithDelay(CRGB::Red, 150);
#else
    g_sequenceManager.getLEDPatterns()->flashOnce(CRGB::Red);
#endif
#else
    //    g_sequenceManager.getLEDPatterns()->flashOnce(CRGB::Maroon);
#endif
    if (g_sequenceManager.getCardInitPassed()) {
        // See if we read more than the default sequence
        if (g_sequenceManager.getNumberOfSequenceNames() > 1) {
            //flashNTimes(0, 255, 0, 1, 150); // Don't do anything..
        } else {
            g_sequenceManager.getLEDPatterns()->flashThreeTimesWithDelay(CRGB::Violet, 150);
        }
    } else {
        // Flash the LEDs all red to indicate no card...
        g_sequenceManager.getLEDPatterns()->flashThreeTimesWithDelay(CRGB::Orange, 150);
        g_sequenceManager.loadDefaultSequence();
    }

#if BLUETOOTH
    BTLEserial.setDeviceName("cyrweel"); /* 7 characters max! */
    BTLEserial.begin();
#endif
}


bool checkVoltage() {
#if DEBUG
    if (IGNORE_VOLTAGE) return true;
#endif
    // check the voltage; if we are low, flast red 3 times at a low brightness...
    static uint32_t lastReadVoltageTime = 0;
    if (millis() - lastReadVoltageTime > TIME_BETWEEN_VOLTAGE_READS) {
        lastReadVoltageTime = millis();
        float voltage = readBatteryVoltage();
        if (voltage < LOW_VOLTAGE_VALUE) {
            DEBUG_PRINTF("---------------------- LOW BATTERY VOLTAGE: %f\r\n", voltage);
            // half the max brightness...
            g_sequenceManager.setLowBatteryWarning();
            g_sequenceManager.getLEDPatterns()->flashThreeTimesWithDelay(CRGB::Red, 150);
            
            delay(2000); // delay for 2 seconds to give the user time to react and turn it off
            return false;
        }
    }
    return true;
}

#if BLUETOOTH
aci_evt_opcode_t laststatus =  ACI_EVT_CONNECTED;
#endif


void loop() {
    g_button.process();
    // Don't show the LED pattern if we have too low of voltage...
    if (checkVoltage()) {
        g_sequenceManager.process();
#if BLUETOOTH
        // Tell the nRF8001 to do whatever it should be working on.
        BTLEserial.pollACI();
        
        // Ask what is our current status
        aci_evt_opcode_t status = BTLEserial.getState();
        // If the status changed....
        if (status != laststatus) {
            // print it out!
            if (status == ACI_EVT_DEVICE_STARTED) {
                Serial.println(F("* Advertising started"));
            }
            if (status == ACI_EVT_CONNECTED) {
                Serial.println(F("* Connected!"));
            }
            if (status == ACI_EVT_DISCONNECTED) {
                Serial.println(F("* Disconnected or advertising timed out"));
            }
            // OK set the last status change to this one
            laststatus = status;
        }
        
        if (status == ACI_EVT_CONNECTED) {
            // Lets see if there's any data for us!
            if (BTLEserial.available()) {
                Serial.print("* "); Serial.print(BTLEserial.available()); Serial.println(F(" bytes available from BTLE"));
            }
            // OK while we still have something to read, get a character and print it out
            while (BTLEserial.available()) {
                char c = BTLEserial.read();
                Serial.print(c);
            }
            
            // Next up, see if we have any data to get from the Serial console
            
            if (Serial.available()) {
                // Read a line from Serial
                Serial.setTimeout(100); // 100 millisecond timeout
                String s = Serial.readString();
                
                // We need to convert the line to bytes, no more than 20 at this time
                uint8_t sendbuffer[20];
                s.getBytes(sendbuffer, 20);
                char sendbuffersize = min(20, s.length());
                
                Serial.print(F("\n* Sending -> \"")); Serial.print((char *)sendbuffer); Serial.println("\"");
                
                // write the data
                BTLEserial.write(sendbuffer, sendbuffersize);
            }
        }
        
#endif
    }
}
