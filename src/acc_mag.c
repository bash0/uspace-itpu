 #include <acc_mag.h>

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>


int facc;
int fmag;


int lsm303_init()
{

    //initialize i2c-device
    facc = open("/dev/i2c-2", O_RDWR | O_NOCTTY | O_NDELAY); //check which i2c
    //    int facc = open("/dev/i2c-2", O_RDWR); //check which i2c
    if (facc == -1)
    {
        printf("open_port: Unable to open i2c-bus");
        return 0; //0 == error
    }

    //fcntl(fgyro, F_SETFL, 0); //what does that??

    if (ioctl(facc, I2C_SLAVE, LSM303_LIN_ACC_ADDR) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        return 0; // 0 == error
    }

    // LSM303 configuration settings
    int8_t LSM303_CTRL_REG1_set = 0b00100111;  // (Atheel recommends: 0b00100111) enable: normal power mode, highest filter cut-off freq, Z-Y-X axes
    int8_t LSM303_CTRL_REG4_set = 0b01000000;  // (Atheel recommends: 0b01000000) enable: normal power mode, highest filter cut-off freq, Z-Y-X axes


    // set LSM303 CTRL REG1 register
    int8_t reg[2];
    reg[0] = LSM303_CTRL_REG1_A;
    reg[1] = LSM303_CTRL_REG1_set;
    if (write(facc, reg, 2) != 2)
    {
        printf("Failed to set REG1\n");
        return 0; //0 == error;
    }

    // set LSM303 CTRL REG4 register
    reg[0] = LSM303_CTRL_REG4_A;
    reg[1] = LSM303_CTRL_REG4_set;
    if (write(facc, reg, 2) != 2)
    {
        printf("Failed to set REG4\n");
        return 0; //0 == error;
    }



    //init magnetometer

    //initialize i2c-device
    fmag = open("/dev/i2c-2", O_RDWR | O_NOCTTY | O_NDELAY); //check which i2c
    //    int fmag = open("/dev/i2c-2", O_RDWR); //check which i2c
    if (fmag == -1)
    {
        printf("open_port: Unable to open i2c-bus");
        return 0; //0 == error
    }
        //fcntl(fgyro, F_SETFL, 0); //what does that??

    if (ioctl(fmag, I2C_SLAVE, LSM303_MAG_SENS_ADDR) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        return 0; // 0 == error
    }

    int8_t LSM303_CRA_REG_set = 0b00010100; // output data rate=333ms, normal measurement configuration
    int8_t LSM303_CRB_REG_set = 0b00100000; // set output gain
    int8_t LSM303_MR_REG_set = 0b00000000; // set magnetometer to continuous conversion mode

    reg[0] = LSM303_CRA_REG_M;
    reg[1] = LSM303_CRA_REG_set;
    if (write(fmag, reg, 2) != 2)
    {
        printf("Failed to set CRA REG\n");
        return 0; //0 == error;
    }


    reg[0] = LSM303_CRB_REG_M;
    reg[1] = LSM303_CRB_REG_set;
    if (write(fmag, reg, 2) != 2)
    {
        printf("Failed to set CRB REG\n");
        return 0; //0 == error;
    }

    reg[0] = LSM303_MR_REG_M;
    reg[1] = LSM303_MR_REG_set;
    if (write(fmag, reg, 2) != 2)
    {
        printf("Failed to set MR REG\n");
        return 0; //0 == error;
    }

    return 1; // success
}
