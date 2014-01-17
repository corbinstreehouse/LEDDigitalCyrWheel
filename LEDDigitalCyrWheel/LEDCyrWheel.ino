// LED Cyr Wheel
//  Created by corbin dunn on 5/18/12

#include  "Arduino.h" // — for Arduino 1.0

//#include <SD.h>        // Library from Adafruit.com 
//#include <SdFatUtil.h>
//#include <SoftwareSerial.h>
//#include <Canbus.h>

#include <Wire.h>
#include <HardwareSerial.h>

//#include <EEPROM.h>
//#include <avr/eeprom.h>

#include <stdint.h> // We can compile without this, but it kills xcode completion without it! it took me a while to discover that..

// Configure your screen sizes. Only tested with this size, and the Adafruit 2.2" TFT display
// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!


#define REDPIN 9
#define GREENPIN 10
#define BLUEPIN 11

#define BUTTON_PIN 2

int led = 13;

#define FADESPEED 5     // make this higher to slow down

void setup() {
    pinMode(REDPIN, OUTPUT);
    pinMode(GREENPIN, OUTPUT);
    pinMode(BLUEPIN, OUTPUT);
    
    pinMode(BUTTON_PIN, INPUT);
    Serial.begin(9600);
    Serial.println("stetup");
    pinMode(led, OUTPUT);
    
    
    digitalWrite(REDPIN, 0);
    digitalWrite(GREENPIN, 0);
    digitalWrite(BLUEPIN, 0);
    
}

long debounce = 200;   // the debounce time, increase if the output flickers
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

long time = 0;         // the last time the output pin was toggled
int state = LOW;      // the current state of the output pin

int mode = 0;

int checkButton() {
    int result = 0;
    int buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == HIGH && previous == LOW && millis() - time > debounce) {
        mode++;
        if (mode >= 4) {
            mode = 0;
        }
        
        time = millis();
        result = 1;
    }
    if (result) {
        analogWrite(BLUEPIN, 255);
        analogWrite(REDPIN, 255);
        analogWrite(GREENPIN, 255);
    }
    return result;
    
}


#define COLOR_FADE_SPEED

#define DEC_CAP_AT_ZERO(a) a--; if (a < 0); a = 0;

void colorFadeToOff(int r, int b, int g) {
    int iterations = MAX(MAX(r, b), g);
    iterations++;
    for (int i = 0; i < iterations; i++) {
        // write once at the start..then iterate
        analogWrite(REDPIN, r);
        analogWrite(BLUEPIN, b);
        analogWrite(GREENPIN, g);
        DEC_CAP_AT_ZERO(r);
        DEC_CAP_AT_ZERO(b);
        DEC_CAP_AT_ZERO(g);
        if (checkButton()) return;
    }
}


void loop() {
    //    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    //  delay(1000);               // wait for a second
    //  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    //  delay(1000);
    //Serial.println("loop");
    
    
    
    int r, g, b;
    //    analogWrite(REDPIN, 255);
    //  return;
    
    // fade from blue to violet
    
    checkButton();
    
start:
    
    if (mode == 0) {
        colorFadeToOff(255, 0, 0);
        colorFadeToOff(0, 255, 0);
        colorFadeToOff(0, 0, 255);
//
//                       
//        // fade from violet to red
//        for (b = 255; b > 0; b--) {
//            analogWrite(BLUEPIN, b);
//            delay(FADESPEED);
//            if (checkButton()) goto start;
//            
//        }
//        // fade from red to yellow
//        for (g = 0; g < 256; g++) {
//            analogWrite(GREENPIN, g);
//            delay(FADESPEED);
//            if (checkButton()) goto start;
//        }
//        // fade from yellow to green
//        analogWrite(REDPIN, 0);
//        analogWrite(BLUEPIN, 0);
//        analogWrite(GREENPIN, 0);
//        
//        for (r = 255; r > 0; r--) {
//            analogWrite(REDPIN, r);
//            delay(FADESPEED);
//            if (checkButton()) goto start;
//        }
//        // fade from green to teal
//        for (b = 0; b < 256; b++) {
//            analogWrite(BLUEPIN, b);
//            delay(FADESPEED);
//            if (checkButton()) goto start;
//        }
//        // fade from teal to blue
//        for (g = 255; g > 0; g--) {
//            analogWrite(GREENPIN, g);
//            delay(FADESPEED);
//            if (checkButton()) goto start;
//        }
    } else if (mode == 1) {
        analogWrite(REDPIN, 255);
        analogWrite(BLUEPIN, 0);
        analogWrite(GREENPIN, 0);
        
    } else if (mode == 2) {
        analogWrite(REDPIN, 0);
        analogWrite(GREENPIN, 255);
        analogWrite(GREENPIN, 0);
        
    } else if (mode == 3) {
        analogWrite(REDPIN, 0);
        analogWrite(BLUEPIN, 0);
        analogWrite(BLUEPIN, 255);
        
    }
    
    
    //  delay(3000);
    
    
}

