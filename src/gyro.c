#include <gyro.h>

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
// #include <stdlib.h>

int init_gyro()
{
    //initialize a serial port to GPS antenna
    int fgyro = open("/dev/i2c-2", O_RDWR | O_NOCTTY | O_NDELAY); //check which i2c
    if (fgyro == -1)
    {
        perror("open_port: Unable to open i2c-bus");
        return 0; //0 == error
    }
    else
    {
        //fcntl(fgyro, F_SETFL, 0); //what does that??

        if (ioctl(fgyro, I2C_SLAVE, ITG3200_I2C_ADDRESS) < 0)
        {
            perror("Failed to acquire bus access and/or talk to slave.\n");
            return 0; // 0 == error
        }

        //try to get the Who Am I register of the ITG3200

        //first inform itg which register is to be read
        char whoami = ITG3200_WHO_AM_I;
        if (write(fgyro, &whoami, 1) != 1)
        {
            perror("Failed to write to gyro\n");
            return 0; //0 == error;
        }

        //second read register
        if (read(fgyro, &whoami, 1) != 1)
        {
            perror("Failed to write to gyro\n");
            return 0; //0 == error
        }
        printf("Got whoami register: %x\n", whoami);
        return 1;  //1 == success
    }
}
