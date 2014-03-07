/* $Id$
 ||
 || @file 		       Button.cpp
 || @author 		     Alexander Brevig              <alexanderbrevig@gmail.com>
 || @url            http://alexanderbrevig.com
 ||
 || @description
 || | This is a Hardware Abstraction Library for Buttons
 || | It provides an easy way of handling buttons
 || #
 ||
 || @license LICENSE_REPLACE
 ||
 */

#include <Arduino.h>
#include "Button.h"

#define debounceDuration 20

/*
 || @constructor
 || | Set the initial state of this button
 || #
 ||
 || @parameter buttonPin  sets the pin that this switch is connected to
 || @parameter buttonMode indicates BUTTON_PULLUP or BUTTON_PULLDOWN resistor
 */
Button::Button(uint8_t buttonPin){
	pin=buttonPin;
    pinMode(pin,INPUT);
    
    digitalWrite(pin,HIGH);
    
    _previousState = HIGH;
    cb_onPress = 0;
    cb_onRelease = 0;
    cb_onClick = 0;
    cb_onHold = 0;
    triggeredHoldEvent = false;
    holdEventThreshold = 500; // half a second
    pressedStartTime = -1;
}

void Button::process(void)
{
    //Serial.println("process");
    //save the previous value
    
    uint8_t currentState = digitalRead(pin);
    if (currentState != _previousState && (millis() - _debounceStartTime) > debounceDuration) {
//        Serial.println("button - start");
        
        _previousState = currentState;
        _debounceStartTime = millis();
        if (currentState == LOW) {
//            Serial.println("button - on press");
            if (cb_onPress) { cb_onPress(*this); }   //fire the onPress event
            pressedStartTime = millis();             //start timing
            triggeredHoldEvent = false;
        } else  {
//            Serial.println("button - release");
            if (cb_onRelease) { cb_onRelease(*this); } //fire the onRelease event
            if (!triggeredHoldEvent) {
                // only send this if we didn't send a hold (mutually exlusive)
                if (cb_onClick) { cb_onClick(*this); }   //fire the onClick event AFTER the onRelease
            }
            //reset states (for timing and for event triggering)
            pressedStartTime = -1;
        }
    } else {
        if (cb_onHold && pressedStartTime != -1 && !triggeredHoldEvent) {
            if (millis() - pressedStartTime > holdEventThreshold) {
//                Serial.println("button - HOLD");
                triggeredHoldEvent = true; // set before calling the method in case it re-enters
                cb_onHold(*this);
            }
        }
    }
}
bool Button::isPressed()
{
    return _previousState == LOW;
}

void Button::setHoldThreshold(unsigned int holdTime)
{
    holdEventThreshold = holdTime;
}

void Button::pressHandler(buttonEventHandler handler)
{
    cb_onPress = handler;
}

void Button::releaseHandler(buttonEventHandler handler)
{
    cb_onRelease = handler;
}

void Button::clickHandler(buttonEventHandler handler)
{
    cb_onClick = handler;
}

void Button::holdHandler(buttonEventHandler handler, unsigned int holdTime /*=0*/)
{
    if (holdTime>0) { setHoldThreshold(holdTime); }
    cb_onHold = handler;
}

unsigned int Button::holdTime() const { if (pressedStartTime!=-1) { return millis()-pressedStartTime; } else return 0; }
