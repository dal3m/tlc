#ifndef MAP_H
#define MAP_H

#include "uthash.h"

typedef struct 
{
	long long id; /* long long because that's what janasson wants, i could cast */
	char name[64];
	UT_hash_handle hh;

} Element;

/* this kinda sucks */
Element *subjects = NULL;
Element *categories = NULL;

void 
add(long long id, const char *name, Element **type)
{
    Element *s = malloc(sizeof(Element));
    s->id = id;
    // strncpy(s->name, name, sizeof(s->name));    
    snprintf(s->name, sizeof(s->name), "%s", name);
    HASH_ADD(hh, *type, id, sizeof(long long), s);
}

const char* 
look(long long id, Element *type)
{
    Element *s;
    HASH_FIND(hh, type, &id, sizeof(long long), s);
    return s ? s->name : NULL;
}


void
freemap(Element* type)
{
	Element *cur, *tmp;

	HASH_ITER(hh, type, cur, tmp)
	{
		HASH_DEL(type, cur);
		free(cur);
	}
}

#endif
