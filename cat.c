#include "stdio.h"
#include "stdlib.h"

int main( int argc, char **argv )
{
    int c;
    do
   	{
    	c = fgetc(stdin);
      	if( feof(stdin) )
      	{
        	break ;
      	}
      	fputc(c, stdout);
   	}
   	while(1);

   	return 0;
}