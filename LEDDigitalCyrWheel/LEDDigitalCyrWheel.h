//
//  LEDDigitalCyrWheel.h
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/11/14.
//
//

#ifndef LEDDigitalCyrWheel_LEDDigitalCyrWheel_h
#define LEDDigitalCyrWheel_LEDDigitalCyrWheel_h

#include "LEDCommon.h" // Defines STRIP_PIN

// All my defined pins
#define SD_CARD_CS_PIN 10 //10 in latest update... /* SS is pin 10 by default, but I have it wird in my wheel to pin 4! */ // I think my wheel is wired to pin 4, which is wrong!!! I fixed my breadboard..
// See: https://www.pjrc.com/teensy/td_libs_SPI.html#ss for more info

#define BUTTON_PIN 23

#define USE_NEW_V_REFERENCE 1 // new resistor values (not in v2 wheel)

//#define BRIGHTNESS_PIN 22

// TOOD: actually make the strip length dynamic...
#define STRIP_LENGTH 331 // my actual count // (14+60*2)// (60*4) // (67*2)

//const int g_LED = LED_BUILTIN; // Using the built-in LED will conflict with the SD Card, as it is on SCLK pin 13. So, don't use it!
const int g_batteryVoltagePin = A3; // pin 17

// TODO: I need to rewrire my battery voltage test; I need to base it off 5v ref, and not 3.3v

//const int g_batteryRefPin = A7; // 3.3v ref voltage is connected to pin 21 (I'm not sure i need this) // NOTE: I need the 3.3v for the SD card! So, I'm no longer using this. My measured value is around 3.29v last I checked, but now my algorithm to check for low voltage is no longer working. However...the wheel just "shuts off", so it isn't too bad.


// Locations in EEPROM
#define EEPROM_BRIGHTNESS_ADDRESS 10
#define EEPROM_START_WIFI_AUTOMATICALLY_ADDRESS 12
#define EEPROM_MIN_MAX_IS_SAVED_ADDRESS 22 // rather abitrary
#define MIN_EEPROM_ADDRESS 24 // rather abitrary
#define EEPROM_ACCELEROMETER_MAX_ADDRESS (24+2*3)    // min value has a vector of 3 16-bit values; 16-bits is 2 bytes. 3*2 = 6 bytes for the prior read


/* Teensy 3.1 wiring diagram 

 Teensy 3.1 only:
 ----------------
 +5v -> Teensy Vin
 Gnd -> Teensy Gnd
 Teensy 3.3v -> Teensy pin 21 / A7 (voltage reference read)
 

 
 LED Strip:
 ----------------
 Teensy STRIP_PIN (pin 2) -> resistor (R?) -> Data in of LED strip
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
 Teensy SD_CARD_CS_PIN -> SD SS        // SD_CARD_CS_PIN is pin 4 (not pin 10!! which is what is defined as SS normallly for teensy 3.0)
 Teensy MOSI (pin 11) -> SD MOSI
 Teensy MISO (pin 12) -> SD MISO
 Teensy SCLK (pin 13) -> SD SCLK
 
 Gyro/Accel: See http://www.pjrc.com/teensy/td_libs_Wire.html and http://www.pololu.com/product/1268 and https://github.com/pololu/minimu-9-ahrs-arduino
 ----------------
 +5v -> Vin (NOT VDD)
 Gnd -> Gnd
 Teensy 18 -> SDA
 Teensy 19 -> SCL
 Then, these two need a pull up. Add +5v to a 4.7k resistor to the break between Teensy 18/SDA. Same goes for 19/SCL.
 
 
*/






#endif
