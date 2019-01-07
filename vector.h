//Dynamically sized array
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned int px,py; //Indexes of line and column in a matrix
    unsigned int digit; //Digit of correlation
    double correlation; //Correlation, a number greater then PRAG but lower than 1
} corr_elem ;
//Dynamically sized array, with size, capacity and pointer.
typedef struct{
    unsigned int size;  //Size of the array
    unsigned int cap;   //Capacity, always doubling - when size meets capacity
    corr_elem *elem; //Pointer
} vector;

//Push_back function on the vector, with pointer to the given struct and corr_elem elements given
void push_back(vector *t, unsigned int x, unsigned int y, double co, unsigned int digit)
{
    if ((t->size + 1) == t->cap) {
        //Doubling the capacty of the ector
        t->cap *= 2;
        //Reallocation of the array with double its capacity
        t->elem = realloc(t->elem, t->cap * sizeof(corr_elem));

    }
    //Adding another element  to the array in question
    t->elem[(t->size)].px = x;
    t->elem[(t->size)].py = y;
    t->elem[(t->size)].digit = digit;
    t->elem[(t->size)++].correlation = co;
}
//Initialising the vector
vector vectorinitialise()
{
    vector p;
    p.size = 0;
    p.cap = 1;
    p.elem = malloc(p.cap * sizeof(corr_elem));
    return p;
}