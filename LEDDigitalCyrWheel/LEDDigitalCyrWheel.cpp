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
//#include <EEPROM.h>
//#include <avr/eeprom.h>

#include <stdint.h> // We can compile without this, but it kills xcode completion without it! it took me a while to discover that..

#include "CWPatternSequenceManager.h"
#include "CDPatternData.h"
#include "CDLEDStripPatterns.h"
#include "LEDDigitalCyrWheel.h"

#define BUTTON_PIN 23


const int g_LED = LED_BUILTIN;

static Button g_button = Button(BUTTON_PIN);
static CWPatternSequenceManager g_sequenceManager;


bool mainProcess() {
    g_button.process();
    if (g_button.isPressed()) {
        return true;
    }
    stripUpdateBrightness();
    return false; // Button not pressed
}

static void flashThreeTimes(uint8_t r, uint8_t g, uint8_t b, uint32_t delay) {
    for (int i = 0; i < 3; i++) {
        flashColor(r, g, b, delay);
        flashColor(0, 0, 0, delay);
    }
}

void buttonClicked(Button &b){
    g_sequenceManager.nextPatternItem();
}

void buttonHeld(Button &b) {
    g_sequenceManager.loadNextSequence();
}

void setup() {
    Wire.begin();
//    pinMode(g_LED, OUTPUT);
//    digitalWrite(g_LED, HIGH);
#if DEBUG
    Serial.begin(9600);
    delay(1000);
    Serial.println("--- begin serial --- ");
#endif

    stripInit();
    
    g_button.clickHandler(buttonClicked);
    g_button.holdHandler(buttonHeld);

    bool initPassed = g_sequenceManager.init();
    if (initPassed) {
        // See if we read more than the default sequence
        if (g_sequenceManager.getNumberOfSequenceNames() > 1) {
            flashThreeTimes(0, 255, 0, 150); // flash green
        } else {
            flashThreeTimes(255, 127, 0, 150); // flash orange...couldn't find any data files
        }
    } else {
        // Flash the LEDs all red to indicate no card...
        flashThreeTimes(255, 0, 0, 300);
        g_sequenceManager.loadDefaultSequence();
    }
    
 //   digitalWrite(g_LED, LOW);
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
    g_sequenceManager.process(false);
}
