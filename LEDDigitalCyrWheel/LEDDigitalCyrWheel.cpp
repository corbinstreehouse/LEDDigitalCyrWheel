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
#include "CDWheelBluetoothController.h"
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
#if BLUETOOTH
static CDWheelBluetoothController g_bluetoothController;
#endif


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

void setup() {
#if DEBUG
    Serial.begin(19200);
    delay(5);
    Serial.println("--- begin serial --- (WARNING: delay on start!!)");
#endif
    // we NEED to do this to avoid conflicting with the other chip selects. TODO: set the OTHER ones I use on SPI as neccessary. This is REQUIRED. Wifi had it missing, and could have been an issue.
    pinMode(SD_CARD_CS_PIN, OUTPUT);
    digitalWrite(SD_CARD_CS_PIN, HIGH);
    
#if BLUETOOTH
    pinMode(BLUETOOTH_CS, OUTPUT);
    digitalWrite(BLUETOOTH_CS, HIGH);
#endif
    
    pinMode(g_batteryVoltagePin, INPUT);
#warning battery ref hooked up????
//    pinMode(g_batteryRefPin, INPUT); // TODO: I just re-enabled this. Make sure it works again

    analogReadAveraging(16); // longer averaging of reads; drastically stabilizes my battery voltage read compared to the default of 4
    analogReadRes(16); // 16 bit analog read resolution

    
    Wire.begin();
    
    SPI.begin(); // Do this early??
    
    g_button.clickHandler(buttonClicked);
    g_button.holdHandler(buttonHeld);

    g_button.process();
    g_sequenceManager.init();

#if BLUETOOTH
    g_bluetoothController.init(&g_sequenceManager, g_button.isPressed());
#endif
    

#if DEBUG
    #if IGNORE_VOLTAGE
        // Having this on could be bad..flash red just to let me know...
        g_sequenceManager.flashThreeTimes(CRGB::Red);
    #else
        g_sequenceManager.getLEDPatterns()->flashOnce(CRGB::Red);
    #endif
#endif
    
    if (g_sequenceManager.getCardInitPassed()) {
        // See if we read more than the default sequence
        if (g_sequenceManager.getRootNumberOfSequenceFilenames() > 1) {
            //flashNTimes(0, 255, 0, 1, 150); // Don't do anything..
        } else {
            g_sequenceManager.flashThreeTimes(CRGB::Violet);
        }
    } else {
        // Flash the LEDs all red to indicate no card...
        g_sequenceManager.flashThreeTimes(CRGB::Orange);
    }

    // Start the patterns by loading the first sequence, if it didn't work
    // NOTE: the loading already did this. This just reloads, which is stupid.
//    g_sequenceManager.loadFirstSequence();
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
            g_sequenceManager.getLEDPatterns()->flashThreeTimes(CRGB::Red, 150);
            
            delay(2000); // delay for 2 seconds to give the user time to react and turn it off
            return false;
        }
    }
    return true;
}

void loop() {
    g_button.process();
#if BLUETOOTH
    g_bluetoothController.process();
#endif
    // Don't show the LED pattern if we have too low of voltage; checkVoltage will slow down our processing with explicit delays
    if (checkVoltage()) {
        g_sequenceManager.process();
    }
}
