//
//  NeoPixelLEDPatterns.h
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 5/31/14.
//
//

#ifndef __LEDDigitalCyrWheel__NeoPixelLEDPatterns__
#define __LEDDigitalCyrWheel__NeoPixelLEDPatterns__

#include "LEDPatterns.h"
#include "Adafruit_NeoPixel.h"


class NeoPixelLEDPatterns : LEDPatterns {
private:
    Adafruit_NeoPixel m_strip;
protected:
    virtual void internalShow();
    
public:
    NeoPixelLEDPatterns(uint32_t ledCount, uint8_t pin=6, uint8_t t=NEO_GRB + NEO_KHZ800) : LEDPatterns(ledCount), m_strip(ledCount, pin, t) {
        
    }

    virtual void begin();
};


#endif /* defined(__LEDDigitalCyrWheel__NeoPixelLEDPatterns__) */
