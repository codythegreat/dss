#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "slides.h"

// defaults
int x = 100;
int y = 30;
int s = 15;
char *globalTitle;


slide* parseTXT(FILE *inFile, int* slideCounter, char *presTitle)
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
    // Slide* slides = (Slide*)malloc(s * sizeof(Slide)); original line
    slide* slides = createSlideArray(s);
    //slides[0].content[0] = '\0'; // erases junk characters
    line *l = malloc(sizeof(line));
    line *first = l;
    l->content[0] = '\0';
    int i = 0;
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "{ENDSLIDE}")!=NULL) { // iterate to the next slide
            slides[i].first = first;
            l = malloc(sizeof(line));
            first = l;
            slides[i].number = i+1;
	        slides[i].x = x;
	        slides[i].y = y;
            slides[i].r = 0;
            slides[i].g = 0;
            slides[i].b = 0;
            i++;
            if (i>=s) 
                break;
        } else { // finds line of slide
            strcat(l->content, buf);
            l = nextLine(l);
        }
    }
    return slides;
}
