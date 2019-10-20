#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

void usage() {
    fprintf(stderr, "%s", "Usage: dss [FILE]\n");
    fprintf(stderr, "%s", "A dead simple slide tool for the terminal.\n\n");
    fprintf(stderr, "%s", "With no FILE, or when FILE is -, read standard input.\n\n");
    exit(EXIT_FAILURE);
}

void version() {
    printf("dss %d.%d.%d\n", DSS_VERSION_MAJOR, DSS_VERSION_MINOR, DSS_VERSION_REVISION);
    printf("Copyright (C) 2019 Cody Maxie\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    FILE *file;
    char buf[1000];
    file = fopen(argv[1], "r"); // open the file as read only
    if (!file) {
        fprintf(stderr, "could not read file.");
        return 1;
    }
    // todo : read a file dynamically from input.

    char title[128];
    // todo : assign title value
    int x = 51;
    int y = 16;
    // todo : assign area to x/y values
    int currentSlide = 0;
    // todo : add ability to get maximum slide count
    int slideCount = 5; // todo : add ability to find slide count in file
    char slide[slideCount][(x+1)*(y+1)]; // leave enough space for top row of underscore and newline characters
    // todo : add ability to get slide 
    // initialize ncurses
    int i = 0;
    while(fgets(buf, 1000, file)!=NULL) {
        if (strstr(buf, "___")!=NULL || strstr(buf, "|")!=NULL) {
            strcat(slide[i], buf);
            if (strstr(buf, "_|")!=NULL) {
                i++;
            }
        } else if (strstr(buf, "title")!=NULL) {
            char quoted[128];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
                strcat(title, quoted);
		continue;
            } else {
                fprintf(stderr, "improper title in %s\n", argv[1]);
            }
            continue; // todo : get title of slide show
        } else if (strstr(buf, "area")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //area == quoted;
		continue;
            } else {
                fprintf(stderr, "improper area in %s\n", argv[1]);
            }
            continue; // todo : get area of slide show
        } else if (strstr(buf, "slides")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //slideCount == quoted;
		continue;
            } else {
                fprintf(stderr, "improper slide count in %s\n", argv[1]);
            }
            continue; // todo : get slide count of slide show
        } else {
            continue;
        }
    }
    fclose(file); // closes the file
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
