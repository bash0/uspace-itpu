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

const float magnetometer_gain_gauss[] = {1050.0, 1050.0, 950.0};
const float magnetometer_gain_tesla[] = {10500000.0, 10500000.0, 9500000.0};
const float accelerometer_gain_SI = 1000.0;

// Sensor variables
float xyz_mag_offset[3] = {0}; // magnetometer offset values

// the values here are from sample measurements. Not very precise but better than uncalibrated.
float xyz_mag_offset_gauss[3] = {-0.29, 0.02, -0.21}; // magnetometer offset values
float xyz_acc_offset[3] = {0}; // magnetometer offset values



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

// saves the magnetometer measurements in an input array of size three, (x,y,z) values
// caution!! direct typecast to float my cause problems!!
int lsm303_read_mag_measurements(float xyz[3])
{
    int8_t readReg = LSM303_OUT_X_H_M;

    if (write(fmag, &readReg, 1) != 1)
    {
        printf("Failed set read register\n");
        return 0; //0 == error;
    }

    int8_t buffer[6];
    if(read(fmag, buffer, 6) != 6)
    {
        printf("Read magnetometer xyz failed\n");
        return 0;
    }

    xyz[0] = (float) ((buffer[0]<<8 | buffer[1])-xyz_mag_offset[0]);
    xyz[1] = (float) ((buffer[2]<<8 | buffer[3])-xyz_mag_offset[1]);
    xyz[2] = (float) ((buffer[4]<<8 | buffer[5])-xyz_mag_offset[2]);

    return 1;
}

int lsm303_read_magnet_raw(int16_t xyz[3])
{


    int8_t readReg = LSM303_OUT_X_H_M;

    if (write(fmag, &readReg, 1) != 1)
    {
        printf("Failed set read register\n");
        return 0; //0 == error;
    }

    int16_t buffer[6];
    if(read(fmag, buffer, 6) != 6)
    {
        printf("Read magnetometer xyz failed\n");
        return 0;
    }

    xyz[0] = (int16_t)((buffer[0]<<8 | buffer[1]));
    xyz[1] = (int16_t)((buffer[2]<<8 | buffer[3]));
    xyz[2] = (int16_t)((buffer[4]<<8 | buffer[5]));

    return 1;
}

int lsm303_read_magnet_gauss(float xyz_gauss[3])
{
    int status;
    int16_t xyz_raw[3];

    status = lsm303_read_magnet_raw(xyz_raw);

    xyz_gauss[0] = (float)xyz_raw[0] / magnetometer_gain_gauss[0];
    xyz_gauss[1] = (float)xyz_raw[1] / magnetometer_gain_gauss[1];
    xyz_gauss[2] = (float)xyz_raw[2] / magnetometer_gain_gauss[2];

    return status;
}


int lsm303_read_magnet_gauss_cal(float xyz[3])
{
    int status;
    float xyz_gauss[3];

    status = lsm303_read_magnet_gauss(xyz_gauss);

    xyz[0] = xyz_gauss[0] - xyz_mag_offset_gauss[0];
    xyz[1] = xyz_gauss[1] - xyz_mag_offset_gauss[1];
    xyz[2] = xyz_gauss[2] - xyz_mag_offset_gauss[2];

    return status;
}


int lsm303_read_magnet_tesla(float xyz_tesla[3])
{
    int status;
    int16_t xyz_raw[3];

    status = lsm303_read_magnet_raw(xyz_raw);

    xyz_tesla[0] = (float)xyz_raw[0] / magnetometer_gain_tesla[0];
    xyz_tesla[1] = (float)xyz_raw[1] / magnetometer_gain_tesla[1];
    xyz_tesla[2] = (float)xyz_raw[2] / magnetometer_gain_tesla[2];

    return status;
}


// reads and returns the rotation angle around the z-axis (xy-plane)
float lsm303_magnet_angle_rad()
{
    // fetch data from sensor
    float xyz[3];

    lsm303_read_magnet_gauss_cal(xyz);


    // measure counterclockwise from north
    return atan2(-xyz[1], xyz[0]);
}


// saves the accelerometer measurements in an input array of size three, (x,y,z) values
int lsm303_read_acc_measurements(float xyz[3])
{
    int8_t readReg = LSM303_OUT_X_L_A | LSM303_SUB_ADDR_multiple_read;

    if (write(facc, &readReg, 1) != 1)
    {
        printf("Failed set read register\n");
        return 0; //0 == error;
    }

    int8_t buffer[6];
    if(read(facc, buffer, 6) != 6)
    {
        printf("Read accelerometer xyz failed\n");
        return 0;
    }

    xyz[0] = (float) ((buffer[0]<<8 | buffer[1])-xyz_acc_offset[0]);
    xyz[1] = (float) ((buffer[2]<<8 | buffer[3])-xyz_acc_offset[1]);
    xyz[2] = (float) ((buffer[4]<<8 | buffer[5])-xyz_acc_offset[2]);

    return 1;
}

int lsm303_read_acc_raw(int16_t xyz[3])
{
    int8_t readReg = LSM303_OUT_X_L_A | LSM303_SUB_ADDR_multiple_read;

    if (write(facc, &readReg, 1) != 1)
    {
        printf("Failed set read register\n");
        return 0; //0 == error;
    }

    int8_t buffer[6];
    if(read(facc, buffer, 6) != 6)
    {
        printf("Read accelerometer xyz failed\n");
        return 0;
    }

    xyz[0] = (int16_t) ((buffer[0]<<8 | buffer[1]));
    xyz[1] = (int16_t) ((buffer[2]<<8 | buffer[3]));
    xyz[2] = (int16_t) ((buffer[4]<<8 | buffer[5]));

    return 1;
}

int lsm303_read_acc_SI(float xyz[3])
{
    int status;
    int16_t xyz_raw[3];

    status = lsm303_read_acc_raw(xyz_raw);

    xyz[0] = (float)xyz_raw[0] / accelerometer_gain_SI;
    xyz[1] = (float)xyz_raw[1] / accelerometer_gain_SI;
    xyz[2] = (float)xyz_raw[2] / accelerometer_gain_SI;

    return status;
}

/* Returns the attitude angle of the satellite with respect
 * to North, based on a magnetometer measurment.
 * Output range: 360 deg.
 */
float lsm303_mag_convert_to_deg_2D(int16_t xy[])
{
    float angle_rad, angle_deg;

    angle_rad = atan2(xy[1], xy[0]);

    angle_deg = angle_rad*360/(2*M_PI);

    angle_deg = fmod(angle_deg + 180.0, 360.0) - 180.0;

    return(angle_deg);
}


// Calibrates the accelerometer for offset error
// z-axis must point straight up!
int lsm303_calibrate_acc()
{
    int status = 0;
//    char ADCS_buffer[50]={0};
    float xyz[3];
    float x_avg, y_avg, z_avg;
    int i,n;

    n = 100; // do 100 measurements for calibration

    //sensor range is +/- 2g -->  +/-32768 measurement values
    int16_t x_pre_def = 0;		// should read zero g
    int16_t y_pre_def = 0;		// should read zero g
    int16_t z_pre_def = 16384;	// should read 1g = 32768/2 = 16384, when using 2g sensor range

    for(i=0; i<n; i++)
    {
        status += lsm303_read_acc_measurements(xyz);

        x_avg += xyz[0];
        y_avg += xyz[1];
        z_avg += xyz[2];
    }

    // calculate averages
    x_avg /= n;
    y_avg /= n;
    z_avg /= n;

    // calculate offset values
    xyz_acc_offset[0] = x_avg-x_pre_def;
    xyz_acc_offset[1] = y_avg-y_pre_def;
    xyz_acc_offset[2] = z_avg-z_pre_def;

    //        if (debug_mode_flag)
    //        {
    //                sprintf(ADCS_buffer, "Accelerometer calibration values: X,Y,Z= %f : %f : %f\n", xyz_acc_offset[0], xyz_acc_offset[1], xyz_acc_offset[2]);
    //                usart_write_line(USART_0, ADCS_buffer);
    //        }

    return status;
}


/* Calibrates the magnetometer, w.r.t. NOAA values of Earths
 * magnetic field. NB. magnetometer must be placed such that:
 * X points exactly to South
 * Y points exactly to East
 * Z points UP
 */
int lsm303_calibrate_mag()
{
    int status = 0;
//    char ADCS_buffer[50] = {0};
    float xyz[3];
    float x_avg, y_avg, z_avg;
    int i, n;

    n = 100; // do 100 measurements for calibration

    //predefined values actual magnetic field values.
    //units in gauss times the sensor gain: 1050 for x,y and 950 for z
    int16_t x_pre_def = -190;
    int16_t y_pre_def = -5;
    int16_t z_pre_def = -420;

    for (i = 0; i < n; i++)
    {
        status += lsm303_read_mag_measurements(xyz);
        x_avg += xyz[0];
        y_avg += xyz[1];
        z_avg += xyz[2];
    }

    // calculate averages
    x_avg /= n;
    y_avg /= n;
    z_avg /= n;

    // calculate offset values
    xyz_mag_offset[0] = x_pre_def-x_avg;
    xyz_mag_offset[1] = y_pre_def-y_avg;
    xyz_mag_offset[2] = z_pre_def-z_avg;

//    if (debug_mode_flag)
//    {
//        sprintf(ADCS_buffer, "Magnetometer calibration values:  X,Y,Z= %f : %f : %f\n", xyz_mag_offset[0], xyz_mag_offset[1], xyz_mag_offset[2]);
//        usart_write_line(USART_DEBUG, ADCS_buffer);
//    }

    return status;
}
