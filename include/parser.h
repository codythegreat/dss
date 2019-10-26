#if !defined( PARSER_H )
#define PARSER_H
#include "slides.h"

//typedef struct
//{
    //char content[5000];
    //int number;
    //int x, y, r, g, b;
//} Slide;

slide* parseTXT(FILE *inFile, int* slideCounter, char *presTitle);

#endif
