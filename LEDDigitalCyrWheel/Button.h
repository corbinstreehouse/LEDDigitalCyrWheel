/* $Id$
||
|| @file 		       Button.cpp
|| @author 		     Alexander Brevig              <alexanderbrevig@gmail.com>        
|| @url            http://alexanderbrevig.com
||
|| @description
|| | This is a Hardware Abstraction Library for Buttons
|| | It providea an easy way of handling buttons
|| #
||
|| @license LICENSE_REPLACE
||
*/

#ifndef Button_h
#define Button_h

#include <inttypes.h>

class Button;
typedef void (*buttonEventHandler)(Button&);

class Button {
  public:
  
    Button(uint8_t buttonPin);
    
    uint8_t             pin;
    void pullup(uint8_t buttonMode);
    void pulldown();
    void process();

    bool isPressed();
    
    void setHoldThreshold(unsigned int holdTime);
    
    void pressHandler(buttonEventHandler handler);
    void releaseHandler(buttonEventHandler handler);
    void clickHandler(buttonEventHandler handler);
    void holdHandler(buttonEventHandler handler, unsigned int holdTime=0);
  
    unsigned int holdTime() const;
    
    bool operator==(Button &rhs);
    
  private: 
    uint8_t _previousState;
    
    unsigned long       pressedStartTime;
    unsigned int        holdEventThreshold;
    unsigned long       _debounceStartTime;
    buttonEventHandler  cb_onPress;
    buttonEventHandler  cb_onRelease;
    buttonEventHandler  cb_onClick;
    buttonEventHandler  cb_onHold;
    unsigned int        numberOfPresses;
    bool                triggeredHoldEvent;
};

#endif