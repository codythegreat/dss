#if !defined( DISPLAY_H )
#define DISPLAY_H
#include "parser.h"

void initDisplay();
int displayLoop(slide *curSlide, int* slideNumber, int* slideCount, char* title, char* fileName);

#endif
