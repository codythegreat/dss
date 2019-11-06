#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "slides.h"

// holds slide count
int slideC;


slide* parseTXT(FILE *inFile, int* slideCounter, char *presTitle)
{
    // reset slide counter to 0
    slideC = 0;

    // create a buffer for each line
    char buf[1000];

    // parse inFile for meta information
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "title=")!=NULL) { // finds title line
            char quoted[256];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
		        strcat(presTitle, quoted);
            } else {
                fprintf(stderr, "improper title\n");
            }
        } else if (strstr(buf, "{ENDSLIDE}")!=NULL) {
            slideC++;
        }
    }
    rewind(inFile);
    *slideCounter = slideC;

    // allocate memory to the heap for storing our array of slides
    // an array is used here to enable jumping to slides by number
    slide* beginning = newSlide();
    slide* s;

    // create a line pointer, l is for iterating while first is
    // the pointer used in each slide struct
    line *l = newLine();
    line *first = l;
    l->content[0] = '\0';

    // curMaxX and curY are used to set x/y values to slides
    int curMaxX = 0;
    int curY = 0;

    int startSlides = 0; // when slides start, begin filling structs

    // continue itteration over file starting after STARTSLIDES
    int i = 0;
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "{STARTSLIDES}")!=NULL) {
            startSlides++;
            continue;
        }
        if (startSlides==0) {
            continue;
        }
	    // if at end, assign slide values and move to next
        if (strstr(buf, "{ENDSLIDE}")!=NULL) {
            if (i==0) {
                beginning->first = first;
                beginning->number = 1;
                beginning->maxX = curMaxX;
                beginning->y = curY;
                beginning->r = beginning->g = beginning->b = 0;
		if (slideC>1) {
                    s = nextSlide(beginning);
		}

            } else {
                s->first = first;
                s->number = i+1;
                s->maxX = curMaxX;
                s->y = curY;
                s->r = s->g = s->b = 0;
                if (i+1!=slideC) {
                    s = nextSlide(s);
                }
            }
            curMaxX = 0;
            curY = 0;
            i++;
            if (i==slideC)
                break;
	    l = newLine();
            first = l;

        } else {
            // replace new line character with string terminator character
	    char *endLine;
	    endLine = strchr(buf, '\n');
	    *endLine = '\0';
            // add buf to current line and itterate to the next
            strcat(l->content, buf);
            l = nextLine(l);

            // with each line, y increases
            curY++;

            // update curMaxX only if line is longer that previous lines
            char end = '\0';
            char *ptr = strchr(buf, end);
            if (ptr) {
                int n = ptr - buf;
                if (n>curMaxX) {
                    curMaxX = n;
                }
            }
        }
    }
    // return first slide
    return beginning;
}
