#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "slides.h"
#include "mdlink.h"

// holds slide count
int slideC;

void printColorValueError(int slideNumber, int lineNumber) {
    fprintf(stderr, "slide %d, line %d - bad color value\n", slideNumber, lineNumber);
    getc(stdin);
}

void handleColorTag(char *buf, ssize_t *lineLength, int slideNumber, int lineNumber, int *colorPairOut) {
    // stores coresponding tag to color values
    char colValues[8][2] = {
        {'B', '0'},
        {'r', '1'},
        {'g', '2'},
        {'y', '3'},
        {'b', '4'},
        {'m', '5'},
        {'c', '6'},
        {'w', '7'},
    };
    // find color tag inside buffer
    char *colPtr = strstr(buf, "COLOR=\"");
    
    // if tag exists, get and assign value, then remove tag
    if (colPtr!=NULL) {
        // get the tag's value
        int foreground = -1;
        int background = -1;
        int i;
        for (i=0;i<8;i++) {
            if (colValues[i][0] == *(colPtr+7)) {
                foreground = colValues[i][1]-48;
            }
            if (colValues[i][0] == *(colPtr+8)) {
                background = colValues[i][1]-48;
            }
        }
        // soft error if bad fg/bg values in tag
        if (foreground==-1 || background==-1 || background==foreground) {
            printColorValueError(slideNumber, lineNumber);
            return;
        }

        int colorNumber = 0;
        // set bg color
        colorNumber = 1 + (background*7);
        // set fg color
        if (foreground>background) {
            colorNumber+=foreground-1;
        } else {
            colorNumber+=foreground;
        }

        if (colorNumber >= 1 && colorNumber <= 56) {
            // assign the tag's value to the line's colorPair
            if (colorPairOut) {
                *colorPairOut = colorNumber;
            }

            // remove the tag from buf by shifting each character up over the tag
            char currentChar;
            int i=0;
            do {
                currentChar = *(colPtr + (10+i));
                buf[(colPtr-&buf[0])+i] = currentChar;
                i++;
            } while (currentChar!='\0');
            if (lineLength) {
                *lineLength -= 10;
            }
        
        // if number is bad, print a soft error
        } else {
            printColorValueError(slideNumber, lineNumber);
            return;
        }
    }
}

// searches for a link in the buffer and parses it into mdlink struct
void handleMarkdownStyleLink(char *buf, slide *s) {
    // points to first char '[' in link
    char *linkPtr = strchr(buf, '[');
    if (linkPtr) {
        // holds first/last char addresses in title
        char *titleStart = linkPtr + 1;
        char *titleEnd = NULL;

        // evaluate each character to see where title ends
        while (*linkPtr!='\0') {
            linkPtr+=1;
            // ] marks end of title, go back one character and break
            if (*linkPtr==']') {
                titleEnd = linkPtr-1;
                break;
            }
            if (*linkPtr=='\n') {
                return;
            }
        }

        // if next char is ( then build link title and url
        linkPtr+=1;
        if (*linkPtr=='(') {
            // holds first last char addresses in url
            char *urlStart = linkPtr+1;
            char *urlEnd = NULL;
            while (*linkPtr!='\0') {
                linkPtr++;
                if (*linkPtr==')') {
                    urlEnd = linkPtr-1;
                    break;
                }
                if (*linkPtr=='\n') {
                    return;
                }
            }

            size_t parsedURLLength = urlEnd - urlStart + 1;
            char *url;
            size_t urlLength;
            bool shouldFreeURL;
            // make sure that http in url before assigning as value to struct
            if (strnstr(urlStart, "http://", parsedURLLength) || 
                strnstr(urlStart, "https://", parsedURLLength)) {
                url = urlStart;
                urlLength = parsedURLLength;
                shouldFreeURL = false;
            } else {
                char *formatString = NULL;
                asprintf(&formatString, "https://%%.%lds", parsedURLLength);
                urlLength = asprintf(&url, formatString, urlStart);
                free(formatString);
                shouldFreeURL = true;
            }

            mdlink *link = newLink(
                titleStart,
                titleEnd - titleStart + 1,
                url,
                urlLength
            );

            if (shouldFreeURL) {
                free(url);
            }

            if (s->link) {
                s->link->next = link;
                link->index = s->link->index + 1;
            } else {
                s->link = link;
            }
        }
    }
}

slide* parseTXT(FILE *inFile, int* slideCounter, char *presTitle)
{
    // reset slide counter to 0
    slideC = 0;

    // create a buffer for each line
    char *buf = NULL;
    size_t buflen = 0;
    ssize_t lineLength = 0;

    // parse inFile for meta information
    while((lineLength = getline(&buf, &buflen, inFile)) != -1) {
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

    // create line pointers, previousLine is for iterating while 
    // firstLine is the pointer used in each slide struct
    line *previousLine = NULL;
    line *firstLine = NULL;

    // curMaxX and curY are used to set x/y values to slides
    ssize_t curMaxX = 0;
    int curY = 0;

    int startSlides = 0; // when {STARTSLIDES} encounterd, set as 1
    

    // continue itteration over file starting after STARTSLIDES
    int i = 0;
    while((lineLength = getline(&buf, &buflen, inFile)) != -1) {
        if (strstr(buf, "{STARTSLIDES}")!=NULL) {
            startSlides++;
            continue;
        }
        if (startSlides==0) {
            continue;
        }
        // if at end of slide, assign slide values and move to next
        if (strstr(buf, "{ENDSLIDE}")!=NULL) {
            s->first = firstLine;
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
    	    previousLine = NULL;
            firstLine = NULL;

        } else {
            // if line contains color tag, get color value and set l->colorPair
            int colorPair = 0;
            handleColorTag(buf, &lineLength, i+1, curY+1, &colorPair);

            // if line contains a link, add it to the slide's links
            handleMarkdownStyleLink(buf,s);

            // add buf to current line and iterate to the next
            line *line = newLine(buf, lineLength);
            line->colorPair = colorPair;
            if (previousLine) {
                line->prev = previousLine;
                previousLine->next = line;
            }
            previousLine = line;
            firstLine = firstLine ?: line;

            // with each line, y increases
            curY++;

            // update curMaxX only if line is longer that previous lines
            curMaxX = (lineLength > curMaxX) ? lineLength : curMaxX;
        }
    }
    if (buf) {
        free(buf);
    }
    // return first slide
    return beginning;
}
