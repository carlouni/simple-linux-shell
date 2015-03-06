#include "stdio.h"
#include "stdlib.h"

/**
 * Reads input from stdin and prints it in stdout.
 * @param argc
 * @param argv
 * @return int
 */
int main( int argc, char **argv )
{
    int c;
    do {
        /* Reading stdin using fgetc */
    	c = fgetc(stdin);
      	if (feof(stdin)) {
            break ;
      	}
        
        /* Printing result using fputc in stdout */
      	fputc(c, stdout);
    } while (1);

    return 0;
}