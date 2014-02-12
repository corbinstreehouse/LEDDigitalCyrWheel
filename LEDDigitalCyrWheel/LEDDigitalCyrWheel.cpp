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
//#include <Wire.h>
//#include <HardwareSerial.h>
//#include <EEPROM.h>
//#include <avr/eeprom.h>

#include <stdint.h> // We can compile without this, but it kills xcode completion without it! it took me a while to discover that..

#include "CWPatternSequenceManager.h"
#include "CDPatternData.h"
#include "CDLEDStripPatterns.h"
#include "LEDDigitalCyrWheel.h"

#define BUTTON_PIN 23


const int g_LED = LED_BUILTIN;

static char g_displayMode = CDDisplayModeImageLEDGradient; // ImagePlayback; // Not CDDisplayMode so I can easily do math with it..
static Button g_button = Button(BUTTON_PIN);
static CWPatternSequenceManager g_sequenceManager;


void mainProcess() {
    g_button.process();
    stripUpdateBrightness();
}

bool mainShouldExitEarly() {
    mainProcess();
    return g_button.isPressed();
}

void buttonClicked(Button &b);


void flashThreeTimes(uint8_t r, uint8_t g, uint8_t b, uint32_t delay) {
    for (int i = 0; i < 3; i++) {
        flashColor(r, g, b, delay);
        flashColor(0, 0, 0, delay);
    }
}


void setup() {
    pinMode(g_LED, OUTPUT);
    digitalWrite(g_LED, HIGH);
#if DEBUG
    Serial.begin(9600);
    delay(1000);
    Serial.println("--- begin serial --- ");
#endif

    stripInit();
    
    g_button.pressHandler(buttonClicked);

    bool initPassed = g_sequenceManager.init();
    if (initPassed) {
        // See if we could read from the SD card
        if (g_sequenceManager.loadFirstSequence()) {
            flashThreeTimes(0, 255, 0, 150); // flash green
        } else {
            flashThreeTimes(255, 127, 0, 150); // flash orange...couldn't find any data files
        }
    } else {
        // Flash the LEDs all red to indicate no card...
        flashThreeTimes(255, 0, 0, 300);
        g_sequenceManager.loadDefaultSequence();
    }
    
    digitalWrite(g_LED, LOW);
}


void buttonClicked(Button &b){
#if DEBUG
//	Serial.printf("onPress, mode %d\r\n", g_displayMode);
#endif
    g_displayMode = g_displayMode++;
    if (g_displayMode >= CDDisplayModeMax) {
        g_displayMode = CDDisplayModeMin;
    }
//    resetState(); // corbin??
}

void loop() {
    mainProcess();
    
#if DEBUG
    if (g_button.isPressed()){
        digitalWrite(g_LED, HIGH);
    } else {
        digitalWrite(g_LED, LOW);
    }
#endif
    
    stripPatternLoop((CDDisplayMode)g_displayMode);
}
