//
//  CDOrientation.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 4/19/14 .
//
//
// http://www.pololu.com/file/download/LSM303DLH-compass-app-note.pdf?file_id=0J434

#include "CDBaseOrientation.h"
#include "LEDCommon.h"

uint8_t CDBaseOrientation::getRotationalVelocityBrightness(uint8_t currentBrightness) {
#define MIN_VELOCITY_BRIGHTNESS 50
#define MAX_VELOCITY_BRIGHTNESS 240 // Not too bright....but seems bright
    
    uint8_t targetBrightness = 0;
#define MAX_ROTATIONAL_VELOCITY 800 // at this value, we hit max brightness, but it is hard to hit..usually I hit half
    double velocity = getRotationalVelocity();
    //    DEBUG_PRINTF("Velocity %.3f\r\n", velocity);
    
    if (velocity >= MAX_ROTATIONAL_VELOCITY) {
        targetBrightness = MAX_VELOCITY_BRIGHTNESS;
    } else {
        float percentage = velocity / (float)MAX_ROTATIONAL_VELOCITY;
        // quadric growth slows it down at the start
        percentage = percentage*percentage*percentage;
        
        float diffBetweenMinMax = MAX_VELOCITY_BRIGHTNESS - MIN_VELOCITY_BRIGHTNESS;
        targetBrightness = MIN_VELOCITY_BRIGHTNESS + round(percentage * diffBetweenMinMax);
    }
    
    uint8_t result = currentBrightness;
    
    if (m_isFirstPass) {
        m_targetBrightness = targetBrightness;
        result = targetBrightness;
        //        m_strip->setBrightness(targetBrightness);
    }
    //    uint8_t currentBrightness = m_strip->getBrightness();
    
    if (m_targetBrightness != targetBrightness) {
        m_targetBrightness = targetBrightness;
        
        if (m_startBrightness != currentBrightness) {
            m_startBrightness = currentBrightness;
            m_brightnessStartTime = millis();
            DEBUG_PRINTF("START Brightness changed: current: %d target: %d\r\n", currentBrightness, targetBrightness);
        } else {
            DEBUG_PRINTF("TARGET Brightness changed: current: %d target: %d\r\n", currentBrightness, targetBrightness);
        }
    }
    
    if (currentBrightness != m_targetBrightness) {
#define BRIGHTNESS_RAMPUP_TIME 200.0 // ms  -- this smooths things out
        float timePassed = millis() - m_brightnessStartTime;
        if (timePassed > BRIGHTNESS_RAMPUP_TIME || timePassed < 0) {
            result = m_targetBrightness;
        } else {
            float percentagePassed = timePassed / BRIGHTNESS_RAMPUP_TIME;
            //y =x^2  exponential ramp up (desired?)
            //            percentagePassed = sq(percentagePassed);
            float targetDiff = m_targetBrightness - m_startBrightness;
            uint8_t brightness = m_startBrightness + round(percentagePassed * targetDiff);
            //            DEBUG_PRINTF("    Changing brightness: %d,  time passed: %.3f   targetDiff: %.3f,    newB: %d\r\n", currentBrightness, timePassed, targetDiff, brightness);
            result = brightness;
        }
    }
    //    DEBUG_PRINTF("brightness: %d\r\n", result);
    return result;
}
