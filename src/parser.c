#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// defaults
int x = 100;
int y = 30;
int s = 15;
char *globalTitle;


Slide* parseTXT(FILE *inFile, int* slideCounter, char *presTitle)
{
    char buf[1000];
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "title=")!=NULL) { // finds title line
            char quoted[128];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
                globalTitle = quoted;
            } else {
                fprintf(stderr, "improper title\n");
            }
        } else if (strstr(buf, "areaX=")!=NULL) {
            if (sscanf(buf, "%*[^\"]\"%d[^\"]\"", &x) == 1) {
                continue;
            } else {
                fprintf(stderr, "improper area\n");
            }
        } else if (strstr(buf, "areaY=")!=NULL) {
            if (sscanf(buf, "%*[^\"]\"%d[^\"]\"", &y) == 1) {
                continue;
            } else {
                fprintf(stderr, "improper area\n");
            }
        } else if (strstr(buf, "slides=")!=NULL) {
            if (sscanf(buf, "%*[^\"]\"%d[^\"]\"", &s) == 1) {
                continue;
            } else {
                fprintf(stderr, "improper slide declaration\n");
            }
        } else if (strstr(buf, "{STARTSLIDES}")!=NULL) {
            break;
        }
    }
    *slideCounter = s;
    strcat(presTitle, globalTitle);

    // allocate memory to the heap for storing our array of slides
    // an array is used here to enable jumping to slides by number
    Slide* slides = (Slide*)malloc(s * sizeof(Slide));
    slides[0].content[0] = '\0'; // erases junk characters
    int i = 0;
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "_____")!=NULL || strstr(buf, "|")!=NULL) { // finds line of slide
            strcat(slides[i].content, buf);
        } else if (strstr(buf, "{ENDSLIDE}")!=NULL) { // iterate to the next slide
            slides[i].number = i+1;
	    slides[i].x = x;
	    slides[i].y = y;
            slides[i].r = 0;
            slides[i].g = 0;
            slides[i].b = 0;
            i++;
            if (i>=s) 
                break;
            slides[i].content[0] = '\0';
        }
    }
    return slides;
}
