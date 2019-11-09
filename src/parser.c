#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "slides.h"

// holds slide count
int slideC;

// todo : better tag (maybe {COLOR_N})
void handleColorTag(char buf[1000], line *l, int slideNumber, int lineNumber) {
    // find color tag inside buffer
    char *colPtr;
    colPtr = strstr(buf, "COLOR=\"");
    
    // if tag exists, get and assign value, then remove tag
    if (colPtr!=NULL) {
        // get the tag's value
        int colorNumber = 0;
        colorNumber = atoi(colPtr+7);

        if (colorNumber >= 1 && colorNumber <= 56) {
            // assign the tag's value to the line's colorPair
            l->colorPair = colorNumber;

            // remove the tag from buf by shifting each character up over the tag
            char currentChar;
            int i=0;
            do {
                if (colorNumber>=10) {
                    currentChar = *(colPtr + (10+i));
                } else {
                    currentChar = *(colPtr + (9+i));
                }
                buf[(colPtr-&buf[0])+i] = currentChar;
                i++;
            } while (currentChar!='\n');
        
        // if number is bad, print a soft error
        } else {
            fprintf(stderr, "slide %d, line %d - bad color value\n", slideNumber, lineNumber);
            getc(stdin);
        }
    }
}

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

    // create slide pointers, s is for iterating while 
    // beginning will be the first slide and return val
    slide *s = newSlide();
    slide *beginning = s;

    // create line pointers, l is for iterating while 
    // first is the pointer used in each slide struct
    line *l = newLine();
    line *first = l;
    l->content[0] = '\0';

    // curMaxX and curY are used to set x/y values to slides
    int curMaxX = 0;
    int curY = 0;

    int startSlides = 0; // when {STARTSLIDES} encounterd, set as 1

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
	// if at end of slide, assign slide values and move to next
        if (strstr(buf, "{ENDSLIDE}")!=NULL) {
            s->first = first;
            s->number = i+1;
            s->maxX = curMaxX;
            s->y = curY;
            s->colorPair = 0;
            if (i+1!=slideC) {
                s = nextSlide(s);
            }
            curMaxX = 0;
            curY = 0;
            i++;
            if (i==slideC)
                break;
	    l = newLine();
            first = l;

        } else {
            // if line contains color tag, get color value and set l->colorPair
            handleColorTag(buf, l, i+1, curY+1);
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
