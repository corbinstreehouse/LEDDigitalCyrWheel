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

// 2 cell LiPO, 4.2v each: 8.4v max. 3.0v should be the min, 3.0*2=6v min
#define LOW_VOLTAGE_VALUE 6.4 // min voltage for 2 cells....I was seeing values "normally" from 7.57+ on up...probably due to voltage sag when illuminating things. I might have to average the voltage over time to see what i am really getting, or lower the min value.
#define TIME_BETWEEN_VOLTAGE_READS 1000 // read every second..
#define MAX_INPUT_VOLTAGE 10.0 // max voltage we can read
#define RESISTOR_Z1_VALUE 10000.0 // 10k resistor
#define RESISTOR_Z2_VALUE 4680.0 // MESURED value of 4.7k resistor

static Button g_button = Button(BUTTON_PIN);
static CWPatternSequenceManager g_sequenceManager;

bool mainProcess() {
    g_button.process();
    if (g_button.isPressed()) {
        return true;
    }
//    stripUpdateBrightness();
    return false; // Button not pressed
}


bool busyDelay(uint32_t ms)
{
	uint32_t start = micros();
    
	if (ms > 0) {
		while (1) {
            if (mainProcess()) return true;
			if ((micros() - start) >= 1000) {
				ms--;
				if (ms == 0) return false;
				start += 1000;
			}
			yield();
		}
	}
    return false;
}

void buttonClicked(Button &b){
    g_sequenceManager.buttonClick();
}

void buttonHeld(Button &b) {
    g_sequenceManager.buttonLongClick();
}

#define DEBUG_VOLTAGE 0

static float readBatteryVoltage() {
    float readValue = analogRead(g_batteryVoltagePin); // returns 0 - 1023; 0 is 0v, and 1023 is 10v (since I use two 10k resistors in a voltage divider). 5v is the max...
#if DEBUG_VOLTAGE
    Serial.print("vread: ");
    Serial.print(readValue);
    Serial.print(" ");
#endif
    float vRef = 3.30; // 3.3v ref
    float refValue = analogRead(g_batteryRefPin);
#if DEBUG_VOLTAGE
    Serial.print("refValue: ");
    Serial.print(refValue);
    Serial.print(" ");
#endif
    
    float voltageReadValue = readValue / refValue * vRef; // 16 bit resolution,
#if DEBUG_VOLTAGE
    Serial.print("vreadV: ");
    Serial.print(voltageReadValue);
    Serial.print(" ");
#endif
    
    // The read value is with regards to the input voltage
    // See voltage dividor for reference
    float voltage = voltageReadValue * (RESISTOR_Z1_VALUE + RESISTOR_Z2_VALUE)/RESISTOR_Z2_VALUE;
#if DEBUG_VOLTAGE
    Serial.print("voltage: ");
    Serial.println(voltage);
#endif
    return voltage;
}

void setup() {
//    pinMode(g_LED, OUTPUT);
//    digitalWrite(g_LED, HIGH);
#if DEBUG
    Serial.begin(9600);
    delay(1000);
    Serial.println("--- begin serial --- ");
#endif
    
    pinMode(g_batteryVoltagePin, INPUT);
    pinMode(g_batteryRefPin, INPUT);

    analogReadAveraging(16); // longer averaging of reads; drastically stabilizes my battery voltage read compared to the default of 4
    analogReadRes(16); // 16 bit analog read resolution

    Wire.begin();

    g_button.clickHandler(buttonClicked);
    g_button.holdHandler(buttonHeld);

    g_button.process();
    bool initPassed = g_sequenceManager.init(g_button.isPressed());
    if (initPassed) {
#if DEBUG
#if IGNORE_VOLTAGE
      // Having this on could be bad..flash red
        flashThreeTimes(255, 0, 0, 150); //
#endif
#endif
        // See if we read more than the default sequence
        if (g_sequenceManager.getNumberOfSequenceNames() > 1) {
            //flashNTimes(0, 255, 0, 1, 150); // Don't do anything..
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
            DEBUG_PRINTF("---------------------- LOW BATTERY VOLTAGE: %f", voltage);
            // half the max brightness...
            g_sequenceManager.setLowBatteryWarning();
            
            flashThreeTimes(255, 0, 0, 150); // flash red
            
            delay(2000); // delay for 2 seconds to give the user time to react and turn it off
            return false;
        }
    }
    return true;
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
    if (checkVoltage()) {
        g_sequenceManager.process(false);
    }
}
