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

// TODO: corbin remove this enum and use the other one..
typedef enum {
    CDDisplayModeMin = 0,
    CDDisplayModeShowOn = 0,
    CDDisplayModeRainbow,
    CDDisplayModeColorWipe,
    CDDisplayModeImagePlayback,
    CDDisplayModeImageLEDGradient,
    CDDisplayModeRainbow2,
    CDDisplayPluseGradientEffect,
    
    CDDisplayModeWarmWhiteShimmer,
    CDDisplayModeRandomColorWalk,
    CDDisplayModeTraditionalColors,
    CDDisplayModeColorExplosion,
    CDDisplayModeGradient,
    CDDisplayModeBrightTwinkle,
    CDDisplayModeCollision,
    
    CDDisplayModeMax,
} CDDisplayMode;


extern void warmWhiteShimmer(unsigned char dimOnly);
extern void randomColorWalk(unsigned char initializeColors, unsigned char dimOnly);
extern void traditionalColors();
extern void brightTwinkleColorAdjust(unsigned char *color);
extern void colorExplosionColorAdjust(unsigned char *color, unsigned char propChance,
                               unsigned char *leftColor, unsigned char *rightColor);
extern void colorExplosion(unsigned char noNewBursts);
extern void gradient();
extern void brightTwinkle(unsigned char minColor, unsigned char numColors, unsigned char noNewBursts);
extern unsigned char collision();


extern void stripPatternLoop(CDDisplayMode g_displayMode); // Call this on each loop to process things
extern void flashColor(uint8_t r, uint8_t g, uint8_t b, uint32_t delay);
extern void stripInit();
extern void stripUpdateBrightness();

#endif
