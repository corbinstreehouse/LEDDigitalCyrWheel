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
// warning!! debug on

#if PATTERN_EDITOR
    #define USE_ADAFRUIT 0
    #define WIFI 0
    #define BLUETOOTH 0
#else
    #define USE_ADAFRUIT 1// 0 uses fast LED
    #define WIFI 0 // disabled!!
    #define BLUETOOTH 1
#endif

#if DEBUG
#warning "DEBUG CODE IS ON!!!! LEDCommon.h"
    #define DEBUG_PRINTLN(a) Serial.println(a)
    #define DEBUG_PRINTF(a, ...) Serial.printf(a, ##__VA_ARGS__)
#else
    #define DEBUG_PRINTLN(a)
    #define DEBUG_PRINTF(a, ...)
#endif

#endif

#define SD_CARD_SUPPORT 1


// Defined here so I can use it in multiple places more easily
#define STRIP_PIN 2 // 14 // Use pin 2 so Octo works, and pin 14 for a secondary strip (opposite side) to do patterns
// CORBIN: led cyr wheel is wired to pin 2


#if DEBUG
    #if PATTERN_EDITOR
        #define ASSERT(a) NSCAssert(a, @"Fail");
    #else
        #define ASSERT(a) if (!(a)) { \
            Serial.print("ASSERT ");  \
            Serial.print(__FILE__); Serial.print(" : "); \
            Serial.println(__LINE__); }
    #endif
#else
    #if PATTERN_EDITOR
        #define ASSERT(a) NSCAssert(a, @"Fail");
    #else
        #define ASSERT(a) ((void)0)
    #endif
#endif

#define MIN_BRIGHTNESS 64 // abitrary..
#define MAX_BRIGHTNESS 200 // usually 128
#define DEFAULT_BRIGHTNESS 128



/*
Notes: 
 Violet flash 3 times on start: no data on SD card
 Orange flash 3 times on start: SD initialization failed (bad SD card or wiring bad)
 Red flash 3 times while running, followed by 2 second delay, and dimmer LEDs: low voltage, turn the wheel off soon!
 

 Red flash 1 times on start: debug code is on.
 Red flash 3 times on start: debug code is on and ignoring voltage, so be careful.
 
 Yellow flash 3 times, then delay: means no pattern, or the pattern file couldn't be read, or was corrupted, or out of date.

 Hold the button down when starting: Goes into calibration mode. Should flash pink. Rotate the wheel amongst each axist to calibrate. Push the button to end calibration and save the state. Turn it off to cancel.
 ^^^ calibration now done differently...
 
 Blue on start: initializing bluetooth
 Blue flash three times on start: problem with bluetooth initialization
 
 If a file on the SD card called "record.txt" exists:
    Long click: Three green flashes: starting to save data to the card.
    Long click: Three blue flashes: end saving data to the card.

 
*/