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
    /* fprintf(stderr, "%s", "  -s     set the number of slides (default 15)\n"); */
}

void version() {
    printf("dss %d.%d.%d\n", DSS_VERSION_MAJOR, DSS_VERSION_MINOR, DSS_VERSION_REVISION);
    printf("Copyright (C) 2019 Cody Maxie\n");
}

int main(int argc, char *argv[])
{
    int slideCount = 15; // default value; should be changed with each file

    char ch;
    while ((ch=getopt(argc, argv, "hvx:y:s:"))!=EOF) {
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
        case 's':
            slideCount = atoi(optarg);
            break;
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

    // todo : assign area to x/y values
    FILE *file;
/* #if DSS_DEBUG */
/*     fprintf(stderr, "%s: %s\n", "file", argv[0]); */
/* #endif */
    file = fopen(argv[0], "r"); // open the file as read only
    if (!file) {
        fprintf(stderr, "%s: %s: '%s'\n", PROGNAME, "could not read file", argv[0]);
        return EXIT_FAILURE;
    }

    char title[256]; // TODO can titles be dynamic?  Should they be?
                     // Maybe we can determine the longest title
                     // in the Scanner function?
                     // -am
    title[0] = '\0';
    int currentSlide = 0;
    slide* slides = parseTXT(file, &slideCount, title);
    // close file after parsing
    fclose(file);
    setSlideCount(&slideCount);
    displayLoop(slides, &currentSlide, title, argv[0]);
    int i;
    for (i=0;i<slideCount;i++) {
        freeLines(slides[i].first);
    }
    free(slides);
    return EXIT_SUCCESS;
}
// vim: set ts=4 sts=4 sw=4:
