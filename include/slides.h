#if !defined( SLIDES_H )
#define SLIDES_H

typedef struct slide
{
    int number;
    int maxX;
    int y;
    int colorPair;
    struct line *first;
    struct slide *next;
    struct slide *prev;
} slide;

typedef struct line {
    char content[256]; // number subject to change
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
