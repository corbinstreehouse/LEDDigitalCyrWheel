//
//  CDPololuOrientation.h
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 4/19/14 .
//Based off the following:
// MinIMU-9-Arduino-AHRS
//Pololu MinIMU-9 + Arduino AHRS (Attitude and Heading Reference System)
//
//MinIMU-9-Arduino-AHRS is based on sf9domahrs by Doug Weibel and Jose Julio:
//http://code.google.com/p/sf9domahrs/
//
//sf9domahrs is based on ArduIMU v1.5 by Jordi Munoz and William Premerlani, Jose
//Julio and Doug Weibel:
//http://code.google.com/p/ardu-imu/
//
//

#ifndef __LEDDigitalCyrWheel__CDPololuOrientation__
#define __LEDDigitalCyrWheel__CDPololuOrientation__


#include  "Arduino.h"

#include "CDBaseOrientation.h"

#include <Wire.h>
#include <L3G.h>
#include <LSM303.h>
#include "SdFat.h"

#define SHORT_FILENAME_LENGTH 13 // 12 + 1 for NULL, and needs to be 256 or so for the simulator if I use it there!

class CDPololuOrientation : CDBaseOrientation {
private:
    bool m_gyroInitialized;
    bool m_compassInitialized;

    char _filenameBuffer[SHORT_FILENAME_LENGTH+1]; // +1 for root path
    L3G _gyro;
    LSM303 _compass;
    
    // TODO: cleanup variables
    int SENSOR_SIGN[9];

    float G_Dt=0.02;    // Integration time (DCM algorithm)  We will run the integration loop at 50Hz if possible
    long timer=0;   //general purpuse timer
    long timer_old;
    long timer24=0; //Second timer used to print values
    int AN[6]; //array that stores the gyro and accelerometer data
    int AN_OFFSET[6]={0,0,0,0,0,0}; //Array that stores the Offset of the sensors
    
    float DCM_Matrix[3][3]= {
        {
            1,0,0  }
        ,{
            0,1,0  }
        ,{
            0,0,1  }
    };
    float Update_Matrix[3][3]={{0,1,2},{3,4,5},{6,7,8}}; //Gyros here
    
    
    float Temporary_Matrix[3][3]={
        {
            0,0,0  }
        ,{
            0,0,0  }
        ,{
            0,0,0  }
    };
    
    int gyro_x;
    int gyro_y;
    int gyro_z;
    int accel_x;
    int accel_y;
    int accel_z;
    int magnetom_x;
    int magnetom_y;
    int magnetom_z;
    float c_magnetom_x;
    float c_magnetom_y;
    float c_magnetom_z;
    float MAG_Heading;
    
    float Accel_Vector[3]= {0,0,0}; //Store the acceleration in a vector
    float Gyro_Vector[3]= {0,0,0};//Store the gyros turn rate in a vector
    float Omega_Vector[3]= {0,0,0}; //Corrected Gyro_Vector data
    float Omega_P[3]= {0,0,0};//Omega Proportional correction
    float Omega_I[3]= {0,0,0};//Omega Integrator
    float Omega[3]= {0,0,0};
    
    // Euler angles
    float roll;
    float pitch;
    float yaw;
    
    float errorRollPitch[3]= {0,0,0}; 
    float errorYaw[3]= {0,0,0};
    
    unsigned int counter=0;
    byte gyro_sat=0;

    LSM303::vector<int16_t> _calibrationMin;
    LSM303::vector<int16_t> _calibrationMax;
    
    
    void _calibrate();
    
    void normalize();
    void driftCorrection();
    void Matrix_update();
    void Euler_angles();
    void Compass_Heading();
    
    bool initGyro();
    bool initAccel();
    
    void readGyro();
    void readAccel();
    void readCompass();
    void _internalProcess();
    void _initDCMMatrix();
    
    void writeStatusToFile();
    
//    double m_maxVelocity;
//    double m_targetBrightness;
//    bool m_isFirstPass;
//    uint32_t m_brightnessStartTime;
//    uint8_t m_startBrightness;
    
public:
    CDPololuOrientation();
    bool init(); // returns false on failure to init. turn on debug for more info
    void process();
    void print();
    
    void beginCalibration();
    void endCalibration();
    void cancelCalibration();
    
    virtual void beginSavingData();
    virtual void endSavingData();
    
    double getAccelX(); // in g's
    double getAccelY();
    double getAccelZ();
    

    double getRotationalVelocity(); // In degress per second
    
    void writeOrientationData(Stream *stream);
    
};




#endif /* defined(__LEDDigitalCyrWheel__CDPololuOrientation__) */
