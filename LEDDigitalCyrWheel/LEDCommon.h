//
//  LEDCommon.h
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 6/1/14.
//
//

#ifndef LEDDigitalCyrWheel_LEDCommon_h
#define LEDDigitalCyrWheel_LEDCommon_h

#define DEBUG 1

#if PATTERN_EDITOR
    #define USE_ADAFRUIT 0
#else
    #define USE_ADAFRUIT 1
#endif

#if DEBUG
    #define DEBUG_PRINTLN(a) Serial.println(a)
    #define DEBUG_PRINTF(a, ...) Serial.printf(a, ##__VA_ARGS__)
#else
    #define DEBUG_PRINTLN(a)
    #define DEBUG_PRINTF(a, ...)
#endif

#endif



#if DEBUG
#if PATTERN_EDITOR
    #define ASSERT(a) NSCAssert(a, @"Fail");
#else
    #define ASSERT(a) if (!(a)) { \
        Serial.print("ASSERT ");  \
        Serial.print(__FILE__); Serial.print(" : "); \
        Serial.print(__LINE__); }
#endif

#else
    #define ASSERT(a) ((void)0)
#endif


/*
Notes: 
 Violet flash 3 times on start: no data on SD card
 Orange flash 3 times on start: SD initialization failed (bad SD card or wiring bad)
 Red flash 3 times while running, followed by 2 second delay, and dimmer LEDs: low voltage, turn the wheel off soon!
 

 Red flash 1 times on start: debug code is on.
 Red flash 3 times on start: debug code is on and ignoring voltage, so be careful.
 
 Yellow flash 3 times, then delay: means no pattern, or the pattern file couldn't be read, or was corrupted, or out of date.

 Hold the button down when starting: Goes into calibration mode. Should flash pink. Rotate the wheel amongst each axist to calibrate. Push the button to end calibration and save the state. Turn it off to cancel.
 
 If a file on the SD card called "record.txt" exists:
    Long click: Three green flashes: starting to save data to the card.
    Long click: Three blue flashes: end saving data to the card.

 
*/