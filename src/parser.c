// this file can be ignored for right now
// it contains code from main that parses 
// the .txt file. eventually this will be
// imported into main.c

#include <stdio.h>
#include <string.h>
#include "parser.h"

struct slide {
    const char *title;
    const char *content;
    int number;
    int r, g, b;
};

void parseTXT(FILE *inFile)
{
    char buf[1000];
    int i = 0;
    // initialize global variables
    int s, x, y;
    char *globalTitle;
    // get meta variables
    // todo : add ability to pull both area and slides from variables in txt file
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "title")!=NULL) { // finds title line
            char quoted[128];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
                // strcat(*title, quoted);
                *globalTitle = quoted;
		        continue;
            } else {
                fprintf(stderr, "improper title\n");
            }
        } else if (strstr(buf, "area")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //area == quoted;
		        continue;
            } else {
                fprintf(stderr, "improper area\n");
            }
        } else if (strstr(buf, "slides")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //slideCount == quoted;
		        continue;
            } else {
                fprintf(stderr, "improper slide count\n");
            }
        } else if (strstr(buf, "{STARTSLIDE}")!=NULL) {
            break;
        }
    }
    struct slide slides[s]; // how should this be returned? pointer?
    char slideBody[s][(x+1)*(y+1)]; // should this be freed later on? 
    slideBody[0][0] = '\0'; // erases junk characters

    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "__________")!=NULL || strstr(buf, "|%*|")!=NULL) { // finds line of slide
            strcat(slideBody[i], buf);
        } else if (strstr(buf, "{ENDSLIDE}")!=NULL) { // iterate to the next slide
		    struct slide placeholder = {slideBody[i], *globalTitle, i, 0, 0, 0};
            i++;
            slideBody[i][0] = '\0';
        }
    }
}
