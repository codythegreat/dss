#if !defined( PARSER_H )
#define PARSER_H

typedef struct
{
    const char *title;
    const char *content;
    int number;
    int r, g, b;
} Slide;

void parseArea(char *buffer, int* xVar, int* yVar);
Slide *parseTXT(FILE *inFile);

#endif
