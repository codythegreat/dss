#if !defined( SLIDES_H )
#define SLIDES_H
#include "mdlink.h"

typedef struct slide
{
    int number;
    int maxX;
    int y;
    int colorPair;
    struct line *first;
    struct slide *next;
    struct slide *prev;
    struct mdlink *link;
} slide;

typedef struct line {
    int colorPair;
    struct line *prev;
    struct line *next;
    size_t contentLength;
    char content[];
} line;

slide *newSlide();
slide *nextSlide(slide *prev);
line *newLine(char *buffer, size_t length);
void freeSlides(slide *s);
void freeLines(line *first);

#endif
