// this file can be ignored for right now
// it contains code from main that parses 
// the .txt file. eventually this will be
// imported into main.c

#include <stdio.h>
#include <string.h>
#include "parser.h"

void parseTXT(FILE *inFile, char *fileName, char *title, char slides[5][3131], int *x, int *y)
{
    char buf[1000];
    int i = 0;
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "__________")!=NULL || strstr(buf, "|%*|")!=NULL) { // finds line of slide
            strcat(slides[i], buf);
        } else if (strstr(buf, "{ENDSLIDE}")!=NULL) { // iterate to the next slide
            i++;
		    slides[i][0] = '\0';
        } else if (strstr(buf, "title")!=NULL) { // finds title line
            char quoted[128];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
                // strcat(*title, quoted);
                *title = quoted;
		        continue;
            } else {
                fprintf(stderr, "improper title in %s\n", *fileName);
            }
        } else if (strstr(buf, "area")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //area == quoted;
		        continue;
            } else {
                fprintf(stderr, "improper area in %s\n", *fileName);
            }
        } else if (strstr(buf, "slides")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //slideCount == quoted;
		        continue;
            } else {
                fprintf(stderr, "improper slide count in %s\n", *fileName);
            }
        } else {
            continue;
        }
    }
}
