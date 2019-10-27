#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "slides.h"

// defaults
int s;
char *globalTitle;


slide* parseTXT(FILE *inFile, int* slideCounter, char *presTitle)
{
    char buf[1000]; // stores each line
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "title=")!=NULL) { // finds title line
            char quoted[128];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
                globalTitle = quoted;
            } else {
                fprintf(stderr, "improper title\n");
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
    slide* slides = createSlideArray(s);

    // create a line pointer, l is for iterating while first is
    // the pointer used in each slide struct
    line *l = malloc(sizeof(line));
    line *first = l;
    l->content[0] = '\0';

    // curMaxX and curY are used to set x/y values to slides
    int curMaxX = 0;
    int curY = 0;

    // continue itteration over file
    int i = 0;
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "{ENDSLIDE}")!=NULL) { // iterate to the next slide
            slides[i].first = first;
            l = malloc(sizeof(line));
            first = l;
            slides[i].number = i+1;
            // assign curMaxX to x, reset variable
            slides[i].maxX = curMaxX;
            curMaxX = 0;
            // assign curY to y, reset variable
	        slides[i].y = curY;
            curY = 0;
            slides[i].r = 0;
            slides[i].g = 0;
            slides[i].b = 0;
            i++;
            if (i>=s) 
                break;
        } else {
            // add buf to current line and itterate to the next
            strcat(l->content, buf);
            l = nextLine(l);

            // update curMaxX only if line is longer that previous lines
            char end = '\n';
            char *ptr = strchr(buf, end);
            if (ptr) {
                int n = ptr - buf;
                if (n>curMaxX) {
                    curMaxX = n;
                }
                curY++;
            }
        }
    }
    return slides;
}
