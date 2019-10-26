#if !defined( DISPLAY_H )
#define DISPLAY_H
#include "parser.h"

void setSlideCount(int* slides);
void initDisplay();
void displayLoop(Slide slides[], int* slideNumber, char* title, char* fileName);

#endif
