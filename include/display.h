#if !defined( DISPLAY_H )
#define DISPLAY_H
#include "parser.h"

void setSlideCount(int* slides);
void initDisplay();
void displayLoop(slide slides[], int* slideNumber, char* title, char* fileName);

#endif
