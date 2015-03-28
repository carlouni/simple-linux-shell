#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* definition of functions */
static char *read_line(char *line, int buffer);
char ** parse_args();
int my_system(const char *command);

/* List of extra commands for the mini shell */
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
    int resp;
    
    /* Ignore child signals */
    signal(SIGCHLD, SIG_IGN);
        
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
            
            /* Prints error if command line is longer that buffer characters */
            fprintf(stderr, "Command line is too long.\n");
            break;
        }   
        
        /* Executes command read from the terminal */
        resp = 0;
        resp = my_system(line);
    }
    return EXIT_SUCCESS;
}

int my_system(const char *command) {
    char *tmpcmd;
    int l;
    char **margv;
    int flag_amp = 0;
    pid_t cpid;
    int cstatus;
    int wait_opt;
    
    /* creates a copy of the command */
    tmpcmd = strdup(command);
    l = strlen(tmpcmd);
    
    /* Verifies if last parameter is an & */
    if (tmpcmd[l - 1] == '&') {
        flag_amp = 1;
        tmpcmd[l - 1] = '\0';
    }

    /* Parsing line to get the command and args entered */
    margv = parse_args(tmpcmd);
    
    /* Exits if command line is equal to exit */
    if(strcmp(margv[0], cmd[0]) == 0)
        exit(EXIT_SUCCESS);

    /* Creates a child process to execute command */
    cpid = fork();
    if(cpid == -1) {
        return -1;
    } else if(cpid == 0) {
        
        /* If process is a child */
        execvp(margv[0], &margv[0]);
        
        /* If it continues, that means failure */
        fprintf(stderr, "%s: command not found\n", margv[0]);
        exit(EXIT_FAILURE);
    } else if(cpid > 0) {
        
        /* Default: shell waits for children. */
        wait_opt = 0;
        
        /* If &, shell continues. */
        if (flag_amp) 
            wait_opt = WNOHANG;
            
        /* Parent does not wait for child to finish */
        if (waitpid(cpid, &cstatus, wait_opt) == -1)
            return -1;
        else if (WIFEXITED(cstatus)) 
            return WEXITSTATUS(cstatus); 
        return -1;
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
    const char delim[] = " ";
    char *tmpline;
    
    /* Creates a copy of the read line */
    tmpline = strdup(command);
    
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