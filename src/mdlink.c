#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdlink.h"


mdlink *newLink(char *title, size_t titleLength, char *url, size_t urlLength) {
    mdlink *l;
    l = malloc(sizeof(mdlink) + sizeof(char) * (titleLength + urlLength + 3));
    l->index = 0;
    l->next = NULL;
    strncpy(l->__storage, title, titleLength);
    l->__storage[titleLength] = '\0';
    strncpy(l->__storage + titleLength + 1, url, urlLength);
    l->__urlOffset = titleLength + 1;
    l->__storage[titleLength + urlLength + 2] = '\0';
    return l;
}

void freeLinks(mdlink *l) {
    mdlink *next;
    while(l) {
        next = l->next;
        memset(l, 0, sizeof(*l));
        free(l);
        l = next;
    }
}
