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


/* Teensy 3.1 wiring diagram 

 Teensy 3.1 only:
 ----------------
 +5v -> Teensy Vin
 Gnd -> Teensy Gnd
 Teensy 3.3v -> Teensy pin 21 / A7 (voltage reference read)
 

 
 LED Strip:
 ----------------
 Teensy STRIP_PIN -> resistor (R?) -> Data in of LED strip
 +5V -> LED strip +5v
 Gnd -> LED strip Gnd
 
 Button
 ----------------
 Teensy BUTTON_PIN -> Button
 Other side of button -> Gnd
 
 
 BRIGHTNESS_PIN --- not wired up anymore..
 ----------------
 
 
 SD Card - http://www.pjrc.com/teensy/sd_adaptor.html and http://www.pjrc.com/teensy/td_libs_SPI.html and http://forum.pjrc.com/threads/16758-Teensy-3-MicroSD-guide
 ----------------
 +5v -> +5v on SD Card
 Gnd -> Gnd on SD Card
 Teensy SD_CARD_CS_PIN -> SD SS
 Teensy MOSI (pin 11) -> SD MOSI
 Teensy MISO (pin 12) -> SD MISO
 Teensy SCLK (pin 13) -> SD SCLK
 
 
 
*/






#endif
