//
//  CyrWheelLEDPatterns.hpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 1/17/16 .
//
//

#ifndef CyrWheelLEDPatterns_hpp
#define CyrWheelLEDPatterns_hpp

#include "LEDPatterns.h"
#include "FastLED.h"
#include "SPI.h"

// NOTE: this class is rather hardcoded for these SPI pins. I could make it work with 11/13 (the standard pins) too.
// XXXX: See CyrWheelLEDPatterns::_spiEnd if these are changed!
#define APA102_LED_DATA_PIN 7 // alternate MOSI pin
#define APA102_LED_CLOCK_PIN 14 // alternate CLK pin

class CyrWheelLEDPatterns : public LEDPatterns {
private:
    void _spiBegin();
    void _spiEnd();
    void show2();
public:
    CyrWheelLEDPatterns(uint32_t ledCount);

    virtual void internalShow();
    virtual void setBrightness(uint8_t brightness);
    uint8_t getBrightness();
    
    virtual void begin();
};


#endif /* CyrWheelLEDPatterns_hpp */
