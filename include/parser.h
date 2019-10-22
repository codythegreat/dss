#if !defined( PARSER_H )
#define PARSER_H

struct Slide
{
    const char *title;
    const char *content;
    int number;
    int r, g, b;
};

void parseArea(char *buffer, int* xVar, int* yVar);
struct Slide *parseTXT(FILE *inFile);

#endif
