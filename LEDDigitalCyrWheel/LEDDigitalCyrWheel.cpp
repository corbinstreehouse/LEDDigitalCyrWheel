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

#include "CWPatternSequenceManager.h"
#include "CDPatternData.h"

#define USE_ADAFRUIT 1
#if USE_ADAFRUIT
#include <Adafruit_NeoPixel.h>
#else
#include <OctoWS2811.h>
#endif


#define DEBUG 1

#define STRIP_PIN 14
#define BRIGHTNESS_PIN 22
#define BUTTON_PIN 23

#define STRIP_LENGTH (60*4) // (67*2)

// about 27.36us/LED. See https://learn.sparkfun.com/tutorials/ws2812-breakout-hookup-guide/ws2812-overview for datatransmission of 24 bits using the specified timing
#define MICROSECONDS_TO_UPDATE_EACH_LED 27.36
#define MICROSECONDS_TO_UDPATE_STRIP (uint32_t)(MICROSECONDS_TO_UPDATE_EACH_LED * (float)STRIP_LENGTH)

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

typedef struct {
    uint8_t green, red, blue; // grb format
} rgb_color;



//pololu demo stuff
unsigned int maxLoops = 0;  // go to next state when loopCount >= maxLoops
unsigned int loopCount = 0;
unsigned int seed = 0;  // used to initialize random number generator


void warmWhiteShimmer(unsigned char dimOnly);
void randomColorWalk(unsigned char initializeColors, unsigned char dimOnly);
void traditionalColors();
void brightTwinkleColorAdjust(unsigned char *color);
void colorExplosionColorAdjust(unsigned char *color, unsigned char propChance,
                               unsigned char *leftColor, unsigned char *rightColor);
void colorExplosion(unsigned char noNewBursts);
void gradient();
void brightTwinkle(unsigned char minColor, unsigned char numColors, unsigned char noNewBursts);
unsigned char collision();

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

static char g_displayMode = CDDisplayModeImageLEDGradient; // ImagePlayback; // Not CDDisplayMode so I can easily do math with it..
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


void flashColor(uint8_t r, uint8_t g, uint8_t b, uint32_t delay) {
    for(int i = 0; i < g_strip.numPixels(); i++) {
        g_strip.setPixelColor(i, r, g, b);
    }
    g_strip.show();
    busyDelay(delay);
}

void flashThreeTimes(uint8_t r, uint8_t g, uint8_t b, uint32_t delay) {
    busyDelay(1); // Initializes/checks things
    for (int i = 0; i < 3; i++) {
        flashColor(r, g, b, delay);
        flashColor(0, 0, 0, delay);
    }
}

CWPatternSequenceManager g_sequenceManager;

void setup() {
    pinMode(g_LED, OUTPUT);
    digitalWrite(g_LED, HIGH);
#if DEBUG
    Serial.begin(9600);
    delay(1000);
    Serial.println("--- begin serial --- ");
#endif
    
    for (int i = 0; i < 8; i++) {
        seed += analogRead(i);
    }
//    seed += EEPROM.read(0);  // get part of the seed from EEPROM
    
    randomSeed(seed);
    
    
    pinMode(BRIGHTNESS_PIN, INPUT);
    
    g_strip.begin();
    g_strip.setBrightness(128); // default: half bright..TODO: store/restore the value? or just read it constantly?
    g_strip.show(); // all off.
    
    g_button.pressHandler(buttonClicked);

    bool initPassed = g_sequenceManager.init();
    if (initPassed) {
        // See if we could read from the SD card
        if (g_sequenceManager.loadFirstSequence()) {
            flashThreeTimes(0, 255, 0, 150); // flash green
        } else {
            flashThreeTimes(255, 127, 0, 150); // flash orange...couldn't find any data files
        }
    } else {
        // Flash the LEDs all red to indicate no card...
        flashThreeTimes(255, 0, 0, 300);
        g_sequenceManager.loadDefaultSequence();
    }
    
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
        
//        if (wait > 0) {
//            if (busyDelay(wait)) {
//                return;
//            }
//        } else
        if (shouldExitEarly()) {
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
    uint32_t lastTime;
} CDPlaybackImageState;


static CDPlaybackImageState g_imageState = { 0 };

void resetState() {
    g_imageState = { 0 };
}

static inline uint8_t *dataEnd(const CDPatternItemHeader *imageHeader) {
    // It ends at the start of this header. The actual image data preceeds it
    return (uint8_t *)imageHeader;
}

static inline uint8_t *dataStart(const CDPatternItemHeader *imageHeader) {
    return dataEnd(imageHeader) - (sizeof(uint8_t)*imageHeader->dataLength);
}

//static void initImageState(CDPatternItemHeader *imageHeader, CDPlaybackImageState *imageState) {
//    imageState->dataOffset = dataStart(imageHeader);
//#if 0 // DEBUG
//    Serial.println("------------------------------");
//    Serial.printf("playing image h: %d, w: %d, len: %d\r\n", imageHeader->height, imageHeader->width, imageHeader->dataLength);
//    Serial.printf("init image state; header: %x, data:%x, datavalue:%x\r\n", imageHeader, imageState->dataOffset, *imageState->dataOffset);
//    Serial.println("");
//#endif
//}


static inline uint8_t *keepOffsetInBounds(uint8_t *currentOffset, const CDPatternItemHeader *imageHeader) {
    if (currentOffset < dataStart(imageHeader) || currentOffset >= dataEnd(imageHeader)) {
        currentOffset = dataStart(imageHeader);
    }
    return currentOffset;
}

//if (floatValue  > 0.0)
//result = floor(floatValue + 0.5);
//else
//result = ceil(num - 0.5);
//Note

void playbackColorFadeWithHeader(const CDPatternItemHeader *imageHeader, CDPlaybackImageState *imageState) {
    if (g_button.isPressed()) return;
    
    imageState->dataOffset = keepOffsetInBounds(imageState->dataOffset, imageHeader);
    uint8_t *currentOffset = imageState->dataOffset;

    uint32_t duration = imageHeader->duration;
    if (duration == 0) {
        duration = 40;
    }

    float percentageThrough = 0;
    int amountOfPassedTime = millis() - imageState->lastTime;
    if (amountOfPassedTime < duration) {
        percentageThrough = (float)amountOfPassedTime / (float)duration;
    }
//    Serial.print("percentage through");
//    Serial.println(percentageThrough);
    
    for (int x = 0; x < g_strip.numPixels(); x++) {
        // First, bounds check each time
        currentOffset = keepOffsetInBounds(currentOffset, imageHeader);

        // TODO: decoding switch here...??
        uint8_t r = currentOffset[0];
        uint8_t g = currentOffset[1];
        uint8_t b = currentOffset[2];
        
        currentOffset += 3;
        if (percentageThrough > 0) {
            // Grab the next color and mix it...
            uint8_t *nextColorOffset = keepOffsetInBounds(currentOffset, imageHeader);

            uint8_t r2 = nextColorOffset[0];
            uint8_t g2 = nextColorOffset[1];
            uint8_t b2 = nextColorOffset[2];
            
//            Serial.print("percentage through");
//            Serial.println(percentageThrough);
//            Serial.printf("x:%d, r:%d, g:%d, b:%d\r\n", x, r, g, b);
//            Serial.printf("x:%d, r:%d, g:%d, b:%d\r\n", x, r2, g2, b2);
//            
//            Serial.println();
            
            // TODO: non floating point math..
            r = r + round(percentageThrough * (float)(r2 - r));
            g = g + round(percentageThrough * (float)(g2 - g));
            b = b + round(percentageThrough * (float)(b2 - b));
//            Serial.printf("x:%d, r:%d, g:%d, b:%d\r\n", x, r, g, b);
            Serial.println();
            Serial.println();
        }

#if 0 // DEBUG
        Serial.printf("x:%d, r:%d, g:%d, b:%d\r\n", x, r, g, b);
#endif
        g_strip.setPixelColor(x, r, g, b);
        if (shouldExitEarly()) return;
    }
    
    if (shouldExitEarly()) return;
    
    g_strip.show();

    // Increment..update after we did all the above work
    uint32_t now = millis();
    if (now - imageState->lastTime > duration) {
        imageState->lastTime = millis();
        imageState->dataOffset += 3;
    }
}

void playbackImageWithHeader(const CDPatternItemHeader *imageHeader, CDPlaybackImageState *imageState) {
//    switch (imageHeader->patternType) {
//        case CDDisplayModeImagePlayback: {
//            playbackColorFadeWithHeader(imageHeader, imageState);
//            break;
//        }
//        default: {
//// TODO??
//            break;
//        }
//    }
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
//    playbackImageWithHeader(&g_imageData.header, &g_imageState);
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

static int g_brightness = 0;

static void updateBrightness() {
    int val = analogRead(BRIGHTNESS_PIN);
    // Map 0 - 1024 to 0-255 brightness
    float b = (float)val * 255.0 / 1024.0;
    int v = b;
    if (g_brightness != v) {
        g_strip.setBrightness(v);
        g_brightness = v;
    }
}

static void process() {
    g_button.process();
    updateBrightness();
}


void ledGradient2() {
    // from pololu https://github.com/pololu/pololu-led-strip-arduino/blob/master/PololuLedStrip/examples/LedStripGradient/LedStripGradient.ino
    uint32_t m = millis();
    byte time = m >> 2;
    Serial.print("millis:");
    Serial.print(m);
    Serial.print(" time:");
    Serial.print(time);
    byte  bb = time - 8*72;
    Serial.print(" b:");
    Serial.print(bb);
    
    Serial.println();
    Serial.println();
    for(uint16_t i = 0; i < g_strip.numPixels(); i++)
    {
//        byte x = time - 4*i;
        byte x = time - 8*i;
        g_strip.setPixelColor(i, 255 - x, x, x);
        if (shouldExitEarly()) return;
    }
    g_strip.show();
}


byte timeAsByte() {
    // drop the last two bits of precision, giving us number 1-255 incremented each tick of the cycle.
    return millis();
}


byte valueForOffset(int currentOffset) {
    const int gradientUpPixelCount = 8;
    const int totalGradientPixels = gradientUpPixelCount*2;
    const int minV = 0;
    const int maxV = 255;
    
    // TODO: see if math mod is faster
    int v = currentOffset % totalGradientPixels;
    
    int currentR = map(v, 0, totalGradientPixels-1, minV, maxV*2);

    // math mod...
    while (currentR > maxV*2) {
        currentR -= maxV*2;
    }
    
//    Serial.print(" currentR:");
//    Serial.print(currentR);
    if (currentR > maxV) {
        // Ramp down the value
        currentR = currentR - (maxV - minV); // This would give a small value..we want a big vaul going down to the minV
        currentR = maxV - currentR; // This gives the ramp down
        //            Serial.print(" ramped: ");
        //            Serial.print(currentR);
    }
    
    byte finalR = currentR; // time - currentR;  // Vary for time and roll over (byte truncation)
    return finalR;
}

void ledGradientForDuration(uint32_t durationInMilliseconds) {
    static byte tickOffset = -1;
    static uint32_t lastUpdateTimeInMicroSeconds = 0;
    
    float percentageThrough = 0;
    uint32_t now = micros();
    int amountOfPassedTime = now - lastUpdateTimeInMicroSeconds;
    uint32_t durationInMicroseconds = durationInMilliseconds * 1000;
    if (amountOfPassedTime < durationInMicroseconds) {
        percentageThrough = (float)amountOfPassedTime / (float)durationInMicroseconds;
    } else {
        lastUpdateTimeInMicroSeconds = now;
        tickOffset++;
    }
    
    if (percentageThrough == 0) {
        for (int i = 0; i < g_strip.numPixels(); i++) {
            // ramp up, then down, gradientUpPixelCount per half
            int currentOffset = i + tickOffset;
            byte finalR = valueForOffset(currentOffset);
            g_strip.setPixelColor(i, finalR, 0, 0);

        }
    } else {
        // interpolate to the next value to provide smoother animations during slow durations where we have extra cycles. Better than a busy delay
        byte currentR = valueForOffset(tickOffset);
        for (int i = 0; i < g_strip.numPixels(); i++) {
            int currentOffset = i + tickOffset;
            byte nextR = valueForOffset(currentOffset + 1);
            byte finalR = currentR + (round(percentageThrough * (float)(nextR - currentR)));

            g_strip.setPixelColor(i, finalR, 0, 0);
            currentR = nextR; // For the next iteration
        }
    }
    g_strip.show();
    // Show disables interrupts, so millis() isn't incremented for quite a while. We want accurate timing, so we adjust for that.
    lastUpdateTimeInMicroSeconds -= MICROSECONDS_TO_UDPATE_STRIP;
}

void pulseGradientEffect() {
    
    byte time = timeAsByte();
    
    const int gradientUpPixelCount = 16;
    const int minV = 0;
    const int maxV = 255;
    for (int i = 0; i < g_strip.numPixels(); i++) {
        // ramp up, then down, gradientUpPixelCount per half
        int totalGradientPixels = gradientUpPixelCount*2;
        int v = i % totalGradientPixels;
        int currentR = map(v, 0, totalGradientPixels-1, minV, maxV*2);

        // math mod...
        while (currentR > maxV*2) {
            currentR -= maxV*2;
        }
        
        if (currentR > maxV) {
            // Ramp down the value
            currentR = currentR - (maxV - minV); // This would give a small value..we want a big vaul going down to the minV
            currentR = maxV - currentR; // This gives the ramp down
            //            Serial.print(" ramped: ");
            //            Serial.print(currentR);
        }
        
        byte finalR = time - currentR;  // Vary for time and roll over (byte truncation)

        
        byte finalB = 0;
        int wholePasses = i / 16;
        if (wholePasses % 2 == 0) {
            finalB = finalR;
        }
        g_strip.setPixelColor(i, finalR, 0, finalB);
        
    }
    g_strip.show();
}


uint32_t hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = v * (255 - s) / 255;
    uint8_t q = v * (255 - f * s / 255) / 255;
    uint8_t t = v * (255 - (255 - f) * s / 255) / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return g_strip.Color(r, g, b);
}

//https://github.com/pololu/pololu-led-strip-arduino/blob/master/PololuLedStrip/examples/LedStripRainbow/LedStripRainbow.ino
void slowRainbow() {
    // Update the colors.
    uint16_t time = millis() >> 2;
    for(uint16_t i = 0; i < g_strip.numPixels(); i++)
    {
        byte x = (time >> 2) - (i << 3);
        g_strip.setPixelColor(i, hsvToRgb((uint32_t)x * 359 / 256, 255, 255));
    }
    
    // Write the colors to the LED strip.
    g_strip.show();
    
    busyDelay(10);
}

#define LED_COUNT g_strip.numPixels()

void loop() {
    process();
    
#if DEBUG
    if (g_button.isPressed()){
        digitalWrite(g_LED, HIGH);
    } else {
        digitalWrite(g_LED, LOW);
    }
#endif
    
    rgb_color *colors = (rgb_color *)g_strip.getPixels();
    if (loopCount == 0 && g_displayMode >= CDDisplayModeWarmWhiteShimmer) {
//        // whenever timer resets, clear the LED colors array (all off)
//        for (int i = 0; i < LED_COUNT; i++)
//        {
//            colors[i] = (rgb_color){0, 0, 0};
//        }
    }
    
    
    if (g_displayMode == CDDisplayModeWarmWhiteShimmer || g_displayMode == CDDisplayModeRandomColorWalk)
    {
        // for these two patterns, we want to make sure we get the same
        // random sequence six times in a row (this provides smoother
        // random fluctuations in brightness/color)
        if (loopCount % 6 == 0)
        {
            seed = random(30000);
        }
        randomSeed(seed);
    }
    
    switch (g_displayMode) {
        case CDDisplayModeShowOn: {
            showOn();
            break;
        }
        case CDDisplayModeRainbow: {
            showRainbow();
            break;
        }
        case CDDisplayModeColorWipe: {
            showColorWipe();
            break;
        }
        case CDDisplayModeImagePlayback: {
//            testGreen();
            playbackImage();
            break;
        }
        case CDDisplayModeImageLEDGradient: {
            ledGradientForDuration(50);
            break;
        }
        case CDDisplayModeRainbow2: {
            slowRainbow();
            break;
        }
        case CDDisplayPluseGradientEffect: {
            pulseGradientEffect();
            break;
        }
        case CDDisplayModeWarmWhiteShimmer:
            // warm white shimmer for 300 loopCounts, fading over last 70
            maxLoops = 300;
            warmWhiteShimmer(loopCount > maxLoops - 70);
            
            break;
            
        case CDDisplayModeRandomColorWalk:
            // start with alternating red and green colors that randomly walk
            // to other colors for 400 loopCounts, fading over last 80
            maxLoops = 400;
            randomColorWalk(loopCount == 0 ? 1 : 0, loopCount > maxLoops - 80);
            
            break;
            
        case CDDisplayModeTraditionalColors:
            // repeating pattern of red, green, orange, blue, magenta that
            // slowly moves for 400 loopCounts
            maxLoops = 400;
            traditionalColors();
            
            break;
            
        case CDDisplayModeColorExplosion:
            // bursts of random color that radiate outwards from random points
            // for 630 loop counts; no burst generation for the last 70 counts
            // of every 200 count cycle or over the over final 100 counts
            // (this creates a repeating bloom/decay effect)
            maxLoops = 630;
            colorExplosion((loopCount % 200 > 130) || (loopCount > maxLoops - 100));
            
            break;
            
        case CDDisplayModeGradient:
            // red -> white -> green -> white -> red ... gradiant that scrolls
            // across the strips for 250 counts; this pattern is overlaid with
            // waves of dimness that also scroll (at twice the speed)
            maxLoops = 250;
            gradient();
            delay(6);  // add an extra 6ms delay to slow things down
            
            break;
            
        case CDDisplayModeBrightTwinkle:
            // random LEDs light up brightly and fade away; it is a very similar
            // algorithm to colorExplosion (just no radiating outward from the
            // LEDs that light up); as time goes on, allow progressively more
            // colors, halting generation of new twinkles for last 100 counts.
            maxLoops = 1200;
            if (loopCount < 400)
            {
                brightTwinkle(0, 1, 0);  // only white for first 400 loopCounts
            }
            else if (loopCount < 650)
            {
                brightTwinkle(0, 2, 0);  // white and red for next 250 counts
            }
            else if (loopCount < 900)
            {
                brightTwinkle(1, 2, 0);  // red, and green for next 250 counts
            }
            else
            {
                // red, green, blue, cyan, magenta, yellow for the rest of the time
                brightTwinkle(1, 6, loopCount > maxLoops - 100);
            }
            
            break;
            
        case CDDisplayModeCollision:
            // colors grow towards each other from the two ends of the strips,
            // accelerating until they collide and the whole strip flashes
            // white and fades; this repeats until the function indicates it
            // is done by returning 1, at which point we stop keeping maxLoops
            // just ahead of loopCount
            if (!collision())
            {
                maxLoops = loopCount + 2;
            }
            
            break;
    }
    
    if (g_displayMode >= CDDisplayModeWarmWhiteShimmer) {
        // do a bit walk over to fix brightness, then show
        if (g_displayMode != CDDisplayModeWarmWhiteShimmer && g_displayMode != CDDisplayModeRandomColorWalk && g_displayMode != CDDisplayModeColorExplosion && g_displayMode != CDDisplayModeBrightTwinkle)
        for (int i = 0; i < LED_COUNT; i++) {
            colors[i].red =  (colors[i].red * g_brightness) >> 8;
            colors[i].green =  (colors[i].green * g_brightness) >> 8;
            colors[i].blue =  (colors[i].blue * g_brightness) >> 8;
        }
        
        
        g_strip.show();
        loopCount++;  // increment our loop counter/timer.
        if (loopCount >= maxLoops)
        {
            // if the time is up for the current pattern and the optional hold
            // switch is not grounding the AUTOCYCLE_SWITCH_PIN, clear the
            // loop counter and advance to the next pattern in the cycle
            loopCount = 0;  // reset timer
    //        pattern = ((unsigned char)(pattern+1))%NUM_STATES;  // advance to next pattern
        }
    }
}






















// pololu... https://github.com/pololu/pololu-led-strip-arduino/blob/master/PololuLedStrip/examples/LedStripXmas/LedStripXmas.ino


// This function applies a random walk to val by increasing or
// decreasing it by changeAmount or by leaving it unchanged.
// val is a pointer to the byte to be randomly changed.
// The new value of val will always be within [0, maxVal].
// A walk direction of 0 decreases val and a walk direction of 1
// increases val.  The directions argument specifies the number of
// possible walk directions to choose from, so when directions is 1, val
// will always decrease; when directions is 2, val will have a 50% chance
// of increasing and a 50% chance of decreasing; when directions is 3,
// val has an equal chance of increasing, decreasing, or staying the same.
void randomWalk(unsigned char *val, unsigned char maxVal, unsigned char changeAmount, unsigned char directions)
{
    unsigned char walk = random(directions);  // direction of random walk
    if (walk == 0)
    {
        // decrease val by changeAmount down to a min of 0
        if (*val >= changeAmount)
        {
            *val -= changeAmount;
        }
        else
        {
            *val = 0;
        }
    }
    else if (walk == 1)
    {
        // increase val by changeAmount up to a max of maxVal
        if (*val <= maxVal - changeAmount)
        {
            *val += changeAmount;
        }
        else
        {
            *val = maxVal;
        }
    }
}


// This function fades val by decreasing it by an amount proportional
// to its current value.  The fadeTime argument determines the
// how quickly the value fades.  The new value of val will be:
//   val = val - val*2^(-fadeTime)
// So a smaller fadeTime value leads to a quicker fade.
// If val is greater than zero, val will always be decreased by
// at least 1.
// val is a pointer to the byte to be faded.
void fade(unsigned char *val, unsigned char fadeTime)
{
    if (*val != 0)
    {
        unsigned char subAmt = *val >> fadeTime;  // val * 2^-fadeTime
        if (subAmt < 1)
            subAmt = 1;  // make sure we always decrease by at least 1
        *val -= subAmt;  // decrease value of byte pointed to by val
    }
}


// ***** PATTERN WarmWhiteShimmer *****
// This function randomly increases or decreases the brightness of the
// even red LEDs by changeAmount, capped at maxBrightness.  The green
// and blue LED values are set proportional to the red value so that
// the LED color is warm white.  Each odd LED is set to a quarter the
// brightness of the preceding even LEDs.  The dimOnly argument
// disables the random increase option when it is true, causing
// all the LEDs to get dimmer by changeAmount; this can be used for a
// fade-out effect.
void warmWhiteShimmer(unsigned char dimOnly)
{
    const unsigned char maxBrightness = 120;  // cap on LED brighness
    const unsigned char changeAmount = 2;   // size of random walk step
    
    rgb_color *colors = (rgb_color *)g_strip.getPixels();
    for (int i = 0; i < LED_COUNT; i += 2)
    {
        // randomly walk the brightness of every even LED
        // g r b
        randomWalk(&colors[i].red, maxBrightness, changeAmount, dimOnly ? 1 : 2);
        
        
        // warm white: red = x, green = 0.8x, blue = 0.125x
        colors[i].green = colors[i].red*4/5;  // green = 80% of red
        colors[i].blue = colors[i].red >> 3;  // blue = red/8
        
        // every odd LED gets set to a quarter the brighness of the preceding even LED
        if (i + 1 < LED_COUNT)
        {
            colors[i+1] = (rgb_color){colors[i].red >> 2, colors[i].green >> 2, colors[i].blue >> 2};
        }
    }
}


// ***** PATTERN RandomColorWalk *****
// This function randomly changes the color of every seventh LED by
// randomly increasing or decreasing the red, green, and blue components
// by changeAmount (capped at maxBrightness) or leaving them unchanged.
// The two preceding and following LEDs are set to progressively dimmer
// versions of the central color.  The initializeColors argument
// determines how the colors are initialized:
//   0: randomly walk the existing colors
//   1: set the LEDs to alternating red and green segments
//   2: set the LEDs to random colors
// When true, the dimOnly argument changes the random walk into a 100%
// chance of LEDs getting dimmer by changeAmount; this can be used for
// a fade-out effect.
void randomColorWalk(unsigned char initializeColors, unsigned char dimOnly)
{
    rgb_color *colors = (rgb_color *)g_strip.getPixels();
    const unsigned char maxBrightness = 180;  // cap on LED brightness
    const unsigned char changeAmount = 3;  // size of random walk step
    
    // pick a good starting point for our pattern so the entire strip
    // is lit well (if we pick wrong, the last four LEDs could be off)
    unsigned char start;
    switch (LED_COUNT % 7)
    {
        case 0:
            start = 3;
            break;
        case 1:
            start = 0;
            break;
        case 2:
            start = 1;
            break;
        default:
            start = 2;
    }
    
    for (int i = start; i < LED_COUNT; i+=7)
    {
        if (initializeColors == 0)
        {
            // randomly walk existing colors of every seventh LED
            // (neighboring LEDs to these will be dimmer versions of the same color)
            randomWalk(&colors[i].red, maxBrightness, changeAmount, dimOnly ? 1 : 3);
            randomWalk(&colors[i].green, maxBrightness, changeAmount, dimOnly ? 1 : 3);
            randomWalk(&colors[i].blue, maxBrightness, changeAmount, dimOnly ? 1 : 3);
        }
        else if (initializeColors == 1)
        {
            // initialize LEDs to alternating red and green
            if (i % 2)
            {
                colors[i] = (rgb_color){maxBrightness, 0, 0};
            }
            else
            {
                colors[i] = (rgb_color){0, maxBrightness, 0};
            }
        }
        else
        {
            // initialize LEDs to a string of random colors
            colors[i] = (rgb_color){random(maxBrightness), random(maxBrightness), random(maxBrightness)};
        }
        
        // set neighboring LEDs to be progressively dimmer versions of the color we just set
        if (i >= 1)
        {
            colors[i-1] = (rgb_color){colors[i].red >> 2, colors[i].green >> 2, colors[i].blue >> 2};
        }
        if (i >= 2)
        {
            colors[i-2] = (rgb_color){colors[i].red >> 3, colors[i].green >> 3, colors[i].blue >> 3};
        }
        if (i + 1 < LED_COUNT)
        {
            colors[i+1] = colors[i-1];
        }
        if (i + 2 < LED_COUNT)
        {
            colors[i+2] = colors[i-2];
        }
    }
}


// ***** PATTERN TraditionalColors *****
// This function creates a repeating patern of traditional Christmas
// light colors: red, green, orange, blue, magenta.
// Every fourth LED is colored, and the pattern slowly moves by fading
// out the current set of lit LEDs while gradually brightening a new
// set shifted over one LED.
void traditionalColors()
{
    rgb_color *colors = (rgb_color *)g_strip.getPixels();
    // loop counts to leave strip initially dark
    const unsigned char initialDarkCycles = 10;
    // loop counts it takes to go from full off to fully bright
    const unsigned char brighteningCycles = 20;
    
    if (loopCount < initialDarkCycles)  // leave strip fully off for 20 cycles
    {
        return;
    }
    
    // if LED_COUNT is not an exact multiple of our repeating pattern size,
    // it will not wrap around properly, so we pick the closest LED count
    // that is an exact multiple of the pattern period (20) and is not smaller
    // than the actual LED count.
    unsigned int extendedLEDCount = (((LED_COUNT-1)/20)+1)*20;
    
    for (int i = 0; i < extendedLEDCount; i++)
    {
        unsigned char brightness = (loopCount - initialDarkCycles)%brighteningCycles + 1;
        unsigned char cycle = (loopCount - initialDarkCycles)/brighteningCycles;
        
        // transform i into a moving idx space that translates one step per
        // brightening cycle and wraps around
        unsigned int idx = (i + cycle)%extendedLEDCount;
        if (idx < LED_COUNT)  // if our transformed index exists
        {
            if (i % 4 == 0)
            {
                // if this is an LED that we are coloring, set the color based
                // on the LED and the brightness based on where we are in the
                // brightening cycle
                switch ((i/4)%5)
                {
                    case 0:  // red
                        colors[idx].red = 200 * brightness/brighteningCycles;
                        colors[idx].green = 10 * brightness/brighteningCycles;
                        colors[idx].blue = 10 * brightness/brighteningCycles;
                        break;
                    case 1:  // green
                        colors[idx].red = 10 * brightness/brighteningCycles;
                        colors[idx].green = 200 * brightness/brighteningCycles;
                        colors[idx].blue = 10 * brightness/brighteningCycles;
                        break;
                    case 2:  // orange
                        colors[idx].red = 200 * brightness/brighteningCycles;
                        colors[idx].green = 120 * brightness/brighteningCycles;
                        colors[idx].blue = 0 * brightness/brighteningCycles;
                        break;
                    case 3:  // blue
                        colors[idx].red = 10 * brightness/brighteningCycles;
                        colors[idx].green = 10 * brightness/brighteningCycles;
                        colors[idx].blue = 200 * brightness/brighteningCycles;
                        break;
                    case 4:  // magenta
                        colors[idx].red = 200 * brightness/brighteningCycles;
                        colors[idx].green = 64 * brightness/brighteningCycles;
                        colors[idx].blue = 145 * brightness/brighteningCycles;
                        break;
                }
            }
            else
            {
                // fade the 3/4 of LEDs that we are not currently brightening
                fade(&colors[idx].red, 3);
                fade(&colors[idx].green, 3);
                fade(&colors[idx].blue, 3);
            }
        }
    }
}


// Helper function for adjusting the colors for the BrightTwinkle
// and ColorExplosion patterns.  Odd colors get brighter and even
// colors get dimmer.
void brightTwinkleColorAdjust(unsigned char *color)
{
    if (*color == 255)
    {
        // if reached max brightness, set to an even value to start fade
        *color = 254;
    }
    else if (*color % 2)
    {
        // if odd, approximately double the brightness
        // you should only use odd values that are of the form 2^n-1,
        // which then gets a new value of 2^(n+1)-1
        // using other odd values will break things
        *color = *color * 2 + 1;
    }
    else if (*color > 0)
    {
        fade(color, 4);
        if (*color % 2)
        {
            (*color)--;  // if faded color is odd, subtract one to keep it even
        }
    }
}


// Helper function for adjusting the colors for the ColorExplosion
// pattern.  Odd colors get brighter and even colors get dimmer.
// The propChance argument determines the likelihood that neighboring
// LEDs are put into the brightening stage when the central LED color
// is 31 (chance is: 1 - 1/(propChance+1)).  The neighboring LED colors
// are pointed to by leftColor and rightColor (it is not important that
// the leftColor LED actually be on the "left" in your setup).
void colorExplosionColorAdjust(unsigned char *color, unsigned char propChance,
                               unsigned char *leftColor, unsigned char *rightColor)
{
    if (*color == 31 && random(propChance+1) != 0)
    {
        if (leftColor != 0 && *leftColor == 0)
        {
            *leftColor = 1;  // if left LED exists and color is zero, propagate
        }
        if (rightColor != 0 && *rightColor == 0)
        {
            *rightColor = 1;  // if right LED exists and color is zero, propagate
        }
    }
    brightTwinkleColorAdjust(color);
}


// ***** PATTERN ColorExplosion *****
// This function creates bursts of expanding, overlapping colors by
// randomly picking LEDs to brighten and then fade away.  As these LEDs
// brighten, they have a chance to trigger the same process in
// neighboring LEDs.  The color of the burst is randomly chosen from
// among red, green, blue, and white.  If a red burst meets a green
// burst, for example, the overlapping portion will be a shade of yellow
// or orange.
// When true, the noNewBursts argument changes prevents the generation
// of new bursts; this can be used for a fade-out effect.
// This function uses a very similar algorithm to the BrightTwinkle
// pattern.  The main difference is that the random twinkling LEDs of
// the BrightTwinkle pattern do not propagate to neighboring LEDs.
void colorExplosion(unsigned char noNewBursts)
{
    rgb_color *colors = (rgb_color *)g_strip.getPixels();
    // adjust the colors of the first LED
    colorExplosionColorAdjust(&colors[0].red, 9, (unsigned char*)0, &colors[1].red);
    colorExplosionColorAdjust(&colors[0].green, 9, (unsigned char*)0, &colors[1].green);
    colorExplosionColorAdjust(&colors[0].blue, 9, (unsigned char*)0, &colors[1].blue);
    
    for (int i = 1; i < LED_COUNT - 1; i++)
    {
        // adjust the colors of second through second-to-last LEDs
        colorExplosionColorAdjust(&colors[i].red, 9, &colors[i-1].red, &colors[i+1].red);
        colorExplosionColorAdjust(&colors[i].green, 9, &colors[i-1].green, &colors[i+1].green);
        colorExplosionColorAdjust(&colors[i].blue, 9, &colors[i-1].blue, &colors[i+1].blue);
    }
    
    // adjust the colors of the last LED
    colorExplosionColorAdjust(&colors[LED_COUNT-1].red, 9, &colors[LED_COUNT-2].red, (unsigned char*)0);
    colorExplosionColorAdjust(&colors[LED_COUNT-1].green, 9, &colors[LED_COUNT-2].green, (unsigned char*)0);
    colorExplosionColorAdjust(&colors[LED_COUNT-1].blue, 9, &colors[LED_COUNT-2].blue, (unsigned char*)0);
    
    if (!noNewBursts)
    {
        // if we are generating new bursts, randomly pick one new LED
        // to light up
        for (int i = 0; i < 1; i++)
        {
            int j = random(LED_COUNT);  // randomly pick an LED
            
            switch(random(7))  // randomly pick a color
            {
                    // 2/7 chance we will spawn a red burst here (if LED has no red component)
                case 0:
                case 1:
                    if (colors[j].red == 0)
                    {
                        colors[j].red = 1;
                    }
                    break;
                    
                    // 2/7 chance we will spawn a green burst here (if LED has no green component)
                case 2:
                case 3:
                    if (colors[j].green == 0)
                    {
                        colors[j].green = 1;
                    }
                    break;
                    
                    // 2/7 chance we will spawn a white burst here (if LED is all off)
                case 4:
                case 5:
                    if ((colors[j].red == 0) && (colors[j].green == 0) && (colors[j].blue == 0))
                    {
                        colors[j] = (rgb_color){1, 1, 1};
                    }
                    break;
                    
                    // 1/7 chance we will spawn a blue burst here (if LED has no blue component)
                case 6:
                    if (colors[j].blue == 0)
                    {
                        colors[j].blue = 1;
                    }
                    break;
                    
                default:
                    break;
            }
        }
    }
}


// ***** PATTERN Gradient *****
// This function creates a scrolling color gradient that smoothly
// transforms from red to white to green back to white back to red.
// This pattern is overlaid with waves of brightness and dimness that
// scroll at twice the speed of the color gradient.
void gradient()
{
    rgb_color *colors = (rgb_color *)g_strip.getPixels();
    unsigned int j = 0;
    
    // populate colors array with full-brightness gradient colors
    // (since the array indices are a function of loopCount, the gradient
    // colors scroll over time)
    while (j < LED_COUNT)
    {
        // transition from red to green over 8 LEDs
        for (int i = 0; i < 8; i++)
        {
            if (j >= LED_COUNT){ break; }
            colors[(loopCount/2 + j + LED_COUNT)%LED_COUNT] = (rgb_color){160 - 20*i, 20*i, (160 - 20*i)*20*i/160};
            j++;
        }
        // transition from green to red over 8 LEDs
        for (int i = 0; i < 8; i++)
        {
            if (j >= LED_COUNT){ break; }
            colors[(loopCount/2 + j + LED_COUNT)%LED_COUNT] = (rgb_color){20*i, 160 - 20*i, (160 - 20*i)*20*i/160};
            j++;
        }
    }
    
    // modify the colors array to overlay the waves of dimness
    // (since the array indices are a function of loopCount, the waves
    // of dimness scroll over time)
    const unsigned char fullDarkLEDs = 10;  // number of LEDs to leave fully off
    const unsigned char fullBrightLEDs = 5;  // number of LEDs to leave fully bright
    const unsigned char cyclePeriod = 14 + fullDarkLEDs + fullBrightLEDs;
    
    // if LED_COUNT is not an exact multiple of our repeating pattern size,
    // it will not wrap around properly, so we pick the closest LED count
    // that is an exact multiple of the pattern period (cyclePeriod) and is not
    // smaller than the actual LED count.
    unsigned int extendedLEDCount = (((LED_COUNT-1)/cyclePeriod)+1)*cyclePeriod;
    
    j = 0;
    while (j < extendedLEDCount)
    {
        unsigned int idx;
        
        // progressively dim the LEDs
        for (int i = 1; i < 8; i++)
        {
            idx = (j + loopCount) % extendedLEDCount;
            if (j++ >= extendedLEDCount){ return; }
            if (idx >= LED_COUNT){ continue; }
            
            colors[idx].red >>= i;
            colors[idx].green >>= i;
            colors[idx].blue >>= i;
        }
        
        // turn off these LEDs
        for (int i = 0; i < fullDarkLEDs; i++)
        {
            idx = (j + loopCount) % extendedLEDCount;
            if (j++ >= extendedLEDCount){ return; }
            if (idx >= LED_COUNT){ continue; }
            
            colors[idx].red = 0;
            colors[idx].green = 0;
            colors[idx].blue = 0;
        }
        
        // progressively bring these LEDs back
        for (int i = 0; i < 7; i++)
        {
            idx = (j + loopCount) % extendedLEDCount;
            if (j++ >= extendedLEDCount){ return; }
            if (idx >= LED_COUNT){ continue; }
            
            colors[idx].red >>= (7 - i);
            colors[idx].green >>= (7 - i);
            colors[idx].blue >>= (7 - i);
        }
        
        // skip over these LEDs to leave them at full brightness
        j += fullBrightLEDs;
    }
}


// ***** PATTERN BrightTwinkle *****
// This function creates a sparkling/twinkling effect by randomly
// picking LEDs to brighten and then fade away.  Possible colors are:
//   white, red, green, blue, yellow, cyan, and magenta
// numColors is the number of colors to generate, and minColor
// indicates the starting point (white is 0, red is 1, ..., and
// magenta is 6), so colors generated are all of those from minColor
// to minColor+numColors-1.  For example, calling brightTwinkle(2, 2, 0)
// will produce green and blue twinkles only.
// When true, the noNewBursts argument changes prevents the generation
// of new twinkles; this can be used for a fade-out effect.
// This function uses a very similar algorithm to the ColorExplosion
// pattern.  The main difference is that the random twinkling LEDs of
// this BrightTwinkle pattern do not propagate to neighboring LEDs.
void brightTwinkle(unsigned char minColor, unsigned char numColors, unsigned char noNewBursts)
{
    rgb_color *colors = (rgb_color *)g_strip.getPixels();
    // Note: the colors themselves are used to encode additional state
    // information.  If the color is one less than a power of two
    // (but not 255), the color will get approximately twice as bright.
    // If the color is even, it will fade.  The sequence goes as follows:
    // * Randomly pick an LED.
    // * Set the color(s) you want to flash to 1.
    // * It will automatically grow through 3, 7, 15, 31, 63, 127, 255.
    // * When it reaches 255, it gets set to 254, which starts the fade
    //   (the fade process always keeps the color even).
    for (int i = 0; i < LED_COUNT; i++)
    {
        brightTwinkleColorAdjust(&colors[i].red);
        brightTwinkleColorAdjust(&colors[i].green);
        brightTwinkleColorAdjust(&colors[i].blue);
    }
    
    if (!noNewBursts)
    {
        // if we are generating new twinkles, randomly pick four new LEDs
        // to light up
        for (int i = 0; i < 4; i++)
        {
            int j = random(LED_COUNT);
            if (colors[j].red == 0 && colors[j].green == 0 && colors[j].blue == 0)
            {
                // if the LED we picked is not already lit, pick a random
                // color for it and seed it so that it will start getting
                // brighter in that color
                switch (random(numColors) + minColor)
                {
                    case 0:
                        colors[j] = (rgb_color){1, 1, 1};  // white
                        break;
                    case 1:
                        colors[j] = (rgb_color){1, 0, 0};  // red
                        break;
                    case 2:
                        colors[j] = (rgb_color){0, 1, 0};  // green
                        break;
                    case 3:
                        colors[j] = (rgb_color){0, 0, 1};  // blue
                        break;
                    case 4:
                        colors[j] = (rgb_color){1, 1, 0};  // yellow
                        break;
                    case 5:
                        colors[j] = (rgb_color){0, 1, 1};  // cyan
                        break;
                    case 6:
                        colors[j] = (rgb_color){1, 0, 1};  // magenta
                        break;
                    default:
                        colors[j] = (rgb_color){1, 1, 1};  // white
                }
            }
        }
    }
}


// ***** PATTERN Collision *****
// This function spawns streams of color from each end of the strip
// that collide, at which point the entire strip flashes bright white
// briefly and then fades.  Unlike the other patterns, this function
// maintains a lot of complicated state data and tells the main loop
// when it is done by returning 1 (a return value of 0 means it is
// still in progress).
unsigned char collision()
{
    rgb_color *colors = (rgb_color *)g_strip.getPixels();
    const unsigned char maxBrightness = 180;  // max brightness for the colors
    const unsigned char numCollisions = 5;  // # of collisions before pattern ends
    static unsigned char state = 0;  // pattern state
    static unsigned int count = 0;  // counter used by pattern
    
    if (loopCount == 0)
    {
        state = 0;
    }
    
    if (state % 3 == 0)
    {
        // initialization state
        switch (state/3)
        {
            case 0:  // first collision: red streams
                colors[0] = (rgb_color){maxBrightness, 0, 0};
                break;
            case 1:  // second collision: green streams
                colors[0] = (rgb_color){0, maxBrightness, 0};
                break;
            case 2:  // third collision: blue streams
                colors[0] = (rgb_color){0, 0, maxBrightness};
                break;
            case 3:  // fourth collision: warm white streams
                colors[0] = (rgb_color){maxBrightness, maxBrightness*4/5, maxBrightness>>3};
                break;
            default:  // fifth collision and beyond: random-color streams
                colors[0] = (rgb_color){random(maxBrightness), random(maxBrightness), random(maxBrightness)};
        }
        
        // stream is led by two full-white LEDs
        colors[1] = colors[2] = (rgb_color){255, 255, 255};
        // make other side of the strip a mirror image of this side
        colors[LED_COUNT - 1] = colors[0];
        colors[LED_COUNT - 2] = colors[1];
        colors[LED_COUNT - 3] = colors[2];
        
        state++;  // advance to next state
        count = 8;  // pick the first value of count that results in a startIdx of 1 (see below)
        return 0;
    }
    
    if (state % 3 == 1)
    {
        // stream-generation state; streams accelerate towards each other
        unsigned int startIdx = count*(count + 1) >> 6;
        unsigned int stopIdx = startIdx + (count >> 5);
        count++;
        if (startIdx < (LED_COUNT + 1)/2)
        {
            // if streams have not crossed the half-way point, keep them growing
            for (int i = 0; i < startIdx-1; i++)
            {
                // start fading previously generated parts of the stream
                fade(&colors[i].red, 5);
                fade(&colors[i].green, 5);
                fade(&colors[i].blue, 5);
                fade(&colors[LED_COUNT - i - 1].red, 5);
                fade(&colors[LED_COUNT - i - 1].green, 5);
                fade(&colors[LED_COUNT - i - 1].blue, 5);
            }
            for (int i = startIdx; i <= stopIdx; i++)
            {
                // generate new parts of the stream
                if (i >= (LED_COUNT + 1) / 2)
                {
                    // anything past the halfway point is white
                    colors[i] = (rgb_color){255, 255, 255};
                }
                else
                {
                    colors[i] = colors[i-1];
                }
                // make other side of the strip a mirror image of this side
                colors[LED_COUNT - i - 1] = colors[i];
            }
            // stream is led by two full-white LEDs
            colors[stopIdx + 1] = colors[stopIdx + 2] = (rgb_color){255, 255, 255};
            // make other side of the strip a mirror image of this side
            colors[LED_COUNT - stopIdx - 2] = colors[stopIdx + 1];
            colors[LED_COUNT - stopIdx - 3] = colors[stopIdx + 2];
        }
        else
        {
            // streams have crossed the half-way point of the strip;
            // flash the entire strip full-brightness white (ignores maxBrightness limits)
            for (int i = 0; i < LED_COUNT; i++)
            {
                colors[i] = (rgb_color){255, 255, 255};
            }
            state++;  // advance to next state
        }
        return 0;
    }
    
    if (state % 3 == 2)
    {
        // fade state
        if (colors[0].red == 0 && colors[0].green == 0 && colors[0].blue == 0)
        {
            // if first LED is fully off, advance to next state
            state++;
            
            // after numCollisions collisions, this pattern is done
            return state == 3*numCollisions;
        }
        
        // fade the LEDs at different rates based on the state
        for (int i = 0; i < LED_COUNT; i++)
        {
            switch (state/3)
            {
                case 0:  // fade through green
                    fade(&colors[i].red, 3);
                    fade(&colors[i].green, 4);
                    fade(&colors[i].blue, 2);
                    break;
                case 1:  // fade through red
                    fade(&colors[i].red, 4);
                    fade(&colors[i].green, 3);
                    fade(&colors[i].blue, 2);
                    break;
                case 2:  // fade through yellow
                    fade(&colors[i].red, 4);
                    fade(&colors[i].green, 4);
                    fade(&colors[i].blue, 3);
                    break;
                case 3:  // fade through blue
                    fade(&colors[i].red, 3);
                    fade(&colors[i].green, 2);
                    fade(&colors[i].blue, 4);
                    break;
                default:  // stay white through entire fade
                    fade(&colors[i].red, 4);
                    fade(&colors[i].green, 4);
                    fade(&colors[i].blue, 4);
            }
        }
    }
    
    return 0;
}
