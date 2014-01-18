// LED Digital Cyr Wheel
//  Created by corbin dunn, january 14, 2014

/* NOTES: For code completion to work:
    1. Select the Project
    2. Select the target named "Index"
    3. Select Build Phase
    4. Expand it; hit plus at the bottom 
    5. Add all *.c, *.cpp files to it
*/

#include  "Arduino.h"
#include <Adafruit_NeoPixel.h>

#include "Button.h"
//#include <Wire.h>
//#include <HardwareSerial.h>
//#include <EEPROM.h>
//#include <avr/eeprom.h>

#include <stdint.h> // We can compile without this, but it kills xcode completion without it! it took me a while to discover that..
#include "CDExportedImage.h"


#define DEBUG 1

#define STRIP_PIN 14
#define BUTTON_PIN 18
#define STRIP_LENGTH 67

typedef enum {
    CDDisplayModeMin = 0,
    CDDisplayModeShowOn = 0,
    CDDisplayModeRainbow,
    CDDisplayColorWipe,
    CDDisplayModeImagePlayback,
    
    CDDisplayModeMax,
} CDDisplayMode;


// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel g_strip = Adafruit_NeoPixel(STRIP_LENGTH, STRIP_PIN, NEO_GRB + NEO_KHZ800);
const int g_LED = LED_BUILTIN;

static char g_displayMode = CDDisplayModeImagePlayback; // Not CDDisplayMode so I can easily do math with it..
static Button g_button = Button(BUTTON_PIN);

// for my own delay to make button responsive...

static inline bool shouldExitEarly() {
    g_button.process();
    return g_button.isPressed();
}

bool busyDelay(uint32_t ms)
{
	uint32_t start = micros();
    
	if (ms > 0) {
		while (1) {
            if (shouldExitEarly()) return true;
			if ((micros() - start) >= 1000) {
				ms--;
				if (ms == 0) return false;
				start += 1000;
			}
			yield();
		}
	}
    return false;
}


void buttonClicked(Button &b);

void setup() {
    pinMode(g_LED, OUTPUT);
    digitalWrite(g_LED, HIGH);
#if DEBUG
    Serial.begin(9600);
    delay(1000);
    Serial.println("--- begin serial --- ");
#endif
    
    g_strip.begin();
    g_strip.show(); // all off.
    
    g_button.pressHandler(buttonClicked);
    
    digitalWrite(g_LED, LOW);
}

#if DEBUG
void dowhite() {
    for (int i = 0; i < g_strip.numPixels(); i++) {
        g_strip.setPixelColor(i, g_strip.Color(255, 255, 255));
    }
    g_strip.show();
    busyDelay(10);
}
#endif

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
    if (WheelPos < 85) {
        return g_strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if(WheelPos < 170) {
        WheelPos -= 85;
        return g_strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
        WheelPos -= 170;
        return g_strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}

void rainbow(uint8_t wait) {
    uint16_t i, j;
    
    for(j=0; j<256; j++) {
        for(i=0; i<g_strip.numPixels(); i++) {
            g_strip.setPixelColor(i, Wheel((i+j) & 255));
        }
        g_strip.show();
        if (busyDelay(wait)) {
            return;
        }
    }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
    for (int j=0; j<10; j++) {  //do 10 cycles of chasing
        for (int q=0; q < 3; q++) {
            for (int i=0; i < g_strip.numPixels(); i=i+3) {
                g_strip.setPixelColor(i+q, c);    //turn every third pixel on
            }
            g_strip.show();
            if (shouldExitEarly()) return;
            
            if (busyDelay(wait)) {
                return;
            }
            
            for (int i=0; i < g_strip.numPixels(); i=i+3) {
                g_strip.setPixelColor(i+q, 0);        //turn every third pixel off
            }
            if (shouldExitEarly()) return;
        }
    }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
    
    for (uint16_t i=0; i<g_strip.numPixels(); i++) {
        g_strip.setPixelColor(i, 0);
    }
    if (shouldExitEarly()) return;
    g_strip.show();
    if (shouldExitEarly()) return;
    
    for(uint16_t i=0; i<g_strip.numPixels(); i++) {
        g_strip.setPixelColor(i, c);
        g_strip.show();
        if (wait > 0) {
            if (busyDelay(wait)) {
                return;
            }
        } else if (shouldExitEarly()) {
            return ;
        }
    }
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
    uint16_t i, j;
    
    for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
        for(i=0; i< g_strip.numPixels(); i++) {
            g_strip.setPixelColor(i, Wheel(((i * 256 / g_strip.numPixels()) + j) & 255));
        }
        if (shouldExitEarly()) return;
        g_strip.show();
        if (busyDelay(wait)) return;
    }
}

void resetState();

void buttonClicked(Button &b){
#if DEBUG
	Serial.printf("onPress, mode %d\r\n", g_displayMode);
#endif
    g_displayMode = g_displayMode++;
    if (g_displayMode >= CDDisplayModeMax) {
        g_displayMode = CDDisplayModeMin;
    }
    resetState();
}

void showOn() {
    for (int i = 0; i < g_strip.numPixels(); i++) {
        g_strip.setPixelColor(i, 0);
    }
    g_strip.setPixelColor(0, 0, 20, 0);
    g_strip.show();
}

void showRainbow() {
    rainbow(10);
}

void showColorWipe() {
    colorWipe(g_strip.Color(255, 0, 0), 10);
}

typedef struct {
    int xOffset;
    uint8_t *dataOffset;
} CDPlaybackImageState;


static CDPlaybackImageState g_imageState = { 0, 0 };

void resetState() {
    g_imageState = { 0, 0 };
}

static inline uint8_t *dataEnd(CDPatternDataHeader *imageHeader) {
    // It ends at the start of this header. The actual image data preceeds it
    return (uint8_t *)imageHeader;
}


static void initImageState(CDPatternDataHeader *imageHeader, CDPlaybackImageState *imageState) {
    imageState->xOffset = 0;
    imageState->dataOffset = dataEnd(imageHeader) - (sizeof(uint8_t)*imageHeader->dataLength);
#if 0 // DEBUG
    Serial.println("------------------------------");
    Serial.printf("playing image h: %d, w: %d, len: %d\r\n", imageHeader->height, imageHeader->width, imageHeader->dataLength);
    Serial.printf("init image state; header: %x, data:%x, datavalue:%x\r\n", imageHeader, imageState->dataOffset, *imageState->dataOffset);
    Serial.println("");
#endif
}

void playbackImageWithHeader(CDPatternDataHeader *imageHeader, CDPlaybackImageState *imageState) {
    // Validate our offsets; assumes the last column is correct...and data isn't truncated
    if (imageState->xOffset >= imageHeader->width || imageState->dataOffset == 0 || imageState->dataOffset >= dataEnd(imageHeader)) {
        initImageState(imageHeader, imageState);
    }

    // Use whatever is smaller...the pixels we got, or the image
    int numPix = g_strip.numPixels();
    int yMax = numPix > imageHeader->height ? imageHeader->height : numPix;
    
    for (int y = 0; y < yMax; y++) {
        // switch on encoding here..
        // RGB..
        uint8_t r = *imageState->dataOffset++;
        uint8_t g = *imageState->dataOffset++;
        uint8_t b = *imageState->dataOffset++;
#if 0 // DEBUG
        Serial.printf("x:%d y:%d, r:%d, g:%d, b:%d, yMax:%d, numPix:%d\r\n", imageState->xOffset, y, r, g, b, yMax, numPix);
#endif
        g_strip.setPixelColor(y, r, g, b);
        if (shouldExitEarly()) return;
    }
    for (int y = yMax; y < numPix; y++) {
        g_strip.setPixelColor(y, 0, 0, 0); // off for any extra pixels we have
    }
    if (shouldExitEarly()) return;

    g_strip.show();
    imageState->xOffset++;
}

static void playbackImage() {
    playbackImageWithHeader(&g_imageData.header, &g_imageState);
}


void loop() {
    g_button.process();

#if DEBUG
    if (g_button.isPressed()){
        digitalWrite(g_LED, HIGH);
    } else {
        digitalWrite(g_LED, LOW);
    }
#endif
    switch (g_displayMode) {
        case CDDisplayModeShowOn: {
            showOn();
            break;
        }
        case CDDisplayModeRainbow: {
            showRainbow();
            break;
        }
        case CDDisplayColorWipe: {
            showColorWipe();
            break;
        }
        case CDDisplayModeImagePlayback: {
            playbackImage();
            break;
        }
            
    }
}

