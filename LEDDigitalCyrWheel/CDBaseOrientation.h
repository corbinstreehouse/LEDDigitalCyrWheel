//
//  CDOrientation.h
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 4/19/14 .

#ifndef __LEDDigitalCyrWheel__CDBASEOrientation__
#define __LEDDigitalCyrWheel__CDBASEOrientation__


#include  "Arduino.h"

class CDBaseOrientation {
private:
    bool m_shouldSaveDataToFile;
    bool m_calibrating;

    double m_targetBrightness;
    
    bool m_isFirstPass;
    uint32_t m_brightnessStartTime;
    uint8_t m_startBrightness;
    
public:
    CDBaseOrientation()  {
        m_shouldSaveDataToFile = false;
        m_calibrating = false;
        m_targetBrightness = 0;
        m_isFirstPass = true;
        m_brightnessStartTime = 0;
        m_startBrightness = 0;
    }
    virtual bool init() { return true; }
    void process();
    
    bool isCalibrating() { return m_calibrating; }
    virtual void beginCalibration() {  m_calibrating = true; }
    virtual void endCalibration() { m_calibrating = false; }
    virtual void cancelCalibration() { m_calibrating = false; }
    
    bool isSavingData() { return m_shouldSaveDataToFile; }
    virtual void beginSavingData()  { m_shouldSaveDataToFile = true; }
    virtual void endSavingData() { }

    virtual double getRotationalVelocity() { return 0; }; // In degress per second

    void setFirstPass(bool isFirstPass) {
        m_isFirstPass = isFirstPass;
        m_brightnessStartTime = millis();
    };
    
    // Calculations for brightness based on the velocity of things for the cyr wheel
    uint8_t getRotationalVelocityBrightness(uint8_t currentBrightness);
    
#if !PATTERN_EDITOR
    virtual void writeOrientationData(Stream *stream) = 0;
#endif
    
};



#endif /* defined(__LEDDigitalCyrWheel__CDBASEOrientation__) */
