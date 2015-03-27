#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* definition of functions */
static char *read_line(char *line, int buffer);
char ** parse_args();
void my_system(char *command);

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
                continue;
            line[l] = '\0';
        } else if (l == sizeof(line) - 2) {
            
            /* Prints error if command line is longer that 200 characters */
            fprintf(stderr, "Command line is too long.\n");
            break;
        }
        
        /* Executes command read from the terminal */
        my_system(line);
    }
    return EXIT_SUCCESS;
}

void my_system(char *command) {
    char **margv;
    int cpid;
    int cstatus;
    pid_t tpid;
    
    /* Parsing line to get the command and args entered */
    margv = parse_args(command);
    
    /* Exits if line is equal to exit */
    if(strcmp(margv[0], cmd[0]) == 0)
        exit(EXIT_SUCCESS);

    /* Creates a child process to execute command */
    cpid = fork();
    
    /* If process is a child */
    if(cpid == 0) {
        execvp(margv[0], &margv[0]);
        
        /* If it continues, that means failure*/
        printf("Unknown command\n");
        exit(EXIT_SUCCESS);
    } else {
        
        /* Parent waits for child to finish */
        do {
            tpid = wait(&cstatus);
            
            /* If process finishes faster than expected */
            if(tpid != cpid)
                break;
        } while(tpid != cpid);
    }
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
char ** parse_args(char *command) {
    static char *rargv[20];
    char **argp;
    const char *delim;
    char tmpline[sizeof(line)];
    
    /* defines the delimiter for arguments */
    delim = " ";
    
    /* Creates a copy of the read line */
    strcpy(tmpline, command);
    
    argp = rargv;
    
    /* get the first token */
    *argp = strtok(tmpline, delim);
   
    /* walk through other tokens */
    while( *argp != NULL ) 
    {
        argp++;
        *argp = strtok(NULL, delim);
    }
    return rargv;
}