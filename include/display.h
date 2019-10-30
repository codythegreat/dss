#if !defined( DISPLAY_H )
#define DISPLAY_H
#include "parser.h"

void initDisplay();
int displayLoop(slide slides[], int* slideNumber, int* slideCount, char* title, char* fileName);

#endif
