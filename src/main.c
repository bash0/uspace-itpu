#include <nmea/nmea.h>
#include <threads.h>
#include <globals.h>

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

nmeaINFO gpsInfo;
nmeaPARSER parser;
float gyro_xyz[3];
float mag_xyz[3];
float acc_xyz[3];

int useGyro = TRUE;
int useAcc  = TRUE;
int useMag  = TRUE;
int useGPS  = TRUE;

/*
 *  stup of a main function to display formatted sensor data to test sensor interface
 */
int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        int i;
        for (i = 0; i< argc; i++)
            if (argv[i][0] == '-')
            {
                useGyro = FALSE;
                useMag  = FALSE;
                useAcc  = FALSE;
                useGPS  = FALSE;
                int j;
                for (j = 1; j< strlen(argv[i]); j++)
                    switch (argv[i][j])
                    {
                    case 'm':
                    case 'M': useMag = TRUE; break;
                    case 'a':
                    case 'A': useAcc = TRUE; break;
                    case 'G':
                    case 'g': useGyro = TRUE; break;
                    case 'p':
                    case 'P': useGPS = TRUE; break;
                    default: printf("Usage:\n");
                        printf("If no parameter is given all sensors are use.\n");
                        printf("Only use certain sensors with:\n");
                        printf("\t-m/-M:\t include magnetometer\n");
                        printf("\t-a/-A:\t include accelerometer\n");
                        printf("\t-g/-G:\t include gyroscope\n");
                        printf("\t-p/-P:\t include GPS position\n\n");
                        return 1;
                    }
            }
    }

    //Scheduling has to be changed to RT-scheduling


    initscr();	//start ncurses

      pthread_t t_1;
      pthread_t t_2;
      pthread_t t_3;

      if(useGPS)
      {
        nmea_zero_INFO(&gpsInfo);
        nmea_parser_init(&parser);
        pthread_create (&t_1, NULL, thread_getGPS, NULL) ;
        pthread_join (t_1, NULL);
      }
      pthread_create (&t_3, NULL, thread_SensorFusion, NULL) ;
      pthread_create (&t_2, NULL, thread_DisplayValues, NULL) ;

       pthread_join (t_2, NULL);
       pthread_join (t_3, NULL);

      endwin();		//end ncurses
//      nmea_parser_destroy(&parser);
      return 0;
}
      
      
