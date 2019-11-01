#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "slides.h"

// holds slide count
int s;


slide* parseTXT(FILE *inFile, int* slideCounter, char *presTitle)
{
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
        } else if (strstr(buf, "slides=")!=NULL) {
            if (sscanf(buf, "%*[^\"]\"%d[^\"]\"", &s) == 1) {
                *slideCounter = s;
                continue;
            } else {
                fprintf(stderr, "improper slide declaration\n");
            }
        } else if (strstr(buf, "{STARTSLIDES}")!=NULL) {
            break;
        }
    }

    // allocate memory to the heap for storing our array of slides
    // an array is used here to enable jumping to slides by number
    slide* slides = createSlideArray(s);
    memset(slides, 0, sizeof(*slides));

    // create a line pointer, l is for iterating while first is
    // the pointer used in each slide struct
    line *l = newLine();
    line *first = l;
    l->content[0] = '\0';

    // curMaxX and curY are used to set x/y values to slides
    int curMaxX = 0;
    int curY = 0;

    // continue itteration over file starting after STARTSLIDES
    int i = 0;
    while(fgets(buf, 1000, inFile)!=NULL) {
	// if at end, assign slide values and move to next
        if (strstr(buf, "{ENDSLIDE}")!=NULL) {
	    // assign first of line dub linked list to current slide
            slides[i].first = first;

            slides[i].number = i+1;

            // assign curMaxX to x, reset variable
            slides[i].maxX = curMaxX;
            curMaxX = 0;

            // assign curY to y, reset variable
            slides[i].y = curY;
            curY = 0;

	    // assign r,g,b values (currently not a feature)
            slides[i].r = 0;
            slides[i].g = 0;
            slides[i].b = 0;

	    // increment i to work with next slide
            i++;
            if (i==*slideCounter) 
                break;

	    // for next slide, mem alloc a new line
	    l = newLine();
            first = l;

        } else {
            // add buf to current line and itterate to the next
            strcat(l->content, buf);
            l = nextLine(l);

	    // with each line, y increases
            curY++;

            // update curMaxX only if line is longer that previous lines
            char end = '\n';
            char *ptr = strchr(buf, end);
            if (ptr) {
                int n = ptr - buf;
                if (n>curMaxX) {
                    curMaxX = n;
                }
            }
        }
    }
    // return array of pointers to slides
    return slides;
}
