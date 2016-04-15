//
//  CyrWheelLEDPatterns.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 1/17/16 .
//
//

#include "CyrWheelLEDPatterns.h"
#include "LEDCommon.h"

#define USE_FAST_LED 1
#define USE_MANUAL_SPI 0

#if USE_FAST_LED

template <uint8_t DATA_PIN, uint8_t CLOCK_PIN, EOrder RGB_ORDER = BGR>
class CD_APA102Controller : public CLEDController {
public:
    CD_APA102Controller() {}
    
    virtual void init() {
    }
    
    virtual void clearLeds(int nLeds) {
        showColor(CRGB(0,0,0), nLeds, CRGB(0,0,0));
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

         
         /*
        for(int i=0; i<4; i++) SPI.transfer(0);    // Start-frame marker
        for(int i = 0; i < nLeds; i++) {
            SPI.transfer(0xFF);                //  Pixel start
            // RGB -> BGR
            uint8_t b = pixels.loadAndScale0();
            SPI.transfer(b);
            b = pixels.loadAndScale1();
            SPI.transfer(b);
            b = pixels.loadAndScale2();
            SPI.transfer(b);
            pixels.stepDithering();
            pixels.advanceData();
            //        for(i=0; i<3; i++) SPI.transfer(*ptr++); // R,G,B
        }
*/
        
        //ending marker; we need to send more clock cycles than what the spec says. This is detailed on: https://cpldcpu.wordpress.com/2014/11/30/understanding-the-apa102-superled/
        
//        for(int i=0; i<4; i++) SPI.transfer(0xFF);
        // End frame: 8+8*(leds >> 4) clock cycles
        for (int i=0; i<nLeds; i+=16) {
            SPI.transfer(0xff); // 8 more clock cycles
        }
        
    }
    
};


#endif

// 12Mhz // 12000000 is clock div 2
// 12Mhz
// Try 24? -- see issues..
// 8 mhz might work - 12mhz is too fast and causes corruption
static SPISettings m_spiSettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);

void CyrWheelLEDPatterns::_spiBegin() {
    // DOUT/ MOSI on pin 7 for this
    // CLK moved to pin 14
    // and then back to the default
    SPI.setMOSI(APA102_LED_DATA_PIN); // 7
    SPI.setSCK(APA102_LED_CLOCK_PIN); // 14
    SPI.begin(); // resets the pins
    
    // corbin?? enable DSE??
    CORE_PIN14_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2); // SCK0 = 13 (PTC5)
    CORE_PIN7_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2); // DOUT/MOSI = 7 (PTD2)

    SPI.beginTransaction(m_spiSettings);
}

void CyrWheelLEDPatterns::_spiEnd() {
    // back to the original (see teensy 3.1 chart)
    SPI.endTransaction();
    // NOTE: see my post about this https://forum.pjrc.com/threads/32834-SPI-disable-not-working-correctly-with-setMOSI-setMISO-setSCK?p=94986#post94986
    // If disable_pins is not fixed then we have to do more work. I'm going to just do what SPCRemulation.disable_pins() in avr_emulation.h *should* do in SPI end so we don't get random data to the LEDs when I use other devices.
//    SPI.end();
    CORE_PIN7_CONFIG = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
    CORE_PIN14_CONFIG = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);

    SPI.setMOSI(11);
    SPI.setSCK(13);
    SPI.begin(); // reset
}

void CyrWheelLEDPatterns::internalShow() {
    _spiBegin();
#if USE_FAST_LED
    FastLED.show();
//    Serial.printf("FPS: %d\r\n", FastLED.getFPS());
#elif USE_MANUAL_SPI
    show2();
#endif
    _spiEnd();
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
    static CD_APA102Controller<APA102_LED_DATA_PIN, APA102_LED_CLOCK_PIN, BGR> c;
    FastLED.addLeds(&c, m_leds, ledCount, 0);
    // 15 amps max? TODO: test this...
    // 18 amps for now...
    FastLED.setMaxPowerInVoltsAndMilliamps(5.0, 18000);
    
#elif USE_MANUAL_SPI


#endif
}

void CyrWheelLEDPatterns::setBrightness(uint8_t brightness) {
    if (FastLED.getBrightness() != brightness) {
        DEBUG_PRINTF("SET BRIGHTNESS: %d\r\n", brightness);
        FastLED.setBrightness(brightness);
    }
}

uint8_t CyrWheelLEDPatterns::getBrightness() {
    return FastLED.getBrightness();
};


// NOTE: not used!!
void CyrWheelLEDPatterns::show2() {
#if USE_MANUAL_SPI
    
    uint8_t *ptr = (uint8_t *)m_leds;            // -> LED data
    uint16_t ledCount = getLEDCount();
    uint16_t n = ledCount;
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
    // NOTE: might need to send more bits according to the other people.
//    for(i=0; i<4; i++) SPI.transfer(0xFF); // see next line
    
    // End frame: 8+8*(leds >> 4) clock cycles
    for (i = 0; i < ledCount; i += 16) {
        SPI.transfer(0xFF);  // 8 more clock cycles
    }
    
#endif
#endif

}


void CyrWheelLEDPatterns::begin() {
    DEBUG_PRINTLN("CyrWheelLEDPatterns::begin\r\n");
    // start all off..
    fill_solid(m_leds, getLEDCount(), CRGB::Black);
    internalShow();
}
