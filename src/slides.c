#include <stdio.h>
#include <stdlib.h>
#include "slides.h"

slide* createSlideArray(int numOfSlides) {
    slide* slides = malloc(sizeof(struct slide)*numOfSlides);
    return slides;
}

line *nextLine(line *prev) {
    line *n = malloc(sizeof(line));
    n->prev = prev;
    prev->next = n;
    return n;
}

void freeSlides(slide *slide[], int numOfSlides) {
    int i;
    for (i=0;i<numOfSlides;i++) {
        free(slide[i]);
    }
}

void freeLines(line *l) {
    line *next;
    while(l) {
        next = l->next;
        free(l);
        l = next;
    }
}
