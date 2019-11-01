#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <locale.h> 
#include "main.h"
#include "parser.h"
#include "display.h"

void usage() {
    fprintf(stderr, "%s", "Usage: dss [OPTIONS]... FILE\n");
    fprintf(stderr, "%s", "A dead simple slide tool for the terminal.\n\n");
    fprintf(stderr, "%s", "  -h     print this message and exit\n");
    fprintf(stderr, "%s", "  -v     display version and copyright information\n");
}

void version() {
    printf("dss %d.%d.%d\n", DSS_VERSION_MAJOR, DSS_VERSION_MINOR, DSS_VERSION_REVISION);
    printf("Copyright (C) 2019 Cody Maxie\n");
}

int main(int argc, char *argv[])
{
    int slideCount;

    char ch;
    while ((ch=getopt(argc, argv, "hv"))!=EOF) {
        /* (am) XXX if we want the program to halt immediately upon
        * encountering an unsupported option, we should
        * uncomment this:
        */

        /* if (optopt != 0) */
        /*     return EXIT_FAILURE; */

        switch (ch)
        {
        case 'v':
            version();
            return EXIT_SUCCESS;
        case 'h':
            usage();
            return EXIT_SUCCESS;
        default:
            usage();
            return EXIT_FAILURE;
        }
    }
    argv += optind; // TODO figure out why this is. -am

    if (argc == 1) {
        /* no args (other than options) */
        fprintf(stderr, "%s: %s\n", PROGNAME, "missing file operand");
        return EXIT_FAILURE;
    }

    // enables UTF-8 support if available on system
    setlocale(LC_CTYPE, "");

/* #if DSS_DEBUG */
/*     fprintf(stderr, "%s: %s\n", "file", argv[0]); */
/* #endif */

    char fileName[1000];
    fileName[0] = '\0';
    strcat(fileName, argv[0]);

    char title[256]; // TODO can titles be dynamic?  Should they be?
                     // Maybe we can determine the longest title
                     // in the Scanner function?
                     // -am
    title[0] = '\0';
    int currentSlide = 0;

    // initialize ncurses display
    int displayInitialized = 0;

    //0-exit, 1-open new file
    int returnCode; 
    do
    {
        // if opening new file, reset vars from last loop
        if (returnCode==1) {
            title[0] = '\0';
            currentSlide = 0;
            returnCode = 0;
        }
        // open the file, otherwise return error
        FILE *currentFile;
        currentFile = fopen(fileName, "r");
        if (!currentFile) {
            fprintf(stderr, "%s: %s: '%s'\n", PROGNAME, "could not read file", fileName);
            return EXIT_FAILURE;
        }

        if (!displayInitialized) {
            initDisplay();
            displayInitialized = 1;
        }

        // parse and return slides from txt file
        slide *slides = parseTXT(currentFile, &slideCount, title);
        // close the file
        fclose(currentFile);
        // initiate display loop, when it returns store exit code
        returnCode = displayLoop(slides, &currentSlide, &slideCount, title, fileName);
        
        // free slides and lines
        int i;
        // todo: something is causing the below code to not function correctly when opening new file
        for (i=0;i<slideCount;i++){
            freeLines(slides[i].first);
        }
        memset(slides, 0, sizeof(*slides));
        free(slides);
    } while (returnCode == 1);

    return EXIT_SUCCESS;
}
// vim: set ts=4 sts=4 sw=4:
