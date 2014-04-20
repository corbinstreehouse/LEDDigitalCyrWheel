//
//  CDOrientation.cpp
//  LEDDigitalCyrWheel
//
//  Created by Corbin Dunn on 4/19/14 .
//
//
// http://www.pololu.com/file/download/LSM303DLH-compass-app-note.pdf?file_id=0J434

#include "CDOrientation.h"

#include "CDLEDStripPatterns.h" // for debug-print

// LSM303 accelerometer: 8 g sensitivity
// 3.9 mg/digit; 1 g = 256
#define GRAVITY 256  //this equivalent to 1G in the raw data coming from the accelerometer

#define ToRad(x) ((x)*0.01745329252)  // *pi/180
#define ToDeg(x) ((x)*57.2957795131)  // *180/pi

// L3G4200D gyro: 2000 dps full scale
// 70 mdps/digit; 1 dps = 0.07
#define Gyro_Gain_X 0.07 //X axis Gyro gain
#define Gyro_Gain_Y 0.07 //Y axis Gyro gain
#define Gyro_Gain_Z 0.07 //Z axis Gyro gain
#define Gyro_Scaled_X(x) ((x)*ToRad(Gyro_Gain_X)) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Y(x) ((x)*ToRad(Gyro_Gain_Y)) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Z(x) ((x)*ToRad(Gyro_Gain_Z)) //Return the scaled ADC raw data of the gyro in radians for second

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
    for (int i = 0; i < 3; i++) {
        float dcmMagnitude = sqrt(sq(DCM_Matrix[i][0]) +  sq(DCM_Matrix[i][1]) + sq(DCM_Matrix[i][2]));
        for (int j = 0; j < 3; j++) {
            DCM_Matrix[i][j] = DCM_Matrix[i][j] / dcmMagnitude;
        }
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
    pitch = -asin(DCM_Matrix[2][0]);
    roll = atan2(DCM_Matrix[2][1],DCM_Matrix[2][2]);
    yaw = atan2(DCM_Matrix[1][0],DCM_Matrix[0][0]);
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
    c_magnetom_x = (float)(magnetom_x - SENSOR_SIGN[6]*M_X_MIN) / (M_X_MAX - M_X_MIN) - SENSOR_SIGN[6]*0.5;
    c_magnetom_y = (float)(magnetom_y - SENSOR_SIGN[7]*M_Y_MIN) / (M_Y_MAX - M_Y_MIN) - SENSOR_SIGN[7]*0.5;
    c_magnetom_z = (float)(magnetom_z - SENSOR_SIGN[8]*M_Z_MIN) / (M_Z_MAX - M_Z_MIN) - SENSOR_SIGN[8]*0.5;
    
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
    
    bzero(&DCM_Matrix, sizeof(DCM_Matrix));
//    DCM_Matrix[0] = { 1, 0, 0};
    DCM_Matrix[0][0] = 1;
//    DCM_Matrix[1] = { 0, 1, 0};
    DCM_Matrix[1][1] = 1;
//    DCM_Matrix[2] = { 0, 0, 1};
    DCM_Matrix[2][2] = 1;
    
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
        // NOTE: not so sure why we do this... is it needed??
        for(int i=0;i<32;i++)    // We take some readings...
        {
            readGyro();
            readAccel();
            for(int y=0; y<6; y++)   // Cumulate values
                AN_OFFSET[y] += AN[y];
            delay(20);
        }
        
        for(int y=0; y<6; y++)
            AN_OFFSET[y] = AN_OFFSET[y]/32;
        
        AN_OFFSET[5]-=GRAVITY*SENSOR_SIGN[5];
        
#if DEBUG
        DEBUG_PRINTLN("Offset:");
        for(int y=0; y<6; y++)
            DEBUG_PRINTLN(AN_OFFSET[y]);
#endif

        // init the compass right now; we utilize it!
        readCompass();    // Read I2C magnetometer
        Compass_Heading(); // Calculate magnetic heading
        
        _internalProcess();
#warning corbin i can probably remove the aboe line
    }
    
    return result;
}

bool CDOrientation::initGyro() {
    bool result = _gyro.init();
    if (result) {
        _gyro.writeReg(L3G_CTRL_REG4, 0x20); // 2000 dps full scale
        _gyro.writeReg(L3G_CTRL_REG1, 0x0F); // normal power mode, all axes enabled, 100 Hz
    } else {
        DEBUG_PRINTLN("FAILED TO init gyro");
    }
    return result;
    
}

bool CDOrientation::initAccel() {
    bool result = _compass.init();
    if (result) {
        _compass.enableDefault();
        switch (_compass.getDeviceType())
        {
            case LSM303::device_D:
                _compass.writeReg(LSM303::CTRL2, 0x18); // 8 g full scale: AFS = 011
                break;
            case LSM303::device_DLHC:
                _compass.writeReg(LSM303::CTRL_REG4_A, 0x28); // 8 g full scale: FS = 10; high resolution output mode
                break;
            default: // DLM, DLH
                _compass.writeReg(LSM303::CTRL_REG4_A, 0x30); // 8 g full scale: FS = 11
        }
    } else {
        DEBUG_PRINTLN("FAILED TO INIT accel/compass ");
    }
    return result;
}

void CDOrientation::readGyro()
{
    _gyro.read();
    
    AN[0] = _gyro.g.x;
    AN[1] = _gyro.g.y;
    AN[2] = _gyro.g.z;
    gyro_x = SENSOR_SIGN[0] * (AN[0] - AN_OFFSET[0]);
    gyro_y = SENSOR_SIGN[1] * (AN[1] - AN_OFFSET[1]);
    gyro_z = SENSOR_SIGN[2] * (AN[2] - AN_OFFSET[2]);
}

// Reads x,y and z accelerometer registers
void CDOrientation::readAccel()
{
    _compass.readAcc();
    
    AN[3] = _compass.a.x >> 4; // shift left 4 bits to use 12-bit representation (1 g = 256)
    AN[4] = _compass.a.y >> 4;
    AN[5] = _compass.a.z >> 4;
    accel_x = SENSOR_SIGN[3] * (AN[3] - AN_OFFSET[3]);
    accel_y = SENSOR_SIGN[4] * (AN[4] - AN_OFFSET[4]);
    accel_z = SENSOR_SIGN[5] * (AN[5] - AN_OFFSET[5]);
}


void CDOrientation::readCompass()
{
    _compass.readMag();
    
    magnetom_x = SENSOR_SIGN[6] * _compass.m.x;
    magnetom_y = SENSOR_SIGN[7] * _compass.m.y;
    magnetom_z = SENSOR_SIGN[8] * _compass.m.z;
}


int _test = 0;

void CDOrientation::_internalProcess() {
#warning corbin
    _test++;
    if (_test > 4) {
//        return;
    }
    
//    DEBUG_PRINTLN("++++++++++++++++++++++++++++++");
    counter++;
    timer_old = timer;
    timer = millis();
    if (timer>timer_old) {
        G_Dt = (timer-timer_old)/1000.0;    // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
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
    print();
}

void CDOrientation::process() {
    if((millis()-timer)>=20)  // Main loop runs at 50Hz
    {
        _internalProcess();
    }
}

void CDOrientation::print()
{
    Serial.print("!");
    
#if PRINT_EULER == 1
    Serial.print("ANG:");
    Serial.print(ToDeg(roll));
    Serial.print(",");
    Serial.print(ToDeg(pitch));
    Serial.print(",");
    Serial.print(ToDeg(yaw));
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
    Serial.println();
    
}

