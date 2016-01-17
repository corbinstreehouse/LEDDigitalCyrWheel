//
//  CyrWheelLEDPatterns.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 1/17/16 .
//
//

#include "CyrWheelLEDPatterns.h"

#define USE_FAST_LED 0
#define USE_MANUAL_SPI 1


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
    FastLED.show();
//    Serial.printf("FPS: %d\r\n", FastLED.getFPS());
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
    FastLED.addLeds<APA102, APA102_LED_DATA_PIN, APA102_LED_CLOCK_PIN, BGR, DATA_RATE_MHZ(24)>(m_leds, ledCount).setCorrection(TypicalLEDStrip);
    
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
