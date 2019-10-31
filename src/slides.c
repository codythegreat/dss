#include <stdio.h>
#include <stdlib.h>
#include "slides.h"

slide* createSlideArray(int s) {
    slide* slides = malloc(sizeof(struct slide)*s);
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
    line *next;
    while(l) {
        next = l->next;
        free(l);
        l = next;
    }
}
