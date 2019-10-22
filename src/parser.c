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

void checkSlide(Slide s)
{
	printf("%s %s\n%i\n", s.content, s.title, s.number);
}

Slide* parseTXT(FILE *inFile)
{
    char buf[1000];
    // initialize default variables. loop detects if change is necessary
    int s = 5;
    int x = 100;
    int y = 30;
    char *globalTitle;
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "title")!=NULL) { // finds title line
            char quoted[128];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
                globalTitle = quoted;
            } else {
                fprintf(stderr, "improper title\n");
            }
        } else if (strstr(buf, "area")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
		    parseArea(quoted, &x, &y);
            } else {
                fprintf(stderr, "improper area\n");
            }
        } else if (strstr(buf, "slides")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
		puts(quoted);
                s = atoi(quoted);
		printf("%i\n", s);
            } else {
                fprintf(stderr, "improper slide count\n");
            }
        } else if (strstr(buf, "{STARTSLIDE}")!=NULL) {
            break;
        }
    }
    printf("%i, %i, %i\n", x, y, s); 
    Slide* slides = (Slide*)calloc(s, sizeof(Slide));
    char slideBody[s][(x+1)*(y+1)]; // should this be freed later on? 
    slideBody[0][0] = '\0'; // erases junk characters
    // I don't believe that the below code is actually running (due to file closing?)
 
    int i = 0;
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
            checkSlide(slides[i]); // test function to be removed
            i++;
            slideBody[i][0] = '\0';
        }
    }
    return slides;
}
