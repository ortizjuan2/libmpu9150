#ifndef MPU9150_H
#define MPU9150_H


#ifdef __cplusplus
extern "C"
{
#endif

#define PWR_MGMT_1          0x6b
#define CLKSEL_PLL_X_REF    0x01

#define GYRO_CONFIG         0x1b
#define INV_FS_SEL_250      0x18
#define INV_FS_SEL_500      0x10
#define INV_FS_SEL_1000     0x08
#define INV_FS_SEL_2000     0x00

#define ACCEL_CONFIG        0x1c
#define INV_AFS_SEL_2G      0x18
#define INV_AFS_SEL_4G      0x10
#define INV_AFS_SEL_8G      0x08
#define INV_AFS_SEL_16G     0x00
    
#define CONFIG              0x1b
#define INV_DLP_CFG_0       0x07
#define INV_DLP_CFG_1       0x06
#define INV_DLP_CFG_2       0x05
#define INV_DLP_CFG_3       0x04
#define INV_DLP_CFG_4       0x03
#define INV_DLP_CFG_5       0x02
#define INV_DLP_CFG_6       0x01
#define INV_DLP_CFG_7       0x00

#define FIFO_EN             0x23
#define TEMP_FIFO_EN        0x80
#define XG_FIFO_EN          0x40
#define YG_FIFO_EN          0x20
#define ZG_FIFO_EN          0x10
#define ACCEL_FIFO_EN       0x08

#define INT_PIN_CFG         0x37
#define I2C_BYPASS_EN       0x02
#define USER_CTRL           0x6a
#define I2C_MST_EN          0x20

// magnetometer
#define MAG_CNTL            0x0a
#define MAG_MODE_SINGLE     0x01
#define MAG_MODE_FUSE_ROM   0x0f
#define MAG_ASAX            0x10
#define MAG_ASAY            0x11
#define MAG_ASAZ            0x12


// Measurements
#define ACCEL_XOUT_H        0x3b
#define ACCEL_XOUT_L        0x3c
#define ACCEL_YOUT_H        0x3d
#define ACCEL_YOUT_L        0x3e
#define ACCEL_ZOUT_H        0x3F
#define ACCEL_ZOUT_L        0x40
//
#define GYRO_XOUT_H         0x43
#define GYRO_XOUT_L         0x44
#define GYRO_YOUT_H         0x45
#define GYRO_YOUT_L         0x46
#define GYRO_ZOUT_H         0x47
#define GYRO_ZOUT_L         0x48
//
#define COMPASS_XOUT_L      0x03
#define COMPASS_XOUT_H      0x04
#define COMPASS_YOUT_L      0x05
#define COMPASS_YOUT_H      0x06
#define COMPASS_ZOUT_L      0x07
#define COMPASS_ZOUT_H      0x08



class MPU9150{
public:
    
    double accel[3];
    double gyro[3];
    double compass[3];
    double gyro_offset[3];
    double accel_offset[3];
    double sensvalue[3];
    int32_t fd, fdm;
    
    MPU9150();
    ~MPU9150();
    int init_sensor(int address);
    void read_accel();
    void read_gyro();
    void read_compass();

};



#ifdef __cplusplus
}
#endif
#endif // MPU9150_H