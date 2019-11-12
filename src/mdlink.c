#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdlink.h"


mdlink *newLink() {
    mdlink *l;
    l = malloc(sizeof(mdlink));
    memset(l, 0, sizeof(*l));
    l->index = 0;
    l->next = NULL;
    return l;
}

mdlink *nextLink(mdlink *prev) {
    mdlink *n;
    n = malloc(sizeof(mdlink));
    memset(n, 0, sizeof(*n));
    prev->next = n;
    n->index = prev->index+1;
    n->next = NULL;
    return n;
}

mdlink *appendLink(mdlink *first) {
    mdlink *l = first;
    mdlink *next = first->next;
    while (next) {
        l = next;
        next = l->next;
    }
    return nextLink(l);
}

void freeLinks(mdlink *first) {
    free(first);
}
