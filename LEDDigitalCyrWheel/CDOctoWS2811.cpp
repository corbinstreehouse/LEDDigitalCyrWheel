//
//  CDOctoWS2811.h
//  LEDDigitalCyrWheel
//
//  Created by corbin dunn on 2/11/14.
//
//

#include "CDOctoWS2811.h"

CDOctoWS2811::~CDOctoWS2811() {
    free(drawBuffer);
    free(frameBuffer);
}

