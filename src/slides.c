#include <stdio.h>
#include <stdlib.h>
#include "slides.h"



slide* createSlideArray(int s) {
    slide* slides = (slide*)malloc(s * sizeof(slide));
    return slides;
}

// not currently in use
//line *newLine() {
    //line *n = malloc(sizeof(line));
    //n->content[0] = '\0'; // fixes junk text
    //n->prev = n->next = NULL;
    //n->r = n->g = n->b = 0;
    //return n;
//}

line *nextLine(line *prev) {
    line *n = malloc(sizeof(line));
    n->prev = prev;
    prev->next = n;
    return n;
}

void freeSlides(slide *slide[], int s) {
    int i;
    for (i=0;i<s;i++) {
        free(slide[i]);
    }
}

void freeLines(line *l) {
    line *current;
    while(l) {
        current = l;
        l = l->next;
        free(current);
    }
}
