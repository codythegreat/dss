// this file can be ignored for right now
// it contains code from main that parses 
// the .txt file. eventually this will be
// imported into main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

void parseArea(char *buffer, int* xVar, int* yVar)
{
    char* token;
    int i = 0;
    while ((token = strsep(&buffer, ","))!=NULL) {
        if (i = 0) {
            *xVar = atoi(token);
	    i++;
	} else {
            *yVar = atoi(token);
	    break;
	}
    }
}

Slide *parseTXT(FILE *inFile)
{
    char buf[1000];
    int i = 0;
    // initialize default variables. loop detects if change is necessary
    int s = 15;
    int x = 100;
    int y = 30;
    char *globalTitle;
    // get meta variables
    // todo : add ability to pull both area and slides from variables in txt file
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "title")!=NULL) { // finds title line
            char quoted[128];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
                // strcat(*title, quoted);
                globalTitle = quoted;
		        continue;
            } else {
                fprintf(stderr, "improper title\n");
            }
        } else if (strstr(buf, "area")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
		    parseArea(buf, &x, &y);
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
    // heap allocate array of structs, return pointer;
    Slide *slides = malloc(sizeof(Slide) * s); // how should this be returned? pointer?
    char slideBody[s][(x+1)*(y+1)]; // should this be freed later on? 
    slideBody[0][0] = '\0'; // erases junk characters

    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "__________")!=NULL || strstr(buf, "|%*|")!=NULL) { // finds line of slide
            strcat(slideBody[i], buf);
        } else if (strstr(buf, "{ENDSLIDE}")!=NULL) { // iterate to the next slide
		slides[i].title = globalTitle;
		slides[i].content = slideBody[i];
		slides[i].number = i+1;
		slides[i].r = 0;
		slides[i].g = 0;
		slides[i].b = 0;
                // slides[i] = {slideBody[i], *globalTitle, i, 0, 0, 0};
            i++;
            slideBody[i][0] = '\0';
        }
    }
    return slides;
}
