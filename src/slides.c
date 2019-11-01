#include <stdio.h>
#include <stdlib.h>
#include "slides.h"
#include <string.h>

slide* createSlideArray(int numOfSlides) {
    slide* slides = malloc(sizeof(struct slide)*numOfSlides);
    return slides;
}

line *nextLine(line *prev) {
    line *n = malloc(sizeof(line));
    memset(n, 0, sizeof(*n));
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
        memset(l, 0, sizeof(*l));
        free(l);
        l = next;
    }
}
