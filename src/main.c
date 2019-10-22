#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main.h"

void usage() {
    fprintf(stderr, "%s", "Usage: dss [OPTIONS]... [FILE]\n");
    fprintf(stderr, "%s", "A dead simple slide tool for the terminal.\n\n");
    fprintf(stderr, "%s", "  -v     display the version number and copyright\n");
    fprintf(stderr, "%s", "  -x     change the slide width value (default 100)\n");
    fprintf(stderr, "%s", "  -y     change the slide height value (default 30)\n");
    fprintf(stderr, "%s", "  -s     set the number of slides (default 15)\n");
    fprintf(stderr, "%s", "File is currently a required argument.\n\n");
    exit(EXIT_FAILURE);
}

void version() {
    printf("dss %d.%d.%d\n", DSS_VERSION_MAJOR, DSS_VERSION_MINOR, DSS_VERSION_REVISION);
    printf("Copyright (C) 2019 Cody Maxie\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int x = 100; // default value. Practical solutions typically occupy something like 50x15
    int y = 30;  // This should allow for plenty of slides without ever risking memory issues
    int slideCount = 15; // default value; should be changed with each file

    char ch;

    while ((ch=getopt(argc, argv, "vx:y:s:"))!=EOF) {
        switch (ch)
        {
        case 'v':
            version();
            break;
        case 'x':
            x = atoi(optarg);
            break;
        case 'y':
            y = atoi(optarg);
            break;
        case 's':
            slideCount = atoi(optarg);
            break;
        default:
            usage();
            break;
        }
    }
    argc -= optind;
    argv += optind;
    // todo : assign area to x/y values
    FILE *file;
    file = fopen(argv[0], "r"); // open the file as read only
    if (!file) {
        fprintf(stderr, "could not read file.");
        return 1;
    }

    char title[128];
    title[0] = '\0';
    int currentSlide = 0;
    char slide[slideCount][(x+1)*(y+1)]; // leave enough space for top row of underscore and newline characters
    slide[0][0] = '\0';
    //parseTXT(*file);
    char buf[1000];
    int i = 0;
    while(fgets(buf, 1000, file)!=NULL) {
        if (strstr(buf, "___")!=NULL || strstr(buf, "|")!=NULL) { // finds line of slide
            strcat(slide[i], buf);
        } else if (strstr(buf, "{ENDSLIDE}")!=NULL) { // iterate to the next slide
            i++;
            slide[i][0] = '\0';
        } else if (strstr(buf, "title")!=NULL) { // finds title line
            char quoted[128];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
                strcat(title, quoted);
                continue;
            } else {
                fprintf(stderr, "improper title in %s\n", argv[1]);
            }
        } else if (strstr(buf, "area")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //area == quoted;
		        continue;
            } else {
                fprintf(stderr, "improper area in %s\n", argv[1]);
            }
        } else if (strstr(buf, "slides")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //slideCount == quoted;
		        continue;
            } else {
                fprintf(stderr, "improper slide count in %s\n", argv[1]);
            }
        } else {
            continue;
        }
    }
    fclose(file); // closes the file
    // initialize ncurses
    initscr();
    cbreak();
    noecho();
    clear();
    while(1) {
	printw(title);
	printw("\n");
        printw(slide[currentSlide]);
        char keyInput = getch();
        if (keyInput == 'q' || keyInput == 'Q') {
            break;
        }
        if (keyInput == 'j' || keyInput == 'J') {
            if (currentSlide != slideCount-1) {
                currentSlide++;
            }
        }
        if (keyInput == 'k' || keyInput == 'K') {
            if (currentSlide != 0) {
                currentSlide--;
            }
        }
        clear();
    }
    // end ncurses session
    endwin();
    exit(0);
}
