//
//  LEDDigitalCyrWheel.h
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/11/14.
//
//

#ifndef LEDDigitalCyrWheel_LEDDigitalCyrWheel_h
#define LEDDigitalCyrWheel_LEDDigitalCyrWheel_h

bool busyDelay(uint32_t ms);

// All my defined pins
#define SD_CARD_CS_PIN 4
#define BUTTON_PIN 23

#define BRIGHTNESS_PIN 22
#define STRIP_PIN 2 // 14 // Use pin 2 so Octo works, and pin 14 for a secondary strip (opposite side) to do patterns

const int g_LED = LED_BUILTIN;
const int g_batteryVoltagePin = A3; // pin 17
const int g_batteryRefPin = A7; // 3.3v ref voltage is connected to pin 21 (I'm not sure i need this)



#endif
