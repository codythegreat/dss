#if !defined( MDLINK_H )
#define MDLINK_H

typedef struct mdlink
{
    int index;
    struct mdlink *next;
    int __urlOffset;
    char __storage[];
} mdlink;

mdlink *newLink(char *title, size_t titleLength, char *url, size_t urlLength);
void freeLinks(mdlink *first);

static inline
__attribute__((always_inline))
char *
linkGetTitle(mdlink *link) 
{
    return link->__storage;
}

static inline
__attribute__((always_inline))
char *
linkGetURL(mdlink *link) 
{
    return link->__storage + link->__urlOffset;
}

#endif
