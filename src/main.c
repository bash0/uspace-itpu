
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


/*
 *  stup of a main function to display formatted sensor data to test sensor interface
 */
int main()
{    
      srand ( time(NULL) );
      initscr();
      int i=0;
      while(1)
      {
	    printw("Current Sensor output:\n\n");
	    printw("Magnetometer:\t X:\t%d\tY:\t%d\tZ:\t%d\n", rand() % 100, rand() % 100, rand() % 100);
	    printw("Accelerometer:\t X:\t%d\tY:\t%d\tZ:\t%d\n", rand() % 100, rand() % 100, rand() % 100);
	    printw("Gyroscope:\t X:\t%d\tY:\t%d\tZ:\t%d\n", rand() % 100, rand() % 100, rand() % 100);
	    printw("GPS:\t\t Long.:\t%d\tLat.:\t%d\tAlt.:\t%d\n", rand() % 1000, rand() % 1000, rand() % 1000);
	    refresh();
	    usleep(500*1000);	    
	    erase();
      }
      endwin();
      
      
      return 0;
}
      
      