#if !defined( PARSER_H )
#define PARSER_H
#include "slides.h"
#include "mdlink.h"

slide* parseTXT(FILE *inFile, int* slideCounter, char *presTitle);

#endif
