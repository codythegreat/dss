#if !defined( SLIDES_H )
#define SLIDES_H

typedef struct slide
{
    int number;
    int maxX;
    int y;
    int r, g, b;
    struct line *first;
} slide;

typedef struct line {
    char content[256]; // number subject to change
    int r, g, b;
    struct line *prev;
    struct line *next;
} line;

slide* createSlideArray(int s);
line *newLine();
line *nextLine(line *prev);
void freeSlides(slide *slide[]);
void freeLines(line *first);

#endif
