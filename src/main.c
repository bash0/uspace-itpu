#include <nmea/nmea.h>

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

/*
 *  stup of a main function to display formatted sensor data to test sensor interface
 */
int main()
{    
      
      nmeaINFO info;
      nmeaPARSER parser;
      nmeaPOS dpos;
      FILE *file;
      char buff[2048];
      int size;
      
      file = fopen("gpslog.txt", "rb");
      
       if(!file)
        return -1;
       
      nmea_zero_INFO(&info);
      nmea_parser_init(&parser);
       
      srand ( time(NULL) );
      initscr();	//start ncurses
      time_t startTime;
      time(&startTime);

      while(!feof(file))
      {
	    
	    time_t currentTime;
	    time ( &currentTime );
	    currentTime -= startTime;
	    size = (int)fread(&buff[0], 1, 100, file);
	    nmea_parse(&parser, &buff[0], size, &info);
	    nmea_info2pos(&info, &dpos);
            
	    printw("Current Sensor output:\n");
	    printw("%ld s\n\n", currentTime );
	    printw("Magnetometer:\t X:\t%d\tY:\t%d\tZ:\t%d\n", rand() % 100, rand() % 100, rand() % 100);
	    printw("Accelerometer:\t X:\t%d\tY:\t%d\tZ:\t%d\n", rand() % 100, rand() % 100, rand() % 100);
	    printw("Gyroscope:\t X:\t%d\tY:\t%d\tZ:\t%d\n", rand() % 100, rand() % 100, rand() % 100);
	    printw("GPS:\t\t Long.:\t%f\tLat.:\t%f\tAlt.:\t%f\n", dpos.lat, dpos.lon, info.elv);
	    refresh(); //update ncurses window
	    usleep(500*1000);	    
	    erase();
      }
      endwin();		//end ncurses
      nmea_parser_destroy(&parser);
      fclose(file);
           
      
      return 0;
}
      
      