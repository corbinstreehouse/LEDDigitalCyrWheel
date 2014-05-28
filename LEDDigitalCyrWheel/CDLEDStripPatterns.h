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
#include "CDOrientation.h"

#define DEBUG 0
#define IGNORE_VOLTAGE 0 //for hardware testing w/out a battery

#define USE_ADAFRUIT 1

#if DEBUG
    #define DEBUG_PRINTLN(a) Serial.println(a)
    #define DEBUG_PRINTF(a, ...) Serial.printf(a, ##__VA_ARGS__)
#else
    #define DEBUG_PRINTLN(a)
    #define DEBUG_PRINTF(a, ...)
#endif


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

void flashThreeTimes(uint8_t r, uint8_t g, uint8_t b, uint32_t delay);
void flashThreeTimesNoProcess(uint8_t r, uint8_t g, uint8_t b, uint32_t delay);
void flashNTimes(uint8_t r, uint8_t g, uint8_t b, uint32_t n, uint32_t delay);
void setEntireStripAsColor(uint8_t r, uint8_t g, uint8_t b);
extern void stripInit(CDOrientation *orientation);

#endif


class CDLEDPatternManager {
private:
    STRIP_CLASS *m_strip;
    CDPatternItemHeader *m_itemHeader;
    uint32_t m_intervalCount;
    uint32_t m_timePassedInMS; // In MS
    bool m_isFirstPass;
    
    // velocity based state
    double m_maxVelocity;
    uint8_t m_targetBrightness;
    uint8_t m_startBrightness;
    uint32_t m_brightnessStartTime;

    uint8_t m_savedBrightness;
    
    CDOrientation *m_orientation;
    
    void resetState();
    
    
    void updateBrightnessBasedOnVelocity();
    void updateBrightness();

    
    // Pattern implementations
public:
    CDLEDPatternManager() : m_maxVelocity(0), m_brightnessStartTime(0) {  }
    void init(STRIP_CLASS *strip, CDOrientation *orientation);
    
    void stripPatternLoop(CDPatternItemHeader *itemHeader, uint32_t intervalCount, uint32_t timePassedInMS, bool isFirstPass);
    void setSavedBrightness(uint8_t b) { m_savedBrightness = b; };
};

extern CDLEDPatternManager g_patternManager;
