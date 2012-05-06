#include <threads.h>
#include <nmea/nmea.h>
#include <gyro.h>

#include <unistd.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <ncurses.h>
#include <math.h>

extern nmeaINFO gpsInfo;
extern nmeaPARSER parser;



struct periodic_info
{
        int timer_fd;
        unsigned long long wakeups_missed;
};

static int make_periodic (unsigned int period, struct periodic_info *info);
static void wait_period (struct periodic_info *info);


void * thread_DisplayValues()
{
    struct periodic_info info;
    make_periodic (500000, &info);
    nmeaPOS dpos;
    srand ( time(NULL) );
    time_t startTime;
    time(&startTime);

    while(1)
    {
        time_t currentTime;
        time ( &currentTime );
        currentTime -= startTime;

        nmea_info2pos(&gpsInfo, &dpos);
        printw("Current Sensor output:\n");
        printw("%ld s\n\n", currentTime );
        printw("Magnetometer:\t X:\t%d\tY:\t%d\tZ:\t%d\n", rand() % 100, rand() % 100, rand() % 100);
        printw("Accelerometer:\t X:\t%d\tY:\t%d\tZ:\t%d\n", rand() % 100, rand() % 100, rand() % 100);
        printw("Gyroscope:\t X:\t%d\tY:\t%d\tZ:\t%d\n", rand() % 100, rand() % 100, rand() % 100);
        printw("GPS:\t\t Long.:\t%f\tLat.:\t%f\tAlt.:\t%f\n", 180*dpos.lat/M_PI, 180*dpos.lon/M_PI, gpsInfo.elv);
        refresh(); //update ncurses window
        erase();
        wait_period(&info);
    }
}

void * thread_SensorFusion()
{

    struct periodic_info info;

    if (init_gyro())
    {
        make_periodic (500000, &info);
        char buffer[512];
        while(1)
        {
           //do nothing
           wait_period(&info);
        }
    }
    else
    {
        perror("i2c-init failed\n");
        exit(1);
    }
}

void * thread_getGPS()
{

    /*
      maybe put the functionality of the serial line in an own function within an own module?
      */
    struct periodic_info info;

    //initialize a serial port to GPS antenna
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
        perror("open_port: Unable to open /dev/ttyS0 -\n");
    else
    {
        fcntl(fd, F_SETFL, 0); //what does that??

        make_periodic (500000, &info);
        char buffer[512];
        while(1)
        {
            char c;
            int index = 0;
            while(read(fd, &c, 1) == 1 )
            {
                buffer[index] = c;
                index++;
                if (c == '\n')
                {
                    nmea_parse(&parser, &buffer[0], index, &gpsInfo);
                    index = 0;
                }
            }
            wait_period(&info);
        }
    }
}

static int make_periodic (unsigned int period, struct periodic_info *info)
{
        int ret;
        unsigned int ns;
        unsigned int sec;
        int fd;
        struct itimerspec itval;

        /* Create the timer */
        fd = timerfd_create (CLOCK_MONOTONIC, 0);
        info->wakeups_missed = 0;
        info->timer_fd = fd;
        if (fd == -1)
                return fd;

        /* Make the timer periodic */
        sec = period/1000000;
        ns = (period - (sec * 1000000)) * 1000;
        itval.it_interval.tv_sec = sec;
        itval.it_interval.tv_nsec = ns;
        itval.it_value.tv_sec = sec;
        itval.it_value.tv_nsec = ns;
        ret = timerfd_settime (fd, 0, &itval, NULL);
        return ret;
}

static void wait_period (struct periodic_info *info)
{
        unsigned long long missed;
        int ret;

        /* Wait for the next timer event. If we have missed any the
           number is written to "missed" */
        ret = read (info->timer_fd, &missed, sizeof (missed));
        if (ret == -1)
        {
                perror ("read timer");
                return;
        }

        info->wakeups_missed += missed;
}
