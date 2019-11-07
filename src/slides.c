#include <stdio.h>
#include <stdlib.h>
#include "slides.h"
#include <string.h>

slide *newSlide() {
    slide *s = malloc(sizeof(slide));
    memset(s, 0, sizeof(*s));
    return s;
}

slide *nextSlide(slide *prev) {
    slide *n = malloc(sizeof(slide));
    memset(n, 0, sizeof(*n));
    n->prev = prev;
    prev->next = n;
    n->next = NULL;
    return n;
}

line *newLine() {
    line *l = malloc(sizeof(line));
    memset(l, 0, sizeof(*l));
    return l;
}

line *nextLine(line *prev) {
    line *n = malloc(sizeof(line));
    memset(n, 0, sizeof(*n));
    n->prev = prev;
    prev->next = n;
    n->next = NULL;
    return n;
}

void freeSlides(slide *s) {
    slide *next;
    while (next!=NULL) {
        next = s->next;
        memset(s, 0, sizeof(*s));
        free(s);
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
