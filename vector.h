//Creation of a structure with push_back function and reinitialization of memory
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned int px,py;
    unsigned int digit;
    double correlation;
} corr_elem ;

typedef struct{
    unsigned int size;
    unsigned int cap;
    corr_elem *elem;
} vector;

void push_back(vector *t, unsigned int x, unsigned int y, double co, unsigned int digit)
{
    if ((t->size + 1) == t->cap) {
        t->cap *= 2;
        t->elem = realloc(t->elem, t->cap * sizeof(corr_elem));
    }
    t->elem[(t->size)].px = x;
    t->elem[(t->size)].py = y;
    t->elem[(t->size)].digit = digit;
    t->elem[(t->size)++].correlation = co;
}

vector vectorinitialise()
{
    vector p;
    p.size = 0;
    p.cap = 1;
    p.elem = malloc(p.cap * sizeof(corr_elem));
    return p;
}