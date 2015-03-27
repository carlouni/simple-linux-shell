#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* definition of functions */
static char *read_line(char *line, int buffer);
char ** parse_args();

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
    char **margv;  
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
                continue;
            line[l] = '\0';
        } else if (l == sizeof(line) - 2) {
            
            /* Prints error if command line is longer that 200 characters */
            fprintf(stderr, "Command line is too long.\n");
            break;
        }
        
        /* Parsing line to get the command and args entered */
        margv = parse_args();
        
        /* Exits if line is equal to exit */
        if(strcmp(margv[0], cmd[0]) == 0)
            break;
        
        /* Executes arbitrary commands using the system function */
         system(line);
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

/**
 * Parses line entered into an array.
 * 
 * @return 
 */
char ** parse_args() {
    static char *rargv[20];
    char **argp;
    const char *delim;
    char tmpline[sizeof(line)];
    
    /* defines the delimiter for arguments */
    delim = " ";
    
    /* Creates a copy of the read line */
    strcpy(tmpline, line);
    
    argp = rargv;
    
    /* get the first token */
    *argp = strtok(tmpline, delim);
   
    /* walk through other tokens */
    while( *argp != NULL ) 
    {
        argp++;
        *argp = strtok(NULL, tmpline);
    }
    return rargv;
}