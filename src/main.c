#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "main.h"
#include "parser.h"
#include "display.h"

void usage() {
    fprintf(stderr, "%s", "Usage: dss [OPTIONS]... FILE\n");
    fprintf(stderr, "%s", "A dead simple slide tool for the terminal.\n\n");
    fprintf(stderr, "%s", "  -h     print this message and exit\n");
    fprintf(stderr, "%s", "  -v     display version and copyright information\n");
    fprintf(stderr, "%s", "  -x     change the slide width value (default 100)\n");
    fprintf(stderr, "%s", "  -y     change the slide height value (default 30)\n");
    /* fprintf(stderr, "%s", "  -s     set the number of slides (default 15)\n"); */
}

void version() {
    printf("dss %d.%d.%d\n", DSS_VERSION_MAJOR, DSS_VERSION_MINOR, DSS_VERSION_REVISION);
    printf("Copyright (C) 2019 Cody Maxie\n");
}

int main(int argc, char *argv[])
{
    int x = 100; // default value. Practical solutions typically occupy something like 50x15
    int y = 30;  // This should allow for plenty of slides without ever risking memory issues
    int slideCount = 15; // default value; should be changed with each file

    char ch;

    while ((ch=getopt(argc, argv, "hvx:y:s:"))!=EOF) {
        /* (am) XXX if we want the program to halt immediately upon
        * encountering an unsupported option, we should
        * uncomment this:
        */

        /* if (optopt != 0) */
        /*     goto bad; */

        switch (ch)
        {
        case 'v':
            version();
            goto bad;
        case 'x':
            x = atoi(optarg);
            break;
        case 'y':
            y = atoi(optarg);
            break;
        case 's':
            slideCount = atoi(optarg);
            break;
        case 'h':
            usage();
            goto good;
        default:
            usage();
            goto bad;
        }
    }
    argv += optind; // TODO figure out why this is. -am

    if (argc == 1) {
        /* no args (other than options) */
        fprintf(stderr, "%s: %s\n", PROGNAME, "missing file operand");
        goto bad;
    }

    // todo : assign area to x/y values
    FILE *file;
/* #if DSS_DEBUG */
/*     fprintf(stderr, "%s: %s\n", "file", argv[0]); */
/* #endif */
    file = fopen(argv[0], "r"); // open the file as read only
    if (!file) {
        fprintf(stderr, "%s: %s: '%s'\n", PROGNAME, "could not read file", argv[0]);
        goto bad;
    }

    char title[128]; // TODO can titles be dynamic?  Should they be?
                     // Maybe we can determine the longest title
                     // in the Scanner function?
                     // -am
    title[0] = '\0';
    int currentSlide = 0;
    Slide* slides = parseTXT(file, &slideCount, title); //to be used when parser.c is implemented
    // close file after parsing
    fclose(file);
    setSlideCount(&slideCount);
    initDisplay();
    while(1) {
        displayLoop(&slides[currentSlide], &currentSlide, title, argv[0]);
    }
good:
    return EXIT_SUCCESS;
bad:
    return EXIT_FAILURE;
}
// vim: set ts=4 sts=4 sw=4:
