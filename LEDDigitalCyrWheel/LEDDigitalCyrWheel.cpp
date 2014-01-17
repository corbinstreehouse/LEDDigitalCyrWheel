// LED Digital Cyr Wheel
//  Created by corbin dunn, january 14, 2014

#include <Adafruit_NeoPixel.h>

#include  "Arduino.h" // — for Arduino 1.0

//#include <Wire.h>
//#include <HardwareSerial.h>

//#include <EEPROM.h>
//#include <avr/eeprom.h>

#include <stdint.h> // We can compile without this, but it kills xcode completion without it! it took me a while to discover that..




#define PIN 14

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(67*2, PIN, NEO_GRB + NEO_KHZ800);
const int led = LED_BUILTIN;
int rainbowColors[180];

int makeColor(unsigned int hue, unsigned int saturation, unsigned int lightness);
uint32_t Wheel(byte WheelPos);

void setup() {
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    Serial.begin(9600);
    delay(1000);
    Serial.println("begin");
    
    strip.begin();
    Serial.println("strip begin done");
    
    strip.show(); // Initialize all pixels to 'off'
    Serial.println("strip show");
    digitalWrite(led, LOW);
    
    
    for (int i=0; i<180; i++) {
        int hue = i * 2;
        int saturation = 100;
        int lightness = 50;
        // pre-compute the 180 rainbow colors
        rainbowColors[i] = makeColor(hue, saturation, lightness);
    }
    
}

unsigned int h2rgb(unsigned int v1, unsigned int v2, unsigned int hue)
{
	if (hue < 60) return v1 * 60 + (v2 - v1) * hue;
	if (hue < 180) return v2 * 60;
	if (hue < 240) return v1 * 60 + (v2 - v1) * (240 - hue);
	return v1 * 60;
}




// Convert HSL (Hue, Saturation, Lightness) to RGB (Red, Green, Blue)
//
//   hue:        0 to 359 - position on the color wheel, 0=red, 60=orange,
//                            120=yellow, 180=green, 240=blue, 300=violet
//
//   saturation: 0 to 100 - how bright or dull the color, 100=full, 0=gray
//
//   lightness:  0 to 100 - how light the color is, 100=white, 50=color, 0=black
//
int makeColor(unsigned int hue, unsigned int saturation, unsigned int lightness)
{
	unsigned int red, green, blue;
	unsigned int var1, var2;
    
	if (hue > 359) hue = hue % 360;
	if (saturation > 100) saturation = 100;
	if (lightness > 100) lightness = 100;
    
	// algorithm from: http://www.easyrgb.com/index.php?X=MATH&H=19#text19
	if (saturation == 0) {
		red = green = blue = lightness * 255 / 100;
	} else {
		if (lightness < 50) {
			var2 = lightness * (100 + saturation);
		} else {
			var2 = ((lightness + saturation) * 100) - (saturation * lightness);
		}
		var1 = lightness * 200 - var2;
		red = h2rgb(var1, var2, (hue < 240) ? hue + 120 : hue - 240) * 255 / 600000;
		green = h2rgb(var1, var2, hue) * 255 / 600000;
		blue = h2rgb(var1, var2, (hue >= 120) ? hue - 120 : hue + 240) * 255 / 600000;
	}
	return (red << 16) | (green << 8) | blue;
}

void dowhite() {
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(127, 127, 127));
    }
    strip.show();
    delay(10);
}

void colorWipe(uint32_t c, uint8_t wait);
void rainbow2(uint8_t wait);

void loop() {
 //     dowhite();
//    return;
    //theaterChaseRainbow(5);

    return;
    rainbow2(10);
    // rainbowCycle(10);
    
    // Some example procedures showing how to display to the pixels:
    colorWipe(strip.Color(255, 0, 0), 1); // Red
    colorWipe(strip.Color(0, 255, 0), 1); // Green
    colorWipe(strip.Color(0, 0, 255), 1); // Blue
    // Send a theater pixel chase in...
    //theaterChase(strip.Color(127, 127, 127), 50); // White
    //heaterChase(strip.Color(127,   0,   0), 50); // Red
    // theaterChase(strip.Color(  0,   0, 127), 50); // Blue
    
    //rainbow(1);
    
    
    // rainbow(10, 1000);
    
    
    
    
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, 0);
    }
    strip.show();
    
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        //  delay(wait);
    }
}

void rainbow(uint8_t wait) {
    uint16_t i, j;
    
    for(j=0; j<256; j++) {
        for(i=0; i<strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel((i+j) & 255));
        }
        strip.show();
        delay(wait);
    }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
    uint16_t i, j;
    
    for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
        for(i=0; i< strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
        }
        strip.show();
        delay(wait + 10);
    }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
    for (int j=0; j<10; j++) {  //do 10 cycles of chasing
        for (int q=0; q < 3; q++) {
            for (int i=0; i < strip.numPixels(); i=i+3) {
                strip.setPixelColor(i+q, c);    //turn every third pixel on
            }
            strip.show();
            
            delay(wait);
            
            for (int i=0; i < strip.numPixels(); i=i+3) {
                strip.setPixelColor(i+q, 0);        //turn every third pixel off
            }
        }
    }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
    for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
        for (int q=0; q < 3; q++) {
            for (int i=0; i < strip.numPixels(); i=i+3) {
                strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
            }
            strip.show();
            
            delay(wait);
            
            for (int i=0; i < strip.numPixels(); i=i+3) {
                strip.setPixelColor(i+q, 0);        //turn every third pixel off
            }
        }
    }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
    if(WheelPos < 85) {
        return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if(WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
        WheelPos -= 170;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}

#define ledsPerStrip 59
// the shift between each row.  phaseShift=0
// causes all rows to show the same colors moving together.
// phaseShift=180 causes each row to be the opposite colors
// as the previous.
//
// cycleTime is the number of milliseconds to shift through
// the entire 360 degrees of the color wheel:
// Red -> Orange -> Yellow -> Green -> Blue -> Violet -> Red
//
void rainbow(int phaseShift, int cycleTime)
{
    int color, x, y, offset, wait;
    
    wait = cycleTime * 1000 / ledsPerStrip;
    for (color=0; color < 180; color++) {
        digitalWrite(1, HIGH);
        for (x=0; x < ledsPerStrip; x++) {
            for (y=0; y < 1; y++) {
                int index = (color + x + y*phaseShift/2) % 180;
                strip.setPixelColor(x + y*ledsPerStrip, rainbowColors[index]);
            }
        }
        strip.show();
        digitalWrite(1, LOW);
        delayMicroseconds(wait);
    }
}

void rainbow2(uint8_t wait) {
    uint16_t i, j;
    
    for(j=0; j<256; j++) {
        for(i=0; i<strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel((i+j) & 255));
        }
        strip.show();
        delay(wait);
    }
}

