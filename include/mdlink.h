#if !defined( MDLINK_H )
#define MDLINK_H

typedef struct mdlink
{
    int index;
    char title[256];
    char url[1000];
    struct mdlink *next;
} mdlink;

mdlink *newLink();
mdlink *appendLink(mdlink *first);
void freeLinks(mdlink *first);

#endif
