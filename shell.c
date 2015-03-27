#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* definition of functions */
static char *read_line(char *line, int buffer);

/* List of commands allowed to the client */
char *cmd[1] = { "exit" };

/* line to be read from the stdin */
char line[200];

/**
 * Initialize command line client
 * 
 * @return int
 */
int main(void)
{       
    int l;
    
    while(1) {
        if (!read_line(line, sizeof(line)))
            break;
        
        /* Cleaning up break line from the input*/
        l = strlen(line);
        if (l == 0)
                break;
        if (line[--l] == '\n') {
                if (l == 0)
                    break;
                line[l] = '\0';
        } else if (l == sizeof(line) - 2) {
            
            /* Prints error if command line is longer that 200 characters */
            fprintf(stderr, "Command line is too long.\n");
            break;
        }
        
        /* Exits if line is equal to exit */
        if(strcmp(line, cmd[0]) == 0)
            break;
        fprintf(stdout,"%s\n",line);

    }
    return EXIT_SUCCESS;
}

/**
 * It reads a line from the stdin.
 * 
 * @param line
 * @param buffer
 * @return 
 */
static char *read_line(char *line, int buffer)
{
    printf("CLI> ");
    fflush(stdout);
    return fgets(line, buffer, stdin);
}