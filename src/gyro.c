#include <gyro.h>

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

float gyro_xyz[3];

int fgyro;

// averaged offset values in rad/sec
float xyz_gyro_offset[3] = {0.0, 0.0, 0.0};

int itg3200_init()
{
    //initialize a serial port to GPS antenna
    fgyro = open("/dev/i2c-2", O_RDWR | O_NOCTTY | O_NDELAY); //check which i2c
//    int fgyro = open("/dev/i2c-2", O_RDWR); //check which i2c
    if (fgyro == -1)
    {
        printf("open_port: Unable to open i2c-bus");
        return 0; //0 == error
    }
    else
    {
        //fcntl(fgyro, F_SETFL, 0); //what does that??

        if (ioctl(fgyro, I2C_SLAVE, ITG3200_I2C_ADDRESS) < 0)
        {
            printf("Failed to acquire bus access and/or talk to slave.\n");
            return 0; // 0 == error
        }

        //try to get the Who Am I register of the ITG3200

        //first inform itg which register is to be read
        int8_t whoami = ITG3200_WHO_AM_I;
        if (write(fgyro, &whoami, 1) != 1)
        {
            printf("Failed to write to gyro\n");
            return 0; //0 == error;
        }

        //second read register
        if (read(fgyro, &whoami, 1) != 1)
        {
            printf("Failed to write to gyro\n");
            return 0; //0 == error
        }
        printf("Got whoami register: %x\n", whoami);
        return 1;  //1 == success
    }
}

int itg3200_read_raw(int xyz_raw[3])
{
    int8_t buffer[6];
    if(read(fgyro, buffer, 6) != 6)
    {
        printf("Read gyro xyz failed\n");
        return 0;
    }

    xyz_raw[0] = buffer[0]<<8 | buffer[1];
    xyz_raw[1] = buffer[2]<<8 | buffer[3];
    xyz_raw[2] = buffer[4]<<8 | buffer[5];

    return 1;
}

int itg3200_read_rad(float xyz_rad[3])
{
        int status = 0;
        // the factor 1.0 / 8 is empirical and lacks explanation
        // experimentally this was still inaccurate to a few %
        static float factor = 1.0 / (8.0  * GYRO_CONVERSION_FACTOR) * DEG_TO_RAD;
        int xyz_raw[3];

        status = itg3200_read_raw(xyz_raw);

        xyz_rad[0] = xyz_raw[0] * factor;
        xyz_rad[1] = xyz_raw[1] * factor;
        xyz_rad[2] = xyz_raw[2] * factor;

        return status;
}

int itg3200_read_deg(float xyz_deg[3])
{
        int status = 0;

        static float factor = 0.125 / GYRO_CONVERSION_FACTOR; // convert to unit to [deg/sec]
        int xyz_raw[3];

        status = itg3200_read_raw(xyz_raw);

        xyz_deg[0] = xyz_raw[0] * factor;
        xyz_deg[1] = xyz_raw[1] * factor;
        xyz_deg[2] = xyz_raw[2] * factor;

        return status;
}

int itg3200_read_rad_calibrated(float xyz_rad_cal[3])
{
        int status;
        float xyz_rad[3];
        status = itg3200_read_rad(xyz_rad);

        xyz_rad_cal[0] = xyz_rad[0] - xyz_gyro_offset[0];
        xyz_rad_cal[1] = xyz_rad[1] - xyz_gyro_offset[1];
        xyz_rad_cal[2] = xyz_rad[2] - xyz_gyro_offset[2];
        return status;
}

int itg3200_read_deg_calibrated(float xyz_deg_cal[3])
{
        int status;
        float xyz_deg[3];
        status = itg3200_read_deg(xyz_deg);

        xyz_deg_cal[0] = xyz_deg[0] - xyz_gyro_offset[0] * RAD_TO_DEG;
        xyz_deg_cal[1] = xyz_deg[1] - xyz_gyro_offset[1] * RAD_TO_DEG;
        xyz_deg_cal[2] = xyz_deg[2] - xyz_gyro_offset[2] * RAD_TO_DEG;
        return status;
}

int itg3200_calibrate_gyro()
{
        int status = 0;
        int n = 100;
        int i;
        float xyz[3];
        float x_sum, y_sum, z_sum;

        x_sum = 0.0;
        y_sum = 0.0;
        z_sum = 0.0;

        for (i = 0; i < n; i++)
        {
                status += itg3200_read_rad(xyz);

                x_sum += xyz[0];
                y_sum += xyz[1];
                z_sum += xyz[2];
        }

        xyz_gyro_offset[0] = x_sum / n;
        xyz_gyro_offset[1] = y_sum / n;
        xyz_gyro_offset[2] = z_sum / n;

//        if (debug_mode_flag)
//        {
//                float rtg = RAD_TO_DEG;
//                sprintf(debug_msg_buffer,
//                                "Gyro calibration values: X,Y,Z = %.2f : %.2f : %.2f deg/s\n",
//                                xyz_gyro_offset[0] * rtg,
//                                xyz_gyro_offset[1] * rtg,
//                                xyz_gyro_offset[2] * rtg);
//                usart_write_line(USART_DEBUG, debug_msg_buffer);
//        }

        return status;
}
