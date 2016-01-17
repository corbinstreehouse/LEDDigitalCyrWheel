//
//  CyrWheelLEDPatterns.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 1/17/16 .
//
//

#include "CyrWheelLEDPatterns.h"

#define USE_FAST_LED 1
#define USE_MANUAL_SPI 0

#if USE_FAST_LED

template <uint8_t DATA_PIN, uint8_t CLOCK_PIN, EOrder RGB_ORDER = BGR, uint8_t SPI_SPEED = DATA_RATE_MHZ(24)>
class CD_APA102Controller : public CLEDController {
//    typedef SPIOutput<DATA_PIN, CLOCK_PIN, SPI_SPEED> SPI;
//    SPI mSPI;
    
//    void startBoundary() { mSPI.writeWord(0); mSPI.writeWord(0); }
//    void endBoundary(int nLeds) { int nBytes = (nLeds/32); do { mSPI.writeByte(0xFF); mSPI.writeByte(0x00); mSPI.writeByte(0x00); mSPI.writeByte(0x00); } while(nBytes--); }
    
//    inline void writeLed(uint8_t b0, uint8_t b1, uint8_t b2) __attribute__((always_inline)) {
//        mSPI.writeByte(0xFF); mSPI.writeByte(b0); mSPI.writeByte(b1); mSPI.writeByte(b2);
//    }
    
public:
    CD_APA102Controller() {}
    
    virtual void init() {
//        mSPI.init();
    }
    
    virtual void clearLeds(int nLeds) {
//        showColor(CRGB(0,0,0), nLeds, CRGB(0,0,0));
    }
    
protected:
    
    virtual void showColor(const struct CRGB & data, int nLeds, CRGB scale) {
        // TODO: implement!!
//        PixelController<RGB_ORDER> pixels(data, nLeds, scale, getDither());
//        
//        mSPI.select();
//        
////        startBoundary();
//        for(int i = 0; i < nLeds; i++) {
//#ifdef FASTLED_SPI_BYTE_ONLY
//            mSPI.writeByte(0xFF);
//            mSPI.writeByte(pixels.loadAndScale0());
//            mSPI.writeByte(pixels.loadAndScale1());
//            mSPI.writeByte(pixels.loadAndScale2());
//#else
//            uint16_t b = 0xFF00 | (uint16_t)pixels.loadAndScale0();
//            mSPI.writeWord(b);
//            uint16_t w = pixels.loadAndScale1() << 8;
//            w |= pixels.loadAndScale2();
//            mSPI.writeWord(w);
//#endif
//            pixels.stepDithering();
//        }
////        endBoundary(nLeds);
//        
//        mSPI.waitFully();
//        mSPI.release();
    }
    
    virtual void show(const struct CRGB *data, int nLeds, CRGB scale) {
        PixelController<RGB_ORDER> pixels(data, nLeds, scale, getDither());
        
        for(int i=0; i<3; i++) {
            SPI.transfer(0x00);    // First 3 start-frame bytes
        }
        SPDR = 0x00;                         // 4th is pipelined
        
        for(int i = 0; i < nLeds; i++) {
            while(!(SPSR & _BV(SPIF)));        //  Wait for prior byte out
            
            SPDR = 0xFF;                       //  Pixel start
            uint8_t b = pixels.loadAndScale0();
            while(!(SPSR & _BV(SPIF)));      //   Wait for prior byte out
            SPDR = b;

            
            b = pixels.loadAndScale1();
            while(!(SPSR & _BV(SPIF)));      //   Wait for prior byte out
            SPDR = b;

            
            b = pixels.loadAndScale2();
            while(!(SPSR & _BV(SPIF)));      //   Wait for prior byte out
            SPDR = b;
            
            pixels.stepDithering();
            pixels.advanceData();
        }
        
        while(!(SPSR & _BV(SPIF)));          // Wait for last byte out
        
        //corbin!!!! this is needed to discard any data that was sent/left on the pipe.. Otherwise it kills the Bluetooth. I need to find out why this really is... as I suspect the bluetooth
        SPI0_MCR |= SPI_MCR_CLR_RXF; // discard any received data
        
        //ending marker..
        for(int i=0; i<4; i++) SPI.transfer(0xFF);
        
    }
    
};


#endif


static SPISettings m_spiSettings = SPISettings(12000000, MSBFIRST, SPI_MODE0);

void CyrWheelLEDPatterns::_spiBegin() {
    // DOUT/ MOSI on pin 7 for this
    // CLK moved to pin 14
    // and then back to the default
    SPI.setMOSI(APA102_LED_DATA_PIN);
    SPI.setSCK(APA102_LED_CLOCK_PIN);
    SPI.begin(); // resets the pins
    SPI.beginTransaction(m_spiSettings);
}

void CyrWheelLEDPatterns::_spiEnd() {
    // back to the original (see teensy 3.1 chart)
    SPI.endTransaction();
    SPI.setMOSI(11);
    SPI.setSCK(13);
    SPI.begin(); // reset
}

void CyrWheelLEDPatterns::internalShow() {
#if USE_FAST_LED
    _spiBegin();
    FastLED.show();
//    Serial.printf("FPS: %d\r\n", FastLED.getFPS());
    _spiEnd();
#elif USE_MANUAL_SPI
    _spiBegin();
    show2();
    _spiEnd();
#endif
}

CyrWheelLEDPatterns::CyrWheelLEDPatterns(uint32_t ledCount) :
    LEDPatterns(ledCount)
{
#if USE_FAST_LED
    // Change the parameters below as needed
    // APA102
    // pin 7 for "data"?
    // datapin 7 data/MOSI
    // clock pin: 14
    // 24mhz would be ideal, but it flickers... in tests
    // 12mhz kills other things. It is really strange..I just can't get fastLED to work right
//  FastLED.addLeds<APA102, APA102_LED_DATA_PIN, APA102_LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(12)>(m_leds, ledCount).setCorrection(TypicalLEDStrip);
    
    static CD_APA102Controller<APA102_LED_DATA_PIN, APA102_LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(24)> c;
    FastLED.addLeds(&c, m_leds, ledCount, 0);
    
#elif USE_MANUAL_SPI


#endif
}

void CyrWheelLEDPatterns::setBrightness(uint8_t brightness) {
    FastLED.setBrightness(brightness);
}

uint8_t CyrWheelLEDPatterns::getBrightness() {
    return FastLED.getBrightness();
};

void CyrWheelLEDPatterns::show2() {
    
    uint8_t *ptr = (uint8_t *)m_leds;            // -> LED data
    uint16_t n   = getLEDCount();
    int i;
    
#if WORKS_BUT_SLOWER
    for(i=0; i<4; i++) SPI.transfer(0);    // Start-frame marker
    do {                               // For each pixel...
        SPI.transfer(0xFF);                //  Pixel start
        // RGB -> BGR
        SPI.transfer(ptr[2]);
        SPI.transfer(ptr[1]);
        SPI.transfer(ptr[0]);
        ptr += 3;
        //        for(i=0; i<3; i++) SPI.transfer(*ptr++); // R,G,B
    } while(--n);
    for(i=0; i<4; i++) SPI.transfer(0xFF); // End-frame marker (see note above)
    
#endif
    
#if 1 // Faster, but has to do the last bit clear before resetting
    int brightness = 0;
    int b16 = 0;
    
    uint8_t next;
    for(i=0; i<3; i++) SPI.transfer(0x00);    // First 3 start-frame bytes
    SPDR = 0x00;                         // 4th is pipelined
    do {                                 // For each pixel...
        while(!(SPSR & _BV(SPIF)));        //  Wait for prior byte out
        SPDR = 0xFF;                       //  Pixel start
        for(i=0; i<3; i++) {               //  For R,G,B...
            uint8_t v = ptr[2-i]; // BGR
            next = brightness ? (v * b16) >> 8 : v; // Read, scale
            while(!(SPSR & _BV(SPIF)));      //   Wait for prior byte out
            SPDR = next;                     //   Write scaled color
        }
        ptr += 3;
    } while(--n);
    while(!(SPSR & _BV(SPIF)));          // Wait for last byte out
    
    //corbin!!!! this is needed to discard any data that was sent/left on the pipe.. Otherwise it kills the Bluetooth. I need to find out why this really is... as I suspect the bluetooth
    SPI0_MCR |= SPI_MCR_CLR_RXF; // discard any received data
    
    //ending marker..
    for(i=0; i<4; i++) SPI.transfer(0xFF);
    
#endif
    
}
void CyrWheelLEDPatterns::begin() {
#if DEBUG
    Serial.print("CyrWheelLEDPatterns::begin\r\n");
#endif
}
