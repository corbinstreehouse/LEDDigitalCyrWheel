//
//  CDOctoWS2811.h
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/11/14.
//
//

#ifndef LEDDigitalCyrWheel_CDOctoWS2811_h
#define LEDDigitalCyrWheel_CDOctoWS2811_h

#include <stdint.h>
#include <OctoWS2811.h>

typedef struct {
    // TODO: maybe add #define support for GRB vs RGB
    uint8_t green, red, blue; // grb format
} rgb_color;


class CDOctoWS2811 : public OctoWS2811 {
private:
    uint8_t brightness;
public:
    inline void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
        if (brightness) { // See notes in setBrightness()
            r = (r * brightness) >> 8;
            g = (g * brightness) >> 8;
            b = (b * brightness) >> 8;
        }
        this->setPixel(this->numPixels(), r, g, b); // hack...not using first input length LED. TODO: specify the strip??
    }
    inline void setPixelColor(uint16_t n, uint32_t c) {
        uint8_t
        r = (uint8_t)(c >> 16),
        g = (uint8_t)(c >>  8),
        b = (uint8_t)c;
        this->setPixelColor(n, r, g, b);
    }
    inline CDOctoWS2811(uint32_t numPerStrip, void *frameBuf, void *drawBuf, uint8_t config = WS2811_GRB) : OctoWS2811(numPerStrip, frameBuf, drawBuf, config) {
    }
    inline uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
    }
    inline int numPixels(void) {
		return stripLen;
	}
    inline void setBrightness(uint8_t b) {
        brightness = b;
    }
    inline uint16_t getNumberOfBytes() { return stripLen*3; };
    inline uint8_t *getPixels() const { return (uint8_t *)drawBuffer; }


};


#endif
