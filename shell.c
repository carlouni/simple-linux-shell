#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

/* definition of functions */
char ** parse_args();
int my_system(const char *command);
static char *read_line(char *line, int buffer);

/* list of variables and functions to keep track of child processes */
int find_chld_proc(int pid);
int add_chld_proc(int pid);
int async_chld_procs[100] = {0}; // list of asyncronous child processes' IDs
int pos_asc_proc = 0; // position of last child process's ID in async_chld_procs

/* List of function that handle custom commands in the mini-shell */
void exit_(void);
void cd(const char *path);
struct cmd *getcmd(const char *name);

/* table structure for extra commands on the mini-shell */
struct cmd {
    const char *c_name; // command name

    /* 
     * Handlers for command depending of nature of parameters. Values can be
     * NULL; however, one of them must be non-NULL
     */
    void (*c_handler_v)(int, char **);
    void (*c_handler_0)(void);
    void (*c_handler_1)(const char *);
};

/* List of extra commands for the mini-shell */
struct cmd cmdtab[] = {
    { "exit", NULL, exit_, NULL },
    { "cd", NULL, NULL, cd },
    { 0, 0, 0, 0 }
};

extern int errno;

/* line to be read from the stdin */
char line[200];

sig_atomic_t child_exit_status;

/**
 * Tracks child processes.
 * 
 * @param signal_number
 */
void track_child_process(int signal_number)
{
    /* Clean up the child process. */
    int status;
    int pid = wait(&status);
    
     /* Store its exit status in a global variable.*/
    child_exit_status = status;
    int pos = find_chld_proc(pid);
    if (WIFEXITED(status) && pos >= 0)
        fprintf(stdout, "[%d] %d Done\n", (pos + 1), pid);
    else if (WIFSIGNALED(status) && pos >= 0)
        fprintf(stdout, "[%d] %d Done\n", (pos + 1), pid);
}

/**
 * Main function of the mini-shell.
 * 
 * @return int
 */
int main(void)
{   
    int l;
    
    /* Handles SIGCHLD by calling track_child_process. */
    struct sigaction sigchld_action;
    memset (&sigchld_action, 0, sizeof (sigchld_action));
    sigchld_action.sa_handler = &track_child_process;
    sigaction (SIGCHLD, &sigchld_action, NULL);
    
    while (1) {
        if (read_line(line, sizeof(line)) == NULL) {
            continue;
        }  
        
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
            continue;
        }   
        
        /* Executes command read from the terminal */
        if(my_system(line) == -1)
            fprintf(stderr, "Unexpected error while executing command.\n");
    }
    return EXIT_SUCCESS;
}

/**
 * Imitates the C library's system function behaviour using the execvp function.
 * 
 * @param command
 * @return 
 */
int my_system(const char *command)
{
    char *tmpcmd;
    char **margv;
    static char *tmp_argv[20];
    char **tmp_argp;
    int l;
    struct cmd *c;
    int amp = 0; // Flag ampersand
    pid_t cpid;
    int cstatus;
    int wait_opt;
    const char *ofile;
    const char *ifile;
    int redout = 0; // flag to redirect stdout to a file
    int redin = 0; // flag to use a file instead of stdin
    FILE *fp;
    int i;
    
    memset(&tmp_argv[0], 0, sizeof(tmp_argv));
    
    /* creates a copy of the command */
    tmpcmd = strdup(command);
    l = strlen(tmpcmd);

    /* Parsing line to get the command and args entered */
    margv = parse_args(tmpcmd);
    
    /* 
     * Searches command in the custom command lists first; if not found, continues
     * execution with the execvp() function.
     */
    c = getcmd(margv[0]);
    
    /* If command exists on the list, executes handler. */
    if (c != NULL ) {
        if (c->c_handler_v) {
            c->c_handler_v(*margv[0], &margv[0]);
            return 0;
        } else if (c->c_handler_0) {
            c->c_handler_0();
            return 0;
        } else if (c->c_handler_1) {
            c->c_handler_1(margv[1]);
            return 0;
        }
    }
    
    i = 0;
    tmp_argp = tmp_argv;
    while (1) {
        if (margv[i] == NULL || !strcmp(margv[i],"\0") || !strcmp(margv[i], ";")) {
            cstatus = 0;
            
            /* Creates a child process to execute command */
            cpid = fork();
            if (cpid == -1) {
                return -1;
            } else if(cpid == 0) {

                /* CHILD PROCESS */
                
                if (redout) {
                    fp = freopen(ofile, "w+", stdout);
                }
                
                if (redin) {
                    fp = freopen(ifile, "r", stdin);
                }

                errno = 0;
                execvp(tmp_argv[0], &tmp_argv[0]);
                if (errno == ENOENT)
                    fprintf(stderr, "%s: command not found\n", margv[0]);
                else
                    fprintf(stderr, "Unexpected error while executing command.\n");
                exit(EXIT_FAILURE);
            } else if (cpid > 0) {

                /* Default: shell waits for children. */
                wait_opt = 0;

                /* If &, shell continues. */
                if (amp) 
                    wait_opt = WNOHANG;

                if (waitpid(cpid, &cstatus, wait_opt) == -1)
                    return -1;
                else if (WIFEXITED(cstatus)) {
                    if (margv[i] != NULL && !strcmp(margv[i], ";")) {
                        memset(&tmp_argv[0], 0, sizeof(tmp_argv));
                        tmp_argp = tmp_argv;
                        i++;
                        if (margv[i] == NULL) {
                            return 0;
                        }
                        
                        /* Resets the option for redirection of stdout & stdin */
                        redout = 0;
                        redin = 0;
                        continue;
                    }

                    if (margv[i] == NULL) {
                        return 0;
                    }
                
                }

                /* If child process is running on background means cstatus >= 0 */
                if (amp && &cstatus != NULL && cstatus >= 0) {
                    memset(&tmp_argv[0], 0, sizeof(tmp_argv));
                    int pos = add_chld_proc(cpid);
                    fprintf(stdout, "[%d] %d\n", (pos + 1), cpid);
                    return 0;
                }
                return -1;
            }
        }
        
        /* if argument is ">", activate redout flag */
        if (!strcmp(margv[i], ">")) {
            ofile= strdup(margv[i + 1]);
            redout = 1;
            *tmp_argp = NULL;
            i = i + 2;
            continue;
        }
        
        /* if argument is "<", activate redin flag */
        if (!strcmp(margv[i], "<")) {
            ifile= strdup(margv[i + 1]);
            redin = 1;
            *tmp_argp = NULL;
            i = i + 2;
            continue;
        }
        
        /* Verifies if last parameter is an & */
        amp = 0;
        if (!strcmp(margv[i], "&") && (margv[i + 1] == NULL || !strcmp(margv[i + 1],"\0") || !strcmp(margv[i + 1], ";")))
            amp = 1;
        
        if (amp) 
            *tmp_argp = '\0';
        else
            *tmp_argp = strdup(margv[i]);
        tmp_argp++;
        i++;
    }
}

/**
 * Read command input from stdin
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
 * Parse command string into an array.
 * 
 * @param command
 * @return 
 */
char ** parse_args(char *command)
{
    static char *rargv[200];
    char **argp;
    char word[200];
    char *p;
    char *pword;
    char prev = ' ';
    
    memset(&rargv[0], 0, sizeof(rargv));
    
    pword = word;
    argp = rargv;
    p = strdup(command);
    int i = 0;
    int l = sizeof(line);
    while ( i < l) {
        switch(*p) {
            case ' ' :
                if (any(prev, " ;<>&"))
                    break;
                *argp = strdup(word);
                argp++;
                memset(&word, 0, sizeof(word));
                pword = word;
                
                break;
            case ';' :
                if (!any(prev, " ;<>&")) {
                    *argp = strdup(word);
                    argp++;
                    memset(&word, 0, sizeof(word));
                    pword = word;
                }
                *argp = ";";
                argp++;
                break;
            case '>' :
                if (!any(prev, " ;<>&")) {
                    *argp = strdup(word);
                    argp++;
                    memset(&word, 0, sizeof(word));
                    pword = word;
                }
                *argp = ">";
                argp++;
                break;
            case '<' :
                if (!any(prev, " ;<>&")) {
                    *argp = strdup(word);
                    argp++;
                    memset(&word, 0, sizeof(word));
                    pword = word;
                }
                *argp = "<";
                argp++;
                break;
            case '&' :
                if (!any(prev, " ;<>&")) {
                    *argp = strdup(word);
                    argp++;
                    memset(&word, 0, sizeof(word));
                    pword = word;
                }
                *argp = "&";
                argp++;
                break;
            case '"' :
                break;
            default:
                *pword = *p;
                pword++;
                break;
        }
        prev = *p;
        p++;
        i++;
    }
    if (pword > word) {
       *argp = strdup(word);
    }
    return rargv;
}

/**
 * Checks if character c is in the string *as
 * 
 * @param c
 * @param as
 * @return 
 */
int any(int c, char *as)
{
    register char *s;
    s = as;
    while(*s)
        if(*s++ == c)
            return(1);
    return(0);
}

/**
 * Resolves the handler of an specific command define  in cmdtab table.
 * 
 * @param name
 * @return 
 */
struct cmd *getcmd(const char *name)
{
    const char *p, *q;
    struct cmd *c;
    for (c = cmdtab; (p = c->c_name) != NULL; c++) {
        for (q = name; *q == *p++; q++)
                if (*q == 0)
                        return (c);
    }
    return NULL;
}

/**
 * Function that executes the 'exit command'
 * @type command handler
 */
void exit_(void)
{
    exit(EXIT_SUCCESS);
}

/**
 * Executes directory change for the shell
 * 
 * @type command handler
 * @param path
 */
void cd(const char *path)
{
    // executes chdir
    errno = 0;
    chdir(path);
    if (errno)
        fprintf(stderr, "shell: cd: %s: %s\n", path, strerror(errno));
}

/**
 * Searches child process's id in the list of asyncronous child processes. Returns
 * the position of the process id in the list. if not found, returns -1.
 * 
 * @param pid
 * @return 
 */
int find_chld_proc(int pid)
{
    int i = 0;
    int l = sizeof(async_chld_procs);
    while(i < l){
        if (async_chld_procs[i] == pid)
            return i;
        i++;
    }
    return -1;
}

/**
 * Adds child process's id to the list of asyncronous child processes.
 *  
 * @param pid
 * @return 
 */
int add_chld_proc(int pid)
{
    async_chld_procs[pos_asc_proc] = pid;
    pos_asc_proc++;
    if (pos_asc_proc < sizeof(async_chld_procs))
        return (pos_asc_proc -1);
    else
        pos_asc_proc = 0;
    return (pos_asc_proc -1);
}