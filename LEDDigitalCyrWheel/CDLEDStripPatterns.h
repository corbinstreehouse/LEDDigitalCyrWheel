//
//  CDLEDStripPatterns.h
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/11/14.
//
//

#ifndef LEDDigitalCyrWheel_CDLEDStripPatterns_h
#define LEDDigitalCyrWheel_CDLEDStripPatterns_h

#include <stdint.h>
#include "CDPatternData.h"

#define DEBUG 0

#define USE_ADAFRUIT 1

#if USE_ADAFRUIT
#include "Adafruit_NeoPixel.h"
#define STRIP_CLASS Adafruit_NeoPixel
#else
#include "CDOctoWS2811.h"
#define STRIP_CLASS CDOctoWS2811
#endif

// only the pattern editor needs to access the exported global g_strip. Ideally if this c file was a class I could instantiate it and pass it around..
#if PATTERN_EDITOR
extern STRIP_CLASS g_strip;
#endif

extern void stripPatternLoop(CDPatternItemHeader *itemHeader, uint32_t intervalCount, uint32_t timePassed, bool isFirstPass); // Call this on each loop to process things

void flashThreeTimes(uint8_t r, uint8_t g, uint8_t b, uint32_t delay);
void flashNTimes(uint8_t r, uint8_t g, uint8_t b, uint32_t n, uint32_t delay);
extern void stripInit();
extern void stripUpdateBrightness();
extern void stripSetLowBatteryBrightness();

#endif
