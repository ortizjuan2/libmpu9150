/*
   $License:
   Copyright (C) 2018 RiderLed Ltd, All Rights Reserved.
   See included License.txt for License information.
   $
   */
/**
 *  @addtogroup  DRIVERS Sensor Driver Layer
 *  @brief       Hardware drivers to communicate with sensors via I2C.
 *
 *  @{
 *      @file       mpu9150.cpp
 *      @brief      An I2C-based driver for Invensense MPU-9150 Accelerometer.
 *      @details    This driver currently works for MPU9150 with AK8975 on 
 *                  the auxiliary bus.
 * 
 */

#include <iostream>
#include <errno.h>
#include <wiringPiI2C.h>
#include <time.h>
#include <vector>
#include <cmath>
#include <sstream>
#include <fstream>
#include "mpu9150.h"

using namespace std;

/*
 * Accelerometer Sensitivity
 */
const double ACCEL_SENS = 16384.0;

/*
 * Gyro Sensitivity
 */
const double GYRO_SENS = 131.0;

/*
 * Default constructor, destructor
 */
MPU9150::MPU9150(){
    /*
     * load stored calibration values if found
     * gyrocal.txt :    gyro offset values
     * accelcal.txt:    accelerometer offset values
     */ 
    fstream fgyroin("./gyrocal.txt", ios::in);
    if(fgyroin.is_open()){
        fgyroin >> gyro_offset[0] >> gyro_offset[1] >> gyro_offset[2];
        fgyroin.close();
        cout << "Gyro offset data loaded successfully!." << endl;
        cout << gyro_offset[0] << " " << gyro_offset[1] << " " << gyro_offset[2] << endl;

    }else{
        gyro_offset[0] = 0.0;
        gyro_offset[1] = 0.0;
        gyro_offset[2] = 0.0;
    }

    fstream accelin("./accelcal.txt", ios::in);
    if(accelin.is_open()){
        accelin >> accel_offset[0] >> accel_offset[1] >> accel_offset[2];
        accelin.close();
        cout << "Accelerometer offset data loaded successfully!." << endl;
        cout << accel_offset[0] << " " << accel_offset[1] << " " << accel_offset[2] << endl;

    }else{
        accel_offset[0] = 0.0;
        accel_offset[1] = 0.0;
        accel_offset[2] = 0.0;
    }

};
MPU9150::~MPU9150(){};


/**
 *  @brief      Initialize hardware.
 *  Initial configuration:\n
 *  Gyro FSR: +/- 250DPS\n
 *  Accel FSR +/- 2G\n
 *  DLPF: 42Hz\n
 *  FIFO rate: 50Hz\n
 *  Clock source: Gyro PLL\n
 *  FIFO: Disabled.\n
 *  @param[in]  int_param   Platform-specific parameters to interrupt API.
 *  @return     0 if successful.
 *  @return     fd, fdm addresses if successful
 */
int MPU9150::init_sensor(int address){
    int8_t asax, asay, asaz;
    int milisec = 10;
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = milisec * 1000000L;
    int8_t regdata;
    /* setup wiringPI */
    fd = wiringPiI2CSetup(address);
    if(fd == -1){
        return -1;
    }

    /*
     * Initialize sensor using PLL with X axis gyroscope reference
     * on bits [2:0] of PWR_MGMT_1 Register
     */
    wiringPiI2CWriteReg8(fd, PWR_MGMT_1, CLKSEL_PLL_X_REF);
    /*
     * Configure the gyroscopes’ full scale range
     * on bit [4:3] of GYRO_CONFIG Register
     */
    regdata = wiringPiI2CReadReg8(fd, GYRO_CONFIG);
    regdata &= ~INV_FS_SEL_250;
    wiringPiI2CWriteReg8(fd, GYRO_CONFIG, regdata);
    /*
     * Configure the accelerometer's full scale range
     * on bit [4:3] of ACCEL_CONFIG Register
     */
    regdata = wiringPiI2CReadReg8(fd, ACCEL_CONFIG);
    regdata &= ~INV_AFS_SEL_2G;
    wiringPiI2CWriteReg8(fd, ACCEL_CONFIG, regdata);
    /*
     * Digital Low Pass Filter (DLPF) setting for both the gyroscopes and accelerometers. 
     * on bit [2:0] of CONFIG Register
     */
    regdata = wiringPiI2CReadReg8(fd, CONFIG);
    regdata &= ~INV_DLP_CFG_3; // Gyro: 42Hz, Accel: 44Hz
    wiringPiI2CWriteReg8(fd, CONFIG, regdata);
    /*
     * Disable FIFO. 
     */
    wiringPiI2CWriteReg8(fd, FIFO_EN, 0x00);
    /*
     * Configure I2C Access to Magnetometer.
     * When this bit is equal to 1 and I2C_MST_EN (Register 106 bit[5]) is equal to
     * 0, the host application processor will be able to directly access the auxiliary
     * I2C bus of the MPU-9150.
     */
    regdata = wiringPiI2CReadReg8(fd, INT_PIN_CFG);
    regdata |= I2C_BYPASS_EN;
    wiringPiI2CWriteReg8(fd, INT_PIN_CFG, regdata);
    regdata = wiringPiI2CReadReg8(fd, USER_CTRL);
    regdata &= ~I2C_MST_EN; // Clear bit 5
    wiringPiI2CWriteReg8(fd, USER_CTRL, regdata);
    /*
     * get file descriptor for magnetometer
     */
    fdm = wiringPiI2CSetup(0x0c);
    if(fdm == -1){
        return -1;
    }
    /*
     * set Fuse ROM to get access to adjustment data
     */ 
    wiringPiI2CWriteReg8(fdm, MAG_CNTL, MAG_MODE_FUSE_ROM);
    // wait 10 ms
    nanosleep(&req, (struct timespec *)NULL);

    /*
     * get Magnetic sensor sensitivity adjustment values
     */ 
    asax = wiringPiI2CReadReg8(fdm, MAG_ASAX);
    sensvalue[0] = (((asax - 128.0) * 0.5)/128.0) + 1.0;
    asay = wiringPiI2CReadReg8(fdm, MAG_ASAY);
    sensvalue[1] = (((asay - 128.0) * 0.5)/128.0) + 1.0;
    asaz = wiringPiI2CReadReg8(fdm, MAG_ASAZ);
    sensvalue[2] = (((asaz - 128.0) * 0.5)/128.0) + 1.0;
    cout << "Compass Sens values: "
        << sensvalue[0] << " "
        << sensvalue[1] << " "
        << sensvalue[2] << endl;

    /*
     * set single measurement mode for magnetometer
     */ 
    wiringPiI2CWriteReg8(fdm, MAG_CNTL, MAG_MODE_SINGLE);

    return 0;
}


/**
 *  @brief      Read Accelerometer data.
 *  @param[in]  none.
 *  @return     load accel data in accel vector
 */
void MPU9150::read_accel(){
    int32_t result;
    //double dresult;
    // read accelerometer Measurements 0x3b to 0x40
    //wiringPiI2CWrite(fd, 0x3b);
    result = wiringPiI2CReadReg8(fd, ACCEL_XOUT_H);
    result <<= 8;
    result |= wiringPiI2CReadReg8(fd, ACCEL_XOUT_L);
    if (result > 32767)
        result -= 65536;
    accel[0] = (double) (result / ACCEL_SENS) ;
    //accel[0] = dresult;
    // cout << "Accel Xout: " << dec <<  dresult  <<  endl;
    result = wiringPiI2CReadReg8(fd, ACCEL_YOUT_H);
    result <<= 8;
    result |= wiringPiI2CReadReg8(fd, ACCEL_YOUT_L);
    if (result > 32767)
        result -= 65536;
    accel[1] = (double)(result / ACCEL_SENS) ;
    //accel[1] = dresult;
    // cout << "Accel Yout: " << dec << dresult << endl;
    result = wiringPiI2CReadReg8(fd, ACCEL_ZOUT_H);
    result <<= 8;
    result |= wiringPiI2CReadReg8(fd, ACCEL_ZOUT_L);
    if (result > 32767)
        result -= 65536;
    accel[2] = (double) (result / ACCEL_SENS) ;
    // accel[2] = dresult;
    // cout << "Accel Zout: " << dec << dresult << endl;
}


/**
 *  @brief      Read Gyro data.
 *  @param[in]  none.
 *  @return     load Gyro data in gyro vector
 */
void MPU9150::read_gyro(){
    int32_t result;
    // double dresult;
    // read gyro Measurements 0x43 to 0x48
    //wiringPiI2CWrite(fd, 0x43);
    result = wiringPiI2CReadReg8(fd, GYRO_XOUT_H);
    result <<= 8;
    result |= wiringPiI2CReadReg8(fd, GYRO_XOUT_L);
    if (result > 32767)
        result -= 65536;
    gyro[0] = (double) result / GYRO_SENS;
    // gyro[0] = dresult - gyro_offset[0];
    // cout << "Gyro Xout: " << dec << dresult << endl;
    result = wiringPiI2CReadReg8(fd, GYRO_YOUT_H);
    result <<= 8;
    result |= wiringPiI2CReadReg8(fd, GYRO_YOUT_L);
    if (result > 32767)
        result -= 65536;
    gyro[1] = (double) result / GYRO_SENS;
    // gyro[1] = dresult - gyro_offset[1];
    // cout << "Gyro Yout: " << dec << dresult << endl;
    result = wiringPiI2CReadReg8(fd, GYRO_ZOUT_H);
    result <<= 8;
    result |= wiringPiI2CReadReg8(fd, GYRO_ZOUT_L);
    if (result > 32767)
        result -= 65536;
    gyro[2] = (double) result / GYRO_SENS;
    // gyro[2] = dresult - gyro_offset[2];
    // cout << "Gyro Zout: " << dec << dresult << endl;
}



/**
 *  @brief      Read Compass data.
 *  @param[in]  none.
 *  @return     loaded compass data in compass vector
 */
void MPU9150::read_compass(){
    int32_t result;
    double fresult;
    int milisec = 10;
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = milisec * 1000000L;
    /*
     * set single measurement mode for magnetometer
     */ 
    wiringPiI2CWriteReg8(fdm, MAG_CNTL, MAG_MODE_SINGLE);
    // wait 10 ms
    nanosleep(&req, (struct timespec *)NULL);
    // read magnetometer  Measurements directly from AK8975
    // the order Low then High
    //wiringPiI2CWrite(fdm, 0x03);
    int32_t result_l = wiringPiI2CReadReg8(fdm, COMPASS_XOUT_L);
    int32_t result_h =  wiringPiI2CReadReg8(fdm, COMPASS_XOUT_H);
    result_h &= 0x1F;
    result = (result_l | (result_h<<8));
    if (result > 4095)
        result -= 8192;
    // result = result * ((asax - 128)*0.5/128.0 + 1);
    compass[0] = (double) result * sensvalue[0]; // apply sensitivity value
    result_l = wiringPiI2CReadReg8(fdm, COMPASS_YOUT_L);
    result_h = wiringPiI2CReadReg8(fdm, COMPASS_YOUT_H);
    result_h &= 0x1F;
    result = (result_l | (result_h<<8));
    if (result > 4095)
        result -= 8192;
    compass[1] = (double ) result * sensvalue[1];
    result_l = wiringPiI2CReadReg8(fdm, COMPASS_ZOUT_L);
    result_h = wiringPiI2CReadReg8(fdm, COMPASS_ZOUT_H);
    result_h &= 0x1F;
    result = (result_l | (result_h<<8));
    if (result > 4095)
        result -= 8192;
    // result = result * ((asaz - 128)*0.5/128.0 + 1);
    compass[2] = (double) result * sensvalue[2];
    // cout << "-Magnet Zout: " << dec << result << endl;


}
