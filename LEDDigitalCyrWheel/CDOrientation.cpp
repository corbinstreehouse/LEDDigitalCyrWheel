//
//  CDOrientation.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 4/19/14 .
//
//
// http://www.pololu.com/file/download/LSM303DLH-compass-app-note.pdf?file_id=0J434

#include "CDOrientation.h"
#include "LEDCommon.h"
#include "SdFat.h"

#if PATTERN_EDITOR

CDOrientation::CDOrientation() : m_maxVelocity(0), m_targetBrightness(0) {
}

bool CDOrientation::init() {
    return true;
}

void CDOrientation::process() {
}

void CDOrientation::beginCalibration() {
}

void CDOrientation::endCalibration() {
}

void CDOrientation::cancelCalibration() {
}


double CDOrientation::getAccelX() {
    return 0;
}

double CDOrientation::getAccelY() {
    return 0;
}

double CDOrientation::getAccelZ() {
    return 0;
}

double CDOrientation::getRotationalVelocity() {
    return 0;
}

void CDOrientation::beginSavingData() {
}

void CDOrientation::endSavingData() {
}

uint8_t CDOrientation::getRotationalVelocityBrightness(uint8_t currentBrightness) {
    return currentBrightness;
}

#else

#include "LEDDigitalCyrWheel.h" // For EEPROM_ADDRESS....i could make this settable.
#include "EEPROM.h"

// LSM303 magnetometer calibration constants; use the Calibrate example from  the Pololu LSM303 library to find the right values for your board
#define PRINT_DCM 0     //Will print the whole direction cosine matrix
#define PRINT_ANALOGS 0 //Will print the analog raw data
#define PRINT_EULER 1   //Will print the Euler angles Roll, Pitch and Yaw

// Uncomment the below line to use this axis definition:
// X axis pointing forward
// Y axis pointing to the right
// and Z axis pointing down.
// Positive pitch : nose up
// Positive roll : right wing down
// Positive yaw : clockwise
#define SENSOR_SIGN_INIT {1,1,1, -1,-1,-1, 1,1,1} //Correct directions x,y,z - gyro, accelerometer, magnetometer

// Uncomment the below line to use this axis definition:
// X axis pointing forward
// Y axis pointing to the left
// and Z axis pointing up.
// Positive pitch : nose down
// Positive roll : right wing down
// Positive yaw : counterclockwise
//SENSOR_SIGN_INIT {1,-1,-1, -1,1,1, 1,-1,-1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer











// LSM303 accelerometer: 8 g sensitivity
// 3.9 mg/digit; 1 g = 256
#define MG_PER_LSB 4 // based on +/-8g for the DHLC, according to the datasheet LA_So
#define GRAVITY (1000/MG_PER_LSB)  //this equivalent to 1G in the raw data coming from the accelerometer /// was 256..which isn't right for my chip!

#define ToRad(x) ((x)*0.01745329252)  // *pi/180
#define ToDeg(x) ((x)*57.2957795131)  // *180/pi

// L3G4200D gyro: 2000 dps full scale
// 70 mdps/digit; 1 dps = 0.07
// corbin: I'm using a L3GD20 gyro http://www.pololu.com/product/2124. Datasheet: http://www.pololu.com/file/download/L3GD20.pdf?file_id=0J563
// 200dps full scale

#define Gyro_Gain_X 0.07 //X axis Gyro gain
#define Gyro_Gain_Y 0.07 //Y axis Gyro gain
#define Gyro_Gain_Z 0.07 //Z axis Gyro gain
#define Gyro_Scaled_X(x) ((x)*ToRad(Gyro_Gain_X)) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Y(x) ((x)*ToRad(Gyro_Gain_Y)) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Z(x) ((x)*ToRad(Gyro_Gain_Z)) //Return the scaled ADC raw data of the gyro in radians for second

#define GYRO_RAW_VALUE_TO_DEG_PER_SEC(x) (x * Gyro_Gain_X) //


// I have no idea why or what these come from..
#define Kp_ROLLPITCH 0.02
#define Ki_ROLLPITCH 0.00002
#define Kp_YAW 1.2
#define Ki_YAW 0.00002

/*For debugging purposes*/
//OUTPUTMODE=1 will print the corrected data,
//OUTPUTMODE=0 will print uncorrected data of the gyros (with drift)
#define OUTPUTMODE 1

//Computes the dot product of two vectors
//http://www.mathsisfun.com/algebra/vectors-dot-product.html
float Vector_Dot_Product(float vector1[3],float vector2[3])
{
    float op=0;
    
    for(int c=0; c<3; c++)
    {
        op+=vector1[c]*vector2[c];
    }
    
    return op;
}

//Computes the cross product of two vectors
void Vector_Cross_Product(float vectorOut[3], float v1[3],float v2[3])
{
    vectorOut[0]= (v1[1]*v2[2]) - (v1[2]*v2[1]);
    vectorOut[1]= (v1[2]*v2[0]) - (v1[0]*v2[2]);
    vectorOut[2]= (v1[0]*v2[1]) - (v1[1]*v2[0]);
}

//Multiply the vector by a scalar.
void Vector_Scale(float vectorOut[3],float vectorIn[3], float scale2)
{
    for(int c=0; c<3; c++)
    {
        vectorOut[c]=vectorIn[c]*scale2;
    }
}

void Vector_Add(float vectorOut[3],float vectorIn1[3], float vectorIn2[3])
{
    for(int c=0; c<3; c++)
    {
        vectorOut[c]=vectorIn1[c]+vectorIn2[c];
    }
}


/**************************************************/
//Multiply two 3x3 matrixs. This function developed by Jordi can be easily adapted to multiple n*n matrix's. (Pero me da flojera!).
void Matrix_Multiply(float a[3][3], float b[3][3],float mat[3][3])
{
    float op[3];
    for(int x=0; x<3; x++)
    {
        for(int y=0; y<3; y++)
        {
            for(int w=0; w<3; w++)
            {
                op[w]=a[x][w]*b[w][y];
            }
            mat[x][y]=0;
            mat[x][y]=op[0]+op[1]+op[2];
            
            float test = mat[x][y];
        }
    }
}


CDOrientation::CDOrientation() {
    m_shouldSaveDataToFile = false;
    m_calibrating = false;
    m_startBrightness = 0;
    m_gyroInitialized = false;
    m_compassInitialized = false;
}

/**************************************************/
void CDOrientation::normalize()
{
//    float error=0;
//    float temporary[3][3];
//    float renorm=0;
//    
//    error= -Vector_Dot_Product(&DCM_Matrix[0][0],&DCM_Matrix[1][0])*.5; //eq.19
//    
//    Vector_Scale(&temporary[0][0], &DCM_Matrix[1][0], error); //eq.19
//    Vector_Scale(&temporary[1][0], &DCM_Matrix[0][0], error); //eq.19
//    
//    Vector_Add(&temporary[0][0], &temporary[0][0], &DCM_Matrix[0][0]);//eq.19
//    Vector_Add(&temporary[1][0], &temporary[1][0], &DCM_Matrix[1][0]);//eq.19
//    
//    Vector_Cross_Product(&temporary[2][0],&temporary[0][0],&temporary[1][0]); // c= a x b //eq.20
//    
//    renorm= .5 *(3 - Vector_Dot_Product(&temporary[0][0],&temporary[0][0])); //eq.21
//    Vector_Scale(&DCM_Matrix[0][0], &temporary[0][0], renorm);
//    
//    renorm= .5 *(3 - Vector_Dot_Product(&temporary[1][0],&temporary[1][0])); //eq.21
//    Vector_Scale(&DCM_Matrix[1][0], &temporary[1][0], renorm);
//    
//    renorm= .5 *(3 - Vector_Dot_Product(&temporary[2][0],&temporary[2][0])); //eq.21
//    Vector_Scale(&DCM_Matrix[2][0], &temporary[2][0], renorm);
//   


    // corbin technique:
    // http://www.ditutor.com/vec/normalizing_vector.html
    // http://www.ditutor.com/vec/vector_magnitude.html
//    for (int i = 0; i < 3; i++) {
//        float dcmMagnitude = sqrt(sq(DCM_Matrix[i][0]) +  sq(DCM_Matrix[i][1]) + sq(DCM_Matrix[i][2]));
//        for (int j = 0; j < 3; j++) {
//            DCM_Matrix[i][j] = DCM_Matrix[i][j] / dcmMagnitude;
//        }
//    }

    float error=0;
    float temporary[3][3];
    float renorm=0;
    bool problem=false;
    
    error= -Vector_Dot_Product(&DCM_Matrix[0][0],&DCM_Matrix[1][0])*.5; //eq.19
    
    Vector_Scale(&temporary[0][0], &DCM_Matrix[1][0], error); //eq.19
    Vector_Scale(&temporary[1][0], &DCM_Matrix[0][0], error); //eq.19
    
    Vector_Add(&temporary[0][0], &temporary[0][0], &DCM_Matrix[0][0]);//eq.19
    Vector_Add(&temporary[1][0], &temporary[1][0], &DCM_Matrix[1][0]);//eq.19
    
    Vector_Cross_Product(&temporary[2][0],&temporary[0][0],&temporary[1][0]); // c= a x b //eq.20
    
    renorm= Vector_Dot_Product(&temporary[0][0],&temporary[0][0]);
    if (renorm < 1.5625f && renorm > 0.64f) {
        renorm= .5 * (3-renorm);                                                 //eq.21
    } else if (renorm < 100.0f && renorm > 0.01f) {
        renorm= 1. / sqrt(renorm);
#if PERFORMANCE_REPORTING == 1
        renorm_sqrt_count++;
#endif
        
#define PRINT_DEBUG 0
        
#if PRINT_DEBUG != 0
        Serial.print("???SQT:1,RNM:");
        Serial.print (renorm);
        Serial.print (",ERR:");
        Serial.print (error);
        Serial.println("***");
#endif
    } else {
        problem = true;
#if PERFORMANCE_REPORTING == 1
        renorm_blowup_count++;
#endif
#if PRINT_DEBUG != 0
        Serial.print("???PRB:1,RNM:");
        Serial.print (renorm);
        Serial.print (",ERR:");
        Serial.print (error);
        Serial.println("***");
#endif
    }
    Vector_Scale(&DCM_Matrix[0][0], &temporary[0][0], renorm);
    
    renorm= Vector_Dot_Product(&temporary[1][0],&temporary[1][0]);
    if (renorm < 1.5625f && renorm > 0.64f) {
        renorm= .5 * (3-renorm);                                                 //eq.21
    } else if (renorm < 100.0f && renorm > 0.01f) {
        renorm= 1. / sqrt(renorm);
#if PERFORMANCE_REPORTING == 1
        renorm_sqrt_count++;
#endif
#if PRINT_DEBUG != 0
        Serial.print("???SQT:2,RNM:");
        Serial.print (renorm);
        Serial.print (",ERR:");
        Serial.print (error);
        Serial.println("***");
#endif
    } else {
        problem = true;
#if PERFORMANCE_REPORTING == 1
        renorm_blowup_count++;
#endif
#if PRINT_DEBUG != 0
        Serial.print("???PRB:2,RNM:");
        Serial.print (renorm);
        Serial.print (",ERR:");
        Serial.print (error);
        Serial.println("***");
#endif
    }
    Vector_Scale(&DCM_Matrix[1][0], &temporary[1][0], renorm);
    
    renorm= Vector_Dot_Product(&temporary[2][0],&temporary[2][0]);
    if (renorm < 1.5625f && renorm > 0.64f) {
        renorm= .5 * (3-renorm);                                                 //eq.21
    } else if (renorm < 100.0f && renorm > 0.01f) {
        renorm= 1. / sqrt(renorm);
#if PERFORMANCE_REPORTING == 1
        renorm_sqrt_count++;
#endif
#if PRINT_DEBUG != 0
        Serial.print("???SQT:3,RNM:");
        Serial.print (renorm);
        Serial.print (",ERR:");
        Serial.print (error);
        Serial.println("***");
#endif
    } else {
        problem = true;
#if PERFORMANCE_REPORTING == 1
        renorm_blowup_count++;
#endif
#if PRINT_DEBUG != 0
        Serial.print("???PRB:3,RNM:");
        Serial.print (renorm);
        Serial.println("***");
#endif
    }
    Vector_Scale(&DCM_Matrix[2][0], &temporary[2][0], renorm);
    
    if (problem) {                // Our solution is blowing up and we will force back to initial condition.  Hope we are not upside down!
        _initDCMMatrix();
        problem = false;
    }
    
}

/**************************************************/
void CDOrientation::driftCorrection()
{
    float mag_heading_x;
    float mag_heading_y;
    float errorCourse;
    //Compensation the Roll, Pitch and Yaw drift.
    static float Scaled_Omega_P[3];
    static float Scaled_Omega_I[3];
    float Accel_magnitude;
    float Accel_weight;
    
    
    //*****Roll and Pitch***************
    
    // Calculate the magnitude of the accelerometer vector
    Accel_magnitude = sqrt(Accel_Vector[0]*Accel_Vector[0] + Accel_Vector[1]*Accel_Vector[1] + Accel_Vector[2]*Accel_Vector[2]);
    Accel_magnitude = Accel_magnitude / GRAVITY; // Scale to gravity.
    // Dynamic weighting of accelerometer info (reliability filter)
    // Weight for accelerometer info (<0.5G = 0.0, 1G = 1.0 , >1.5G = 0.0)
    Accel_weight = constrain(1 - 2*abs(1 - Accel_magnitude),0,1);  //
    
    Vector_Cross_Product(&errorRollPitch[0],&Accel_Vector[0],&DCM_Matrix[2][0]); //adjust the ground of reference
    Vector_Scale(&Omega_P[0],&errorRollPitch[0],Kp_ROLLPITCH*Accel_weight);
    
    Vector_Scale(&Scaled_Omega_I[0],&errorRollPitch[0],Ki_ROLLPITCH*Accel_weight);
    Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);
    
    //*****YAW***************
    // We make the gyro YAW drift correction based on compass magnetic heading
    
    mag_heading_x = cos(MAG_Heading);
    mag_heading_y = sin(MAG_Heading);
    errorCourse=(DCM_Matrix[0][0]*mag_heading_y) - (DCM_Matrix[1][0]*mag_heading_x);  //Calculating YAW error
    Vector_Scale(errorYaw,&DCM_Matrix[2][0],errorCourse); //Applys the yaw correction to the XYZ rotation of the aircraft, depeding the position.
    
    Vector_Scale(&Scaled_Omega_P[0],&errorYaw[0],Kp_YAW);//.01proportional of YAW.
    Vector_Add(Omega_P,Omega_P,Scaled_Omega_P);//Adding  Proportional.
    
    Vector_Scale(&Scaled_Omega_I[0],&errorYaw[0],Ki_YAW);//.00001Integrator
    Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);//adding integrator to the Omega_I
}
/**************************************************/
/*
 void Accel_adjust(void)
 {
 Accel_Vector[1] += Accel_Scale(speed_3d*Omega[2]);  // Centrifugal force on Acc_y = GPS_speed*GyroZ
 Accel_Vector[2] -= Accel_Scale(speed_3d*Omega[1]);  // Centrifugal force on Acc_z = GPS_speed*GyroY
 }
 */
/**************************************************/

void CDOrientation::Matrix_update()
{
    Gyro_Vector[0]=Gyro_Scaled_X(gyro_x); //gyro x roll
    Gyro_Vector[1]=Gyro_Scaled_Y(gyro_y); //gyro y pitch
    Gyro_Vector[2]=Gyro_Scaled_Z(gyro_z); //gyro Z yaw
    
    Accel_Vector[0]=accel_x;
    Accel_Vector[1]=accel_y;
    Accel_Vector[2]=accel_z;
    
    Vector_Add(&Omega[0], &Gyro_Vector[0], &Omega_I[0]);  //adding proportional term
    Vector_Add(&Omega_Vector[0], &Omega[0], &Omega_P[0]); //adding Integrator term
    
    //Accel_adjust();    //Remove centrifugal acceleration.   We are not using this function in this version - we have no speed measurement
    
#if OUTPUTMODE==1
    Update_Matrix[0][0]=0;
    Update_Matrix[0][1]=-G_Dt*Omega_Vector[2];//-z
    Update_Matrix[0][2]=G_Dt*Omega_Vector[1];//y
    Update_Matrix[1][0]=G_Dt*Omega_Vector[2];//z
    Update_Matrix[1][1]=0;
    Update_Matrix[1][2]=-G_Dt*Omega_Vector[0];//-x
    Update_Matrix[2][0]=-G_Dt*Omega_Vector[1];//-y
    Update_Matrix[2][1]=G_Dt*Omega_Vector[0];//x
    Update_Matrix[2][2]=0;
#else                    // Uncorrected data (no drift correction)
    Update_Matrix[0][0]=0;
    Update_Matrix[0][1]=-G_Dt*Gyro_Vector[2];//-z
    Update_Matrix[0][2]=G_Dt*Gyro_Vector[1];//y
    Update_Matrix[1][0]=G_Dt*Gyro_Vector[2];//z
    Update_Matrix[1][1]=0;
    Update_Matrix[1][2]=-G_Dt*Gyro_Vector[0];
    Update_Matrix[2][0]=-G_Dt*Gyro_Vector[1];
    Update_Matrix[2][1]=G_Dt*Gyro_Vector[0];
    Update_Matrix[2][2]=0;
#endif
    
    Matrix_Multiply(DCM_Matrix,Update_Matrix,Temporary_Matrix); //a*b=c
    
    for(int x=0; x<3; x++) //Matrix Addition (update)
    {
        for(int y=0; y<3; y++)
        {
            DCM_Matrix[x][y]+=Temporary_Matrix[x][y];
        }
    }
}

void CDOrientation::Euler_angles(void)
{
#if (OUTPUTMODE==2)         // Only accelerometer info (debugging purposes)
    roll = 1.9*atan2(Accel_Vector[1],Accel_Vector[2]);    // atan2(acc_y,acc_z)
    pitch = -1.9*asin((Accel_Vector[0])/(double)GRAVITY); // asin(acc_x)
    yaw = 0;
#else
    pitch = -1.9*asin(DCM_Matrix[2][0]);
    roll = 1.9*atan2(DCM_Matrix[2][1],DCM_Matrix[2][2]);
    yaw = atan2(DCM_Matrix[1][0],DCM_Matrix[0][0]);
#endif
}



void CDOrientation::Compass_Heading()
{
    float MAG_X;
    float MAG_Y;
    float cos_roll;
    float sin_roll;
    float cos_pitch;
    float sin_pitch;
    
    cos_roll = cos(roll);
    sin_roll = sin(roll);
    cos_pitch = cos(pitch);
    sin_pitch = sin(pitch);
    
    // adjust for LSM303 compass axis offsets/sensitivity differences by scaling to +/-0.5 range
    c_magnetom_x = (float)(magnetom_x - SENSOR_SIGN[6]*_compass.m_min.x) / (_compass.m_max.x - _compass.m_min.x) - SENSOR_SIGN[6]*0.5;
    c_magnetom_y = (float)(magnetom_y - SENSOR_SIGN[7]*_compass.m_min.y) / (_compass.m_max.y - _compass.m_min.y) - SENSOR_SIGN[7]*0.5;
    c_magnetom_z = (float)(magnetom_z - SENSOR_SIGN[8]*_compass.m_min.z) / (_compass.m_max.z - _compass.m_min.z) - SENSOR_SIGN[8]*0.5;
    
    // Tilt compensated Magnetic filed X:
    MAG_X = c_magnetom_x*cos_pitch+c_magnetom_y*sin_roll*sin_pitch+c_magnetom_z*cos_roll*sin_pitch;
    // Tilt compensated Magnetic filed Y:
    MAG_Y = c_magnetom_y*cos_roll-c_magnetom_z*sin_roll;
    // Magnetic Heading
    MAG_Heading = atan2(-MAG_Y,MAG_X);
}

static inline float convert_to_dec(float x)
{
    return x; // *10000000;
}

void CDOrientation::_initDCMMatrix() {
    bzero(&DCM_Matrix, sizeof(DCM_Matrix));
    //    DCM_Matrix[0] = { 1, 0, 0};
    DCM_Matrix[0][0] = 1;
    //    DCM_Matrix[1] = { 0, 1, 0};
    DCM_Matrix[1][1] = 1;
    //    DCM_Matrix[2] = { 0, 0, 1};
    DCM_Matrix[2][2] = 1;
}

bool CDOrientation::init() {
    SENSOR_SIGN[0] = SENSOR_SIGN[1] = SENSOR_SIGN[2] = 1;
    SENSOR_SIGN[3] = SENSOR_SIGN[4] = SENSOR_SIGN[5] = -1;
    SENSOR_SIGN[6] = SENSOR_SIGN[7] = SENSOR_SIGN[8] = 1;
    G_Dt=0.02;
    timer = 0;   //general purpuse timer
    timer_old = 0;
    timer24=0; //Second timer used to print values
    bzero(&AN_OFFSET, sizeof(AN_OFFSET));
//    AN_OFFSET={0,0,0,0,0,0}; //Array that stores the Offset of the sensors
    
    _initDCMMatrix();
    
//    Update_Matrix = {{0,1,2},{3,4,5},{6,7,8}}; //Gyros here
    int tmp = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Update_Matrix[i][j] = tmp;
            tmp++;
        }
    }
    

    bzero(&Temporary_Matrix, sizeof(Temporary_Matrix));
    bzero(&Accel_Vector, sizeof(Accel_Vector));
    bzero(&Omega_Vector, sizeof(Omega_Vector));
    bzero(&Omega_P, sizeof(Omega_P));
    bzero(&Omega_I, sizeof(Omega_I));
    bzero(&Omega, sizeof(Omega));
    
    roll, pitch, yaw = 0;
    
    bzero(&errorRollPitch, sizeof(errorRollPitch));
    bzero(&errorYaw, sizeof(errorYaw));
    
    counter=0;
    gyro_sat=0;
    
    Wire.begin();
    bool result = initGyro() && initAccel();
    if (result) {
        DEBUG_PRINTLN("Orientation initialized");

        // Don't do this calibration! we want the same position always....
//        
//        // NOTE: not so sure why we do this... is it needed?? (I think it figured out the current orientation as default
//        for(int i=0;i<32;i++)    // We take some readings...
//        {
//            readGyro();
//            readAccel();
//            for(int y=0; y<6; y++)   // Cumulate values
//                AN_OFFSET[y] += AN[y];
//            delay(20);
//        }
//        
//        for(int y=0; y<6; y++) {
//            AN_OFFSET[y] = AN_OFFSET[y]/32;
//        }
//        
//
//        // corbin, evaluate...
//        AN_OFFSET[5] -= GRAVITY*SENSOR_SIGN[5];
        
#if DEBUG
        DEBUG_PRINTLN("Offset:");
        for(int y=0; y<6; y++)
            DEBUG_PRINTLN(AN_OFFSET[y]);
#endif

        // init the compass right now; we utilize it!
        readCompass();    // Read I2C magnetometer
        Compass_Heading(); // Calculate magnetic heading
        
        _internalProcess();
#warning corbin i can probably remove the above line
        
        DEBUG_PRINTLN("Orientation initial read done...");

    }
    
    return result;
}

bool CDOrientation::initGyro() {
    if (!m_gyroInitialized) {
        DEBUG_PRINTLN("  initGyro");
        // why do I try 10 times??
        for (int i = 0; i < 10; i++) {
            m_gyroInitialized = _gyro.init();
            if (m_gyroInitialized) {
                _gyro.writeReg(L3G_CTRL_REG4, 0x20); // 2000 dps full scale. See http://www.pololu.com/file/download/L3GD20.pdf?file_id=0J563
                _gyro.writeReg(L3G_CTRL_REG1, 0x0F); // normal power mode, all axes enabled, 100 Hz
                break;
            } else {
                DEBUG_PRINTLN(" -> failed");
            }
        }
        if (!m_gyroInitialized) DEBUG_PRINTLN("FAILED TO init gyro");
    }
    return m_gyroInitialized;
}

// rather abitrary...
#define IS_VALID_MIN_COMPASS_VALUE(x) ((x > -4000 && x <= 0) ? true : false)
#define IS_VALID_MAX_COMPASS_VALUE(x) ((x > 0 && x < 4000) ? true : false)

bool CDOrientation::initAccel() {
    DEBUG_PRINTLN("initAccel");
    if (!m_compassInitialized) {
        for (int i = 0; i < 10; i++) {
            m_compassInitialized = _compass.init();
            if (m_compassInitialized) {
                break;
            }
        }
    }
    if (m_compassInitialized) {
        _compass.enableDefault();
        switch (_compass.getDeviceType())
        {
            case LSM303::device_D:
                _compass.writeReg(LSM303::CTRL2, 0x18); // 8 g full scale: AFS = 011
                break;
            case LSM303::device_DLHC: /// corbin: NOTE: what i'm using.
                // 0x28 == 0010 1000
                _compass.writeReg(LSM303::CTRL_REG4_A, 0x28); // 8 g full scale: FS = 10; high resolution output mode. Setting the LA_FS from the datasheet
                // That causes the LA_So to be: 4 mg/LSB
                
                _compass.writeMagReg(LSM303::CRB_REG_M, 0b10000000); // gain: ±4.0 Gauss (see datasheet)
                
                break;
            default: // DLM, DLH
                _compass.writeReg(LSM303::CTRL_REG4_A, 0x30); // 8 g full scale: FS = 11
        }
        // values I read from Calibrate for the given gain setting; I could have a program/mode to read/write these
//        EEPROM.write(EEPROM_MIN_MAX_IS_SAVED_ADDRESS, false); // corbin: flash the chip with this once on new chips so they don't have random data in them
        
        bool isMinMaxSaved = EEPROM.read(EEPROM_MIN_MAX_IS_SAVED_ADDRESS) != 0;
        if (isMinMaxSaved) {
            DEBUG_PRINTLN("loading saved compass calibration values");
            LSM303::vector<int16_t> savedMin;
            LSM303::vector<int16_t> savedMax;
            EEPROM.get(MIN_EEPROM_ADDRESS, savedMin);
            EEPROM.get(EEPROM_ACCELEROMETER_MAX_ADDRESS, savedMax);
            // Validate them before using them
            if (IS_VALID_MIN_COMPASS_VALUE(savedMin.x) && IS_VALID_MIN_COMPASS_VALUE(savedMin.y) && IS_VALID_MIN_COMPASS_VALUE(savedMin.z)) {
                _compass.m_min = savedMin;
                DEBUG_PRINTF("SAVED calibration MIN: %d, %d, %d\r\n", savedMin.x, savedMin.y, savedMin.z);
            } else {
                DEBUG_PRINTLN("---------------- INVALID SAVED MIN VALUE IN EEPROM");
                // hardcode!!
                _compass.m_min = (LSM303::vector<int16_t>){ -64, -114, 0 };
            }
            if (IS_VALID_MAX_COMPASS_VALUE(savedMax.x) && IS_VALID_MAX_COMPASS_VALUE(savedMax.y) && IS_VALID_MAX_COMPASS_VALUE(savedMax.z)) {
                _compass.m_max = savedMax;
                DEBUG_PRINTF("SAVED calibration MAX: %d, %d, %d\r\n", savedMax.x, savedMax.y, savedMax.z);
            } else {
                DEBUG_PRINTLN("---------------- INVALID SAVED max VALUE IN EEPROM");
                _compass.m_max = (LSM303::vector<int16_t>){ 0, 0, 102 };
            }
        } else {
            // Default??
        }
    } else {
        DEBUG_PRINTLN("FAILED TO INIT accel/compass ");
    }
    return m_compassInitialized;
}

void CDOrientation::readGyro()
{
    if (!m_gyroInitialized) return;
    _gyro.read();
    
    AN[0] = _gyro.g.x;
    AN[1] = _gyro.g.y;
    AN[2] = _gyro.g.z;
    gyro_x = SENSOR_SIGN[0] * (AN[0] - AN_OFFSET[0]);
    gyro_y = SENSOR_SIGN[1] * (AN[1] - AN_OFFSET[1]);
    gyro_z = SENSOR_SIGN[2] * (AN[2] - AN_OFFSET[2]);
}

// Reads x,y and z accelerometer registers
void CDOrientation::readAccel() {
    if (!m_compassInitialized) return;
    
    _compass.readAcc();
    
    
    // corbin, the DLHC has 12-bit rep, and we have to drop the last 4 bits.
    
    AN[3] = _compass.a.x >> 4; // shift right 4 bits to use 12-bit representation (1 g = 256)
    AN[4] = _compass.a.y >> 4;
    AN[5] = _compass.a.z >> 4;
    
    
    
    accel_x = SENSOR_SIGN[3] * (AN[3] - AN_OFFSET[3]);
    accel_y = SENSOR_SIGN[4] * (AN[4] - AN_OFFSET[4]);
    accel_z = SENSOR_SIGN[5] * (AN[5] - AN_OFFSET[5]);
}


void CDOrientation::readCompass() {
    if (!m_compassInitialized) return;
    
    _compass.readMag();
    
    magnetom_x = SENSOR_SIGN[6] * _compass.m.x;
    magnetom_y = SENSOR_SIGN[7] * _compass.m.y;
    magnetom_z = SENSOR_SIGN[8] * _compass.m.z;
}



void CDOrientation::_internalProcess() {
    if (m_compassInitialized && m_gyroInitialized) {
        
        counter++;
        timer_old = timer;
        timer = millis();
        if (timer>timer_old) {
            G_Dt = (timer-timer_old)/1000.0;    // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
            if (G_Dt > 1) {
                G_Dt = 0;  //Something is wrong - keeps dt from blowing up, goes to zero to keep gyros from departing
            }
        } else {
            G_Dt = 0;
        }
    //    DEBUG_PRINTF("G_Dt: %.3f\r\n--", G_Dt);
        
        // *** DCM algorithm
        // Data adquisition
        readGyro();   // This read gyro data
        readAccel();     // Read I2C accelerometer
        
        if (counter > 5)  // Read compass data at 10Hz... (5 loop runs)
        {
            counter = 0;
            readCompass();    // Read I2C magnetometer
            Compass_Heading(); // Calculate magnetic heading
        }
    //    print();
    //
    //    DEBUG_PRINTF("START: DCM: %.3f, %.3f, %.3f\r\n", DCM_Matrix[0][0], DCM_Matrix[0][1], DCM_Matrix[0][2]);
    //    DEBUG_PRINTF("Omega: %.3f %.3f %.3f\r\n", Omega[0], Omega[1], Omega[2]);
    //    DEBUG_PRINTF("Gyro_Vector: %.3f %.3f %.3f\r\n", Gyro_Vector[0], Gyro_Vector[1], Gyro_Vector[2]);
    //    DEBUG_PRINTF("Omega_I: %.3f %.3f %.3f\r\n", Omega_I[0], Omega_I[1], Omega_I[2]);
    //    DEBUG_PRINTF("Omega_P: %.3f %.3f %.3f\r\n---\r\n", Omega_P[0], Omega_P[1], Omega_P[2]);
    //    print();

        // Calculations...
        Matrix_update();

    //    DEBUG_PRINTF("AFTER MATRIX_UPDATE DCM: %.3f, %.3f, %.3f\r\n", DCM_Matrix[0][0], DCM_Matrix[0][1], DCM_Matrix[0][2]);
    //    DEBUG_PRINTF("Omega: %.3f %.3f %.3f\r\n", Omega[0], Omega[1], Omega[2]);
    //    DEBUG_PRINTF("Gyro_Vector: %.3f %.3f %.3f\r\n", Gyro_Vector[0], Gyro_Vector[1], Gyro_Vector[2]);
    //    DEBUG_PRINTF("Omega_I: %.3f %.3f %.3f\r\n", Omega_I[0], Omega_I[1], Omega_I[2]);
    //    DEBUG_PRINTF("Omega_P: %.3f %.3f %.3f\r\n++++\r\n", Omega_P[0], Omega_P[1], Omega_P[2]);
    //    print();

        normalize();

    //    DEBUG_PRINTF("AFTER NORM: DCM: %.3f, %.3f, %.3f\r\n", DCM_Matrix[0][0], DCM_Matrix[0][1], DCM_Matrix[0][2]);
    //    DEBUG_PRINTF("Omega: %.3f %.3f %.3f\r\n", Omega[0], Omega[1], Omega[2]);
    //    DEBUG_PRINTF("Gyro_Vector: %.3f %.3f %.3f\r\n", Gyro_Vector[0], Gyro_Vector[1], Gyro_Vector[2]);
    //    DEBUG_PRINTF("Omega_I: %.3f %.3f %.3f\r\n", Omega_I[0], Omega_I[1], Omega_I[2]);
    //    DEBUG_PRINTF("Omega_P: %.3f %.3f %.3f\r\n++++\r\n", Omega_P[0], Omega_P[1], Omega_P[2]);
    //    print();

        driftCorrection();

    //    DEBUG_PRINTF("AFTER DRIFT: DCM: %.3f, %.3f, %.3f\r\n", DCM_Matrix[0][0], DCM_Matrix[0][1], DCM_Matrix[0][2]);
    //    DEBUG_PRINTF("Omega: %.3f %.3f %.3f\r\n", Omega[0], Omega[1], Omega[2]);
    //    DEBUG_PRINTF("Gyro_Vector: %.3f %.3f %.3f\r\n", Gyro_Vector[0], Gyro_Vector[1], Gyro_Vector[2]);
    //    DEBUG_PRINTF("Omega_I: %.3f %.3f %.3f\r\n", Omega_I[0], Omega_I[1], Omega_I[2]);
    //    DEBUG_PRINTF("Omega_P: %.3f %.3f %.3f\r\n++++\r\n", Omega_P[0], Omega_P[1], Omega_P[2]);
    //    print();

        Euler_angles();
        
    //    DEBUG_PRINTLN("--DONE");
    #if DEBUG
    //    print();
    #endif
        
        if (m_shouldSaveDataToFile) {
            writeStatusToFile();
        }
    }
}


void CDOrientation::process() {
    if (m_calibrating) {
        _calibrate();
    } else if ((millis()-timer)>=20) {   // Main loop runs at 50Hz
        _internalProcess();
    }
}

void CDOrientation::beginCalibration() {
    _calibrationMin = {INT16_MAX, INT16_MAX, INT16_MAX};
    _calibrationMax = {INT16_MIN, INT16_MIN, INT16_MIN};
    _compass.m_min = _calibrationMin;
    _compass.m_max = _calibrationMax;
    m_calibrating = true;
}

void CDOrientation::_calibrate() {
    _compass.read();
    
    if (_compass.m.x == -4096) {
        DEBUG_PRINTLN("error X");
    } else {
        _calibrationMin.x = min(_calibrationMin.x, _compass.m.x);
        _calibrationMax.x = max(_calibrationMax.x, _compass.m.x);
    }
    
    if (_compass.m.y == -4096) {
        DEBUG_PRINTLN("error Y");
    } else {
        _calibrationMin.y = min(_calibrationMin.y, _compass.m.y);
        _calibrationMax.y = max(_calibrationMax.y, _compass.m.y);
    }
    
    if (_compass.m.z == -4096) {
        DEBUG_PRINTLN("error Z");
    } else {
        _calibrationMin.z = min(_calibrationMin.z, _compass.m.z);
        _calibrationMax.z = max(_calibrationMax.z, _compass.m.z);
    }

   // DEBUG_PRINTF("  compass.m_min.x = %+6d; compass.m_min.y = %+6d; compass.m_min.z = %+6d; \r\n  compass.m_max.x = %+6d; compass.m_max.y = %+6d; compass.m_max.z = %+6d;\r\n\r\n", _calibrationMin.x, _calibrationMin.y, _calibrationMin.z, _calibrationMax.x, _calibrationMax.y, _calibrationMax.z);
}

void CDOrientation::cancelCalibration() {
    DEBUG_PRINTLN("cancel calibration called");
    m_calibrating = false;
}

void CDOrientation::endCalibration() {
    DEBUG_PRINTLN("endCalibration called");
    m_calibrating = false;
    // TODO: maybe average it with the last calibration I did???
    
    _compass.m_min = _calibrationMin;
    _compass.m_max = _calibrationMax;
    EEPROM.write(EEPROM_MIN_MAX_IS_SAVED_ADDRESS, true);
    EEPROM.put(MIN_EEPROM_ADDRESS, _calibrationMin);
    EEPROM.put(EEPROM_ACCELEROMETER_MAX_ADDRESS, _calibrationMax);
}

double rawAccelToG(int16_t a) {
    // 12-bit preciscion; last 4 bits are 0.
    a = a >> 4;
    // According to the datasheet, the DHLC has a measurement of 4 mg/LSB. This means I take the raw value, multiply it by 4 and that gives me the mg's. Divide by 1000 and that is the g value. If I change the range i have to change this value!!
    double result = (a*MG_PER_LSB) / 1000.0;
    return result;
}

double CDOrientation::getAccelX() {
    return rawAccelToG(_compass.a.x);
}

double CDOrientation::getAccelY() {
    return rawAccelToG(_compass.a.y);
}

double CDOrientation::getAccelZ() {
    return rawAccelToG(_compass.a.z);
}

double CDOrientation::getRotationalVelocity() {
    // Get the length of the gyro vector
    double temp = sqrt(sq(_gyro.g.x) + sq(_gyro.g.y) + sq(_gyro.g.z));
    double result = GYRO_RAW_VALUE_TO_DEG_PER_SEC(temp);
    if (result > m_maxVelocity) {
        m_maxVelocity = result; // I might do something with this later
        //        DEBUG_PRINTF("new max velocity: %.3f\r\n", m_maxVelocity);
    }
    return result;
}

void CDOrientation::beginSavingData() {
    m_shouldSaveDataToFile = true;
    // figure out a new filename to try
    int v = 0;
    FatFile rootDir = FatFile("/", O_READ);
    sprintf(_filenameBuffer, "Gyro%d.txt", v);
    
    while (rootDir.exists(_filenameBuffer)) {
        v++;
        sprintf(_filenameBuffer, "Gyro%d.txt", v);
        if (v > 1024) {
            // Too many files...
            break;
        }
    }
    rootDir.close();
}

void CDOrientation::endSavingData() {
    m_shouldSaveDataToFile = false;
}

void CDOrientation::writeStatusToFile() {
    SdFile file = SdFile(_filenameBuffer, O_WRITE|O_APPEND);
    file.printf("%.0f,\t%.0f,\t%.0f,\t  RotationalVelocityDPS:%.1f\r\n",   GYRO_RAW_VALUE_TO_DEG_PER_SEC(_gyro.g.x), GYRO_RAW_VALUE_TO_DEG_PER_SEC(_gyro.g.y), GYRO_RAW_VALUE_TO_DEG_PER_SEC(_gyro.g.z), getRotationalVelocity());
    file.close();
}


void CDOrientation::print()
{
//    if (abs(_compass.m.x) < 350 && abs(_compass.m.y) < 350 && abs(_compass.m.z) < 350) {
//        Serial.printf("x %5d,   y %5d,    z %5d    a.x %5d %5d,   a.y %5d %4d,    a.z %5d %5d\r        ", _compass.m.x, _compass.m.y, _compass.m.z,      _compass.a.x, _compass.a.y, _compass.a.z, _compass.a.x >> 4, _compass.a.y >> 4, _compass.a.z >> 4);
    // 4 mg/LSB
    
       //     Serial.printf("a.x %5d %.3fg,   a.y %5d %.3fg,    a.z %5d  %.3fg\r        ",   _compass.a.x, rawAccelToG(_compass.a.x), _compass.a.y, rawAccelToG(_compass.a.y), _compass.a.z, rawAccelToG(_compass.a.z));

    
    Serial.printf("Gyro: %.3f     %.3f      %.3f\r        ",   GYRO_RAW_VALUE_TO_DEG_PER_SEC(_gyro.g.x), GYRO_RAW_VALUE_TO_DEG_PER_SEC(_gyro.g.y), GYRO_RAW_VALUE_TO_DEG_PER_SEC(_gyro.g.z));
    
    //    }
    return;
//    Serial.print("!");
    
#if PRINT_EULER == 1
//    Serial.printf("ANG:% 3.1f,% 3.1f,% 3.1f\r", ToDeg(roll), ToDeg(pitch), ToDeg(yaw));
    
    Serial.printf("pitch% 3.1f,  x mag: % 5d   x acc: % 5d\r", ToDeg(pitch), _compass.m.x, _compass.a.x);

    // _compass.m.y really seems to be the physical x!!
    
//    Serial.print("ANG:");
//    Serial.print(ToDeg(roll));
//    Serial.print(",");
//    Serial.print(ToDeg(pitch));
//    Serial.print(",");
//    Serial.print(ToDeg(yaw));
    
    
    
//    Serial.print(_compass.heading());
#endif
#if PRINT_ANALOGS==1
    Serial.print(",AN:");
    Serial.print(AN[0]);  //(int)read_adc(0)
    Serial.print(",");
    Serial.print(AN[1]);
    Serial.print(",");
    Serial.print(AN[2]);
    Serial.print(",");
    Serial.print(AN[3]);
    Serial.print (",");
    Serial.print(AN[4]);
    Serial.print (",");
    Serial.print(AN[5]);
    Serial.print(",");
    Serial.print(c_magnetom_x);
    Serial.print (",");
    Serial.print(c_magnetom_y);
    Serial.print (",");
    Serial.print(c_magnetom_z);
#endif
#if PRINT_DCM == 1
    Serial.print (",DCM:");
    Serial.print(convert_to_dec(DCM_Matrix[0][0]));
    Serial.print (",");
    Serial.print(convert_to_dec(DCM_Matrix[0][1]));
    Serial.print (",");
    Serial.print(convert_to_dec(DCM_Matrix[0][2]));
    Serial.print (",");
    Serial.print(convert_to_dec(DCM_Matrix[1][0]));
    Serial.print (",");
    Serial.print(convert_to_dec(DCM_Matrix[1][1]));
    Serial.print (",");
    Serial.print(convert_to_dec(DCM_Matrix[1][2]));
    Serial.print (",");
    Serial.print(convert_to_dec(DCM_Matrix[2][0]));
    Serial.print (",");
    Serial.print(convert_to_dec(DCM_Matrix[2][1]));
    Serial.print (",");
    Serial.print(convert_to_dec(DCM_Matrix[2][2]));
#endif
// Serial.println();
    
}

uint8_t CDOrientation::getRotationalVelocityBrightness(uint8_t currentBrightness) {
#define MIN_BRIGHTNESS 50
#define MAX_BRIGHTNESS 240 // Not too bright....but seems bright
    
    uint8_t targetBrightness = 0;
#define MAX_ROTATIONAL_VELOCITY 800 // at this value, we hit max brightness, but it is hard to hit..usually I hit half
    double velocity = getRotationalVelocity();
//    DEBUG_PRINTF("Velocity %.3f\r\n", velocity);

    if (velocity >= MAX_ROTATIONAL_VELOCITY) {
        targetBrightness = MAX_BRIGHTNESS;
    } else {
        float percentage = velocity / (float)MAX_ROTATIONAL_VELOCITY;
        // quadric growth slows it down at the start
        percentage = percentage*percentage*percentage;
        
        float diffBetweenMinMax = MAX_BRIGHTNESS - MIN_BRIGHTNESS;
        targetBrightness = MIN_BRIGHTNESS + round(percentage * diffBetweenMinMax);
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

#endif

