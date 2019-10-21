// this file can be ignored for right now
// it contains code from main that parses 
// the .txt file. eventually this will be
// imported into main.c

#include <stdio.h>
#include <string.h>
#include "parser.h"

int parseTXT(FILE *inFile)
{
    int i = 0;
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "__________")!=NULL || strstr(buf, "|%*|")!=NULL) { // finds line of slide
            strcat(slide[i], buf);
        } else if (strstr(buf, "{ENDSLIDE}")!=NULL) { // iterate to the next slide
            i++;
		    slide[i][0] = '\0';
        } else if (strstr(buf, "title")!=NULL) { // finds title line
            char quoted[128];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
                strcat(title, quoted);
		        continue;
            } else {
                fprintf(stderr, "improper title in %s\n", argv[1]);
            }
        } else if (strstr(buf, "area")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //area == quoted;
		        continue;
            } else {
                fprintf(stderr, "improper area in %s\n", argv[1]);
            }
        } else if (strstr(buf, "slides")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //slideCount == quoted;
		        continue;
            } else {
                fprintf(stderr, "improper slide count in %s\n", argv[1]);
            }
        } else {
            continue;
        }
    }
    return 1;
}
