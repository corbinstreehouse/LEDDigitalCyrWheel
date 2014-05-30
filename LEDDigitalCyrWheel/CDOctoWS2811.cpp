//
//  CDOctoWS2811.h
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/11/14.
//
//

#include "CDLEDStripPatterns.h"
#if !USE_ADAFRUIT

#include "CDOctoWS2811.h"

CDOctoWS2811::~CDOctoWS2811() {
    free(drawBuffer);
    free(frameBuffer);
}


#endif