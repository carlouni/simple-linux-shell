#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <error.h>


void execcat(char **argv);
void scanfile(FILE *file);

/* Flags to capture options entered from the terminal */
static int nFlag = 0;
static int EFlag = 0;

/**
 * Reads input from stdin and prints it in stdout.
 * @param argc
 * @param argv
 * @return int
 */
int main( int argc, char **argv )
{
    int ch;
    
    /* Parsing options entered from terminal*/
    while ((ch = getopt(argc, argv, "nE")) != -1) {
        
        /* Setting up option flags*/
        switch (ch) {
            case 'n':
                nFlag = 1;
                break;
            case 'E':
                EFlag = 1;
                break;
        }
    }

    /* pointer to '-' . After it, params should be files */
    argv += optind;
     
    /* execute cat */
    execcat(argv);

    return 0;
}

void execcat(char **argv)
{
    FILE *file;
    file = stdin;
    char *fname = "stdin";
    
    do {
        if (*argv) {
            if (!strcmp(*argv, "-"))
                file = stdin;
            else if ((file = fopen(*argv, "r")) == NULL) {
            	fprintf(stderr, "cat: %s: No such file or directory\n", *argv); 
                ++argv;
                continue;
            }
            fname = *argv++;
        }
        
        /* If file check is successful. file scanning starts */
        scanfile(file);

        if (file != stdin)
            (void)fclose(file);
        
    } while (*argv);
}

void scanfile(FILE *file)
{
    int ch;
    char prev;
    int count = 1;
    
    while( ( ch = fgetc(file) ) != EOF ) {
        if ( nFlag && (prev == '\n' || count == 1) ){
            fprintf(stdout,"%6d\t",count);
            count++;
        }
        
        if ( EFlag && ch == '\n'){
            fputc('$', stdout);
        }
        fputc(ch, stdout);
        prev = ch;
    }
   fclose(file);
}