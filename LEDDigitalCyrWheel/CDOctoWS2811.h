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
    uint8_t m_brightness;
public:
    inline void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
        if (m_brightness) { // See notes in setBrightness()
            r = (r * m_brightness) >> 8;
            g = (g * m_brightness) >> 8;
            b = (b * m_brightness) >> 8;
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
    
    inline CDOctoWS2811(uint32_t numPerStrip, uint8_t config = WS2811_GRB) : OctoWS2811(numPerStrip, NULL, NULL, config)  {
        int byteCount = numPerStrip*24; // whatever...not sure why 24..
        frameBuffer = malloc(byteCount);
        drawBuffer = malloc(byteCount);
    }
    
    ~CDOctoWS2811();
    
    inline uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
    }
    inline int numPixels(void) {
		return stripLen;
	}
    
    inline void setBrightness(uint8_t b) {
        // Stored brightness value is different than what's passed.
        // This simplifies the actual scaling math later, allowing a fast
        // 8x8-bit multiply and taking the MSB.  'brightness' is a uint8_t,
        // adding 1 here may (intentionally) roll over...so 0 = max brightness
        // (color values are interpreted literally; no scaling), 1 = min
        // brightness (off), 255 = just below max brightness.
        uint8_t newBrightness = b + 1;
        if (newBrightness != m_brightness) { // Compare against prior value
            // Brightness has changed -- re-scale existing data in RAM
            // TODO: Corbin...this stuff
//            uint8_t  c,
//            *ptr           = frameBuffer,
//            oldBrightness = m_brightness - 1; // De-wrap old brightness value
//            uint16_t scale;
//            if(oldBrightness == 0) scale = 0; // Avoid /0
//            else if(b == 255) scale = 65535 / oldBrightness;
//            else scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
//            for(uint16_t i=0; i<_numberOfBytes; i++) {
//                c      = *ptr;
//                *ptr++ = (c * scale) >> 8;
//            }
            m_brightness = newBrightness;
        }

    }
    uint8_t getBrightness() { return m_brightness - 1; };

    inline uint16_t getNumberOfBytes() { return stripLen*3; };
    inline uint8_t *getPixels() const { return (uint8_t *)drawBuffer; }


};


#endif
