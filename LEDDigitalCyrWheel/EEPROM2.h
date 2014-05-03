//
//  EEPROM2.h
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 5/2/14 .
//
//

#ifndef LEDDigitalCyrWheel_EEPROM2_h
#define LEDDigitalCyrWheel_EEPROM2_h

#include <EEPROM.h>
#include <Arduino.h>  // for type definitions

template <class T> int EEPROM_Write(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_Read(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    return i;
}


#endif
