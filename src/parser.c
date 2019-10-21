// this file can be ignored for right now
// it contains code from main that parses 
// the .txt file. eventually this will be
// imported into main.c

#include <stdio.h>
#include <string.h>

int parseTXT(FILE *inFile)
{
    while(fgets(buf, 1000, inFile)!=NULL) {
        if (strstr(buf, "___")!=NULL || strstr(buf, "|")!=NULL) {
            strcat(slide[i], buf);
            if (strstr(buf, "_|")!=NULL) {
                i++;
            }
        } else if (strstr(buf, "title")!=NULL) {
            char quoted[128];
            if (sscanf(buf, "%*[^\"]\"%127[^\"]\"", quoted) == 1) {
                strcat(title, quoted);
		continue;
            } else {
                fprintf(stderr, "improper title in %s\n", argv[1]);
            }
            continue; // todo : get title of slide show
        } else if (strstr(buf, "area")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //area == quoted;
		continue;
            } else {
                fprintf(stderr, "improper area in %s\n", argv[1]);
            }
            continue; // todo : get area of slide show
        } else if (strstr(buf, "slides")!=NULL) {
            char quoted[10];
            if (sscanf(buf, "%*[^\"]\"%9[^\"]\"", quoted) == 1) {
                //slideCount == quoted;
		continue;
            } else {
                fprintf(stderr, "improper slide count in %s\n", argv[1]);
            }
            continue; // todo : get slide count of slide show
        } else {
            continue;
        }
    }
}
