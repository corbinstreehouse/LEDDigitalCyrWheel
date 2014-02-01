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

#include "Button.h"
//#include <Wire.h>
//#include <HardwareSerial.h>
//#include <EEPROM.h>
//#include <avr/eeprom.h>

#include <stdint.h> // We can compile without this, but it kills xcode completion without it! it took me a while to discover that..
#include "CDExportedImage.h"

#define USE_ADAFRUIT 1
#if USE_ADAFRUIT
#include <Adafruit_NeoPixel.h>
#else
#include <OctoWS2811.h>
#endif 


#define DEBUG 1

#define STRIP_PIN 14
#define BRIGHTNESS_PIN 17
#define BUTTON_PIN 18

#define STRIP_LENGTH 60 // (67*2)

typedef enum {
    CDDisplayModeMin = 0,
    CDDisplayModeShowOn = 0,
    CDDisplayModeRainbow,
    CDDisplayColorWipe,
    CDDisplayModeImagePlayback,
    
    CDDisplayModeMax,
} CDDisplayMode;

#if USE_ADAFRUIT
Adafruit_NeoPixel g_strip = Adafruit_NeoPixel(STRIP_LENGTH, STRIP_PIN, NEO_GRB + NEO_KHZ800);
#else

class CDOctoWS2811 : public OctoWS2811 {
private:
    uint8_t brightness;
public:
    inline void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
        if(brightness) { // See notes in setBrightness()
            r = (r * brightness) >> 8;
            g = (g * brightness) >> 8;
            b = (b * brightness) >> 8;
        }
        this->setPixel(STRIP_LENGTH + n, r, g, b); // hack...not using first input length LED.
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
};


int framebuff[STRIP_LENGTH*6];
int drawingbuff[STRIP_LENGTH*6];
CDOctoWS2811 g_strip = CDOctoWS2811(STRIP_LENGTH, framebuff, drawingbuff, WS2811_GRB + WS2811_800kHz);


#endif
const int g_LED = LED_BUILTIN;

static char g_displayMode = CDDisplayModeShowOn; // CDDisplayModeImagePlayback; // Not CDDisplayMode so I can easily do math with it..
static Button g_button = Button(BUTTON_PIN);

// for my own delay to make button responsive...


static void process();

static inline bool shouldExitEarly() {
    process();
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
    
    pinMode(BRIGHTNESS_PIN, INPUT);
    
    g_strip.begin();
    g_strip.setBrightness(128); // default: half bright..TODO: store/restore the value? or just read it constantly?
    
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
    Serial.println("rainbow");
    int count = 0; // hack, corbin
    for(j=0; j<256; j++) {
        for(i=0; i<g_strip.numPixels(); i++) {
            uint32_t color = Wheel((i+j) & 255);
            g_strip.setPixelColor(i, color);
            uint8_t r,g,b;
            r = (color >> 16) & 255;
            b = (color >> 8) & 255;
            g = color & 255;
            Serial.print(r);
            Serial.print(",");
            Serial.print(g);
            Serial.print(",");
            Serial.print(b);
            Serial.print(", ");
            count++;
        }
        g_strip.show();
        if (busyDelay(wait)) {
     //       return;
        }
    }
    Serial.println("\r\nend rainbow");
    Serial.println(count);
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
//	Serial.printf("onPress, mode %d\r\n", g_displayMode);
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
    colorWipe(g_strip.Color(255, 0, 0), 5);
}

typedef struct {
    uint8_t *dataOffset;
} CDPlaybackImageState;


static CDPlaybackImageState g_imageState = { 0 };

void resetState() {
    g_imageState = { 0 };
}

static inline uint8_t *dataEnd(const CDPatternDataHeader *imageHeader) {
    // It ends at the start of this header. The actual image data preceeds it
    return (uint8_t *)imageHeader;
}

static inline uint8_t *dataStart(const CDPatternDataHeader *imageHeader) {
    return dataEnd(imageHeader) - (sizeof(uint8_t)*imageHeader->dataLength);
}

//static void initImageState(CDPatternDataHeader *imageHeader, CDPlaybackImageState *imageState) {
//    imageState->dataOffset = dataStart(imageHeader);
//#if 0 // DEBUG
//    Serial.println("------------------------------");
//    Serial.printf("playing image h: %d, w: %d, len: %d\r\n", imageHeader->height, imageHeader->width, imageHeader->dataLength);
//    Serial.printf("init image state; header: %x, data:%x, datavalue:%x\r\n", imageHeader, imageState->dataOffset, *imageState->dataOffset);
//    Serial.println("");
//#endif
//}


void playbackColorFadeWithHeader(const CDPatternDataHeader *imageHeader, CDPlaybackImageState *imageState, bool sideScrolling = false) {
    static bool done = false;
    if (done) return;
    
    if (g_button.isPressed()) return;
    
    // Fill up each pixel, wrapping as we need, and then sleep the duration specified (if any)
    uint8_t *currentOffset = imageState->dataOffset;
    // Check to make sure it is still good!
    if (currentOffset < dataStart(imageHeader) || currentOffset >= dataEnd(imageHeader)) {
        currentOffset = dataStart(imageHeader);
        imageState->dataOffset = currentOffset; // Save it for the next loop
#if DEBUG
        Serial.println("+++++++++++++++++ reset ------------------------------");
        Serial.print("start:");
        Serial.println((int)dataStart(imageHeader));
        Serial.print("end:");
        Serial.println((int)dataEnd(imageHeader));
        
#endif
    }
    
    Serial.println(" start ------------------------ ");
    Serial.println((int)currentOffset);
    Serial.println("------------------------ ");
    for (int x = 0; x < g_strip.numPixels(); x++) {
        // First, bounds check each time
        if (currentOffset < dataStart(imageHeader) || currentOffset >= dataEnd(imageHeader)) {
            currentOffset = dataStart(imageHeader);
#if DEBUG
            Serial.println("----------------- overflow reset in loop ------------------------------");
#endif
            done = true;
            break; ///////////////// TEMP CORBIN

        
        }
        // TODO: decoding switch here...
        Serial.println((int)currentOffset);
        uint8_t r = pgm_read_byte(currentOffset++);
        Serial.println((int)currentOffset);
        uint8_t g = pgm_read_byte(currentOffset++);
        Serial.println((int)currentOffset);
        uint8_t b = pgm_read_byte(currentOffset++);
#if DEBUG
//        Serial.printf("x:%d, r:%d, g:%d, b:%d\r\n", x, r, g, b);
        Serial.printf("                 %d,%d,%d\r\n", r, g, b);

#endif
        g_strip.setPixelColor(x, r, g, b);
        //if (shouldExitEarly()) return; // corbin
        
    }
    
    if (shouldExitEarly()) return;
    
    g_strip.show();
    
    Serial.println(" - done");
    Serial.println((int)currentOffset);
    Serial.println(" ---------------- ");
    
    if (imageHeader->duration > 0) {
        busyDelay(imageHeader->duration);
    } else {
        //temp, corbin..hardcoded to match
        busyDelay(10);
//        delay(10);
    }
    Serial.println("delay --------- done");
    
    if (sideScrolling) {
        imageState->dataOffset = imageState->dataOffset + 3; // Go one pixel further.. // TODO: abstract this...based on encoding
    } else {
        // full page of pixels at a time...
        imageState->dataOffset = currentOffset;
    }
}

void playbackImageWithHeader(const CDPatternDataHeader *imageHeader, CDPlaybackImageState *imageState) {
    switch (imageHeader->patternType) {
        case CDPatternTypeFade: {
            playbackColorFadeWithHeader(imageHeader, imageState);
            break;
        }
//        default: {
//// TODO??
//            break;
//        }
    }
    /*
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
#if DEBUG
//        Serial.printf("x:%d y:%d, r:%d, g:%d, b:%d, yMax:%d, numPix:%d\r\n", imageState->xOffset, y, r, g, b, yMax, numPix);
#endif
        g_strip.setPixelColor(y, r, g, b);
        if (shouldExitEarly()) return;
    }
    for (int y = yMax; y < numPix; y++) {
        g_strip.setPixelColor(y, 0, 0, 0); // off for any extra pixels we have
    }
    if (shouldExitEarly()) return;

    g_strip.show();
    
    busyDelay(200);
    
    imageState->xOffset++;
     */
}

static void playbackImage() {
    playbackImageWithHeader(&g_imageData.header, &g_imageState);
}

static void testGreen() {
    static bool set = false;
    if (set) return;
    set = true;
    
    
    for (int i = 0; i < g_strip.numPixels(); i++) {
        uint8_t g = 255 * ((float)i / (float)g_strip.numPixels());
        Serial.println(g);
        g_strip.setPixelColor(i, 0, g, 0);
    }
    g_strip.show();
}
static void updateBrightness() {
    int val = analogRead(BRIGHTNESS_PIN);
    // Map 0 - 1024 to 0-255 brightness
    float b = (float)val * 255.0 / 1024.0;
    int v = b;
    static int priorV  = 0;
    if (priorV != v) {
        g_strip.setBrightness(v);
    }
}

static void process() {
    g_button.process();
    updateBrightness();
}

void loop() {
    process();
    
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
            static bool shown = false;
            if (!shown) { shown = true;
//                showRainbow();
            }
            
            break;
        }
        case CDDisplayColorWipe: {
    //        showColorWipe();
            break;
        }
        case CDDisplayModeImagePlayback: {
//            testGreen();
            playbackImage();
            break;
        }
            
    }
}

