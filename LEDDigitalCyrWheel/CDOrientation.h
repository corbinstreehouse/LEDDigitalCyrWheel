//
//  CDOrientation.h
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

#ifndef __LEDDigitalCyrWheel__CDOrientation__
#define __LEDDigitalCyrWheel__CDOrientation__

#include  "Arduino.h"
#include <Wire.h>

#include <L3G.h>
#include <LSM303.h>


// LSM303 magnetometer calibration constants; use the Calibrate example from  the Pololu LSM303 library to find the right values for your board
// TODO: make these per-class constants
#define M_X_MIN -865 //-421
#define M_Y_MIN -1293 //-639
#define M_Z_MIN -370 //-238
#define M_X_MAX 602 //424
#define M_Y_MAX 409 // 295
#define M_Z_MAX 593 //472


//#define PRINT_DCM 0     //Will print the whole direction cosine matrix
#define PRINT_ANALOGS 0 //Will print the analog raw data
#define PRINT_EULER 1   //Will print the Euler angles Roll, Pitch and Yaw

class CDOrientation {
private:
    L3G _gyro;
    LSM303 _compass;
    
    // TODO: cleanup variables
    // Uncomment the below line to use this axis definition:
    // X axis pointing forward
    // Y axis pointing to the right
    // and Z axis pointing down.
    // Positive pitch : nose up
    // Positive roll : right wing down
    // Positive yaw : clockwise
    int SENSOR_SIGN[9] = {1,1,1,-1,-1,-1,1,1,1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer
    // Uncomment the below line to use this axis definition:
    // X axis pointing forward
    // Y axis pointing to the left
    // and Z axis pointing up.
    // Positive pitch : nose down
    // Positive roll : right wing down
    // Positive yaw : counterclockwise
    //int SENSOR_SIGN[9] = {1,-1,-1,-1,1,1,1,-1,-1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer

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
public:
    bool init(); // returns false on failure to init. turn on debug for more info
    void process();
    void print();
};




#endif /* defined(__LEDDigitalCyrWheel__CDOrientation__) */
