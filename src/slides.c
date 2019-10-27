#include <stdio.h>
#include <stdlib.h>
#include "slides.h"

slide* createSlideArray(int s) {
    slide* slides = (slide*)malloc(s * sizeof(slide));
    return slides;
}

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