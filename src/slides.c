#include <stdio.h>
#include <stdlib.h>
#include "slides.h"
#include <string.h>

slide *newSlide() {
    slide *s = malloc(sizeof(slide));
    memset(s, 0, sizeof(*s));
    s->link = NULL;
    return s;
}

slide *nextSlide(slide *prev) {
    slide *n = malloc(sizeof(slide));
    memset(n, 0, sizeof(*n));
    n->prev = prev;
    prev->next = n;
    n->next = NULL;
    n->link = NULL;
    return n;
}

line *newLine(char *buffer, size_t length) {
    line *l = malloc(sizeof(line) + sizeof(char) * length);
    l->colorPair = 0;
    l->prev = NULL;
    l->next = NULL;
    l->contentLength = length;
    strncpy(l->content, buffer, length);
    l->content[length - 1] = '\0';
    return l;
}

void freeSlides(slide *s) {
    slide *next;
    while (s) {
        next = s->next;
        memset(s, 0, sizeof(*s));
        free(s);
        s = next;
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
