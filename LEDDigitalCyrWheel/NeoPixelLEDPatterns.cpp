//
//  NeoPixelLEDPatterns.cpp
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 5/31/14.
//
//

#include "NeoPixelLEDPatterns.h"


void NeoPixelLEDPatterns::begin() {
    m_strip.begin();
}

void NeoPixelLEDPatterns::internalShow() {
    m_strip.show();
}