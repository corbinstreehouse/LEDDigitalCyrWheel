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

#define DEBUG 1

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
#if USE_ADAFRUIT
extern Adafruit_NeoPixel g_strip;
#else
extern CDOctoWS2811 g_strip;
#endif
#endif


//extern void warmWhiteShimmer(unsigned char dimOnly);
//extern void randomColorWalk(unsigned char initializeColors, unsigned char dimOnly);
//extern void traditionalColors();
//extern void brightTwinkleColorAdjust(unsigned char *color);
//extern void colorExplosionColorAdjust(unsigned char *color, unsigned char propChance,
//                               unsigned char *leftColor, unsigned char *rightColor);
//extern void colorExplosion(unsigned char noNewBursts);
//extern void gradient();
//extern void brightTwinkle(unsigned char minColor, unsigned char numColors, unsigned char noNewBursts);
//extern unsigned char collision();


extern void stripPatternLoop(CDPatternType patternType); // Call this on each loop to process things
extern void flashColor(uint8_t r, uint8_t g, uint8_t b, uint32_t delay);
extern void stripInit();
extern void stripUpdateBrightness();

#endif
