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
    char content[256]; // todo can this be dynamic?
    int colorPair;
    struct line *prev;
    struct line *next;
} line;

slide *newSlide();
slide *nextSlide(slide *prev);
line *newLine();
line *nextLine(line *prev);
void freeSlides(slide *s);
void freeLines(line *first);

#endif
