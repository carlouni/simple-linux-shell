#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

/* to be used for error handling when reading files */
extern int errno ;

/* function declarations */
void execcat(char **argv);
void scanfile(FILE *file);

/* Flags to capture options entered from the terminal */
static int nFlag = 0;
static int EFlag = 0;
static int bFlag = 0;
static int sFlag = 0;
static int vFlag = 0;
static int tFlag = 0;

/**
 * Main function of cat. It accept arguments in the format [options]... [files]...
 * @param argc
 * @param argv
 * @return int
 */
int main( int argc, char **argv )
{
    int ch;
    
    /* Parsing options entered from terminal*/
    while ((ch = getopt(argc, argv, "nEbsvt")) != -1) {
        
        /* Setting up option flags*/
        switch (ch) {
            case 'n':
                nFlag = 1;
                break;
            case 'E':
                EFlag = 1;
                break;
            case 'b':
                bFlag = 1;
                // b is special case of n, so nFlag should be activated
                nFlag = 1;
                break;
            case 's':
                sFlag = 1;
                break;
            case 'v':
                vFlag = 1;
                break;
            case 't':
                tFlag = 1;
                // t does what v does except for the TAB character
                vFlag =1;
                break;
        }
    }

    /* pointer to '-' . After it, params should be files */
    argv += optind;
     
    /* execute cat */
    execcat(argv);

    return EXIT_SUCCESS;
}

/**
 * Identifies file or files to be scanned. After a file is identified, it scans
 * the file by calling the function scanfile.
 * @param argv
 */
void execcat(char **argv)
{
    FILE *file;
    file = stdin;
    
    do {
        if (*argv) {
            if (!strcmp(*argv, "-")) {
                /* stdin is handle as a normal file */
                file = stdin;
            }
            else if ((file = fopen(*argv, "r")) == NULL) {
            	fprintf(stderr, "cat: %s: %s\n", *argv, strerror( errno ));
                ++argv;
                continue;
            }
            *argv++;
        }
        
        /* If file check is successful. file scanning starts */
        scanfile(file);
        
        /* Close file after scanning it */
        if (file != stdin)
            (void)fclose(file);
        
    } while (*argv);
}

/**
 * Scans file entered as parameter. It proceeds in the same way when the stdin is
 * entered as parameter.
 * @param file
 */
void scanfile(FILE *file)
{
    int ch;
    char prev;
    int count = 1;
    int blkPrinted = 0;
    
    while ((ch = fgetc(file)) != EOF) {
                
        /*
         * If 's' is activated and if it has already printed a blank line it
         * continues to next loop iteration.
         */
        if (sFlag && prev == '\n' && ch == '\n') {
            if (blkPrinted) {
                continue;
            }
            blkPrinted = 1;
        } else if (sFlag && prev == '\n' && ch != '\n') {
            blkPrinted = 0;
        }
        
        /* 
         * Number is printed if n is activated. However, if b is activated it
         * only prints if non-blank line is found.
         */
        if ((nFlag && (prev == '\n' || count == 1)) && (!bFlag || ch != '\n')) {
            fprintf(stdout,"%6d\t",count);
            count++;
        }

        /* If option E */
        if (EFlag && ch == '\n') {
            fputc('$', stdout);
        }

        /* If option v. This is also activated in option t */
        if (vFlag) {
            if (!isascii(ch)) {
                ch = (ch) & 0x7f;
            }

            /* If tFlag activated and ch == TAB, it prints ^I */
            if (iscntrl(ch) && ch != '\n' && (tFlag || ch != '\t')) {
                fputc('^', stdout);
                fputc(ch == '\177' ? '?' : ch | 0100, stdout); 
                continue;
            } 
        }
        fputc(ch, stdout);
        prev = ch;
    }
}