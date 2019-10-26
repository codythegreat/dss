#if !defined( PARSER_H )
#define PARSER_H

typedef struct
{
    char content[5000];
    int number;
    int r, g, b;
} Slide;

Slide* parseTXT(FILE *inFile, int* slideCounter, char *presTitle);

#endif
