#include <nmea/nmea.h>
#include <threads.h>

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>

nmeaINFO gpsInfo;
nmeaPARSER parser;
float gyro_xyz[3];
float mag_xyz[3];
float acc_xyz[3];

/*
 *  stup of a main function to display formatted sensor data to test sensor interface
 */
int main()
{              

    //Scheduling has to be changed to RT-scheduling


      nmea_zero_INFO(&gpsInfo);
      nmea_parser_init(&parser);
       
      initscr();	//start ncurses

      pthread_t t_1;
      pthread_t t_2;

      pthread_create (&t_1, NULL, thread_getGPS, NULL) ;
      pthread_create (&t_2, NULL, thread_DisplayValues, NULL) ;

       pthread_join (t_1, NULL);
       pthread_join (t_2, NULL);


      endwin();		//end ncurses
      nmea_parser_destroy(&parser);      
      return 0;
}
      
      
