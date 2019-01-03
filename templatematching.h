#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

const double OP_PRAG = 0.2;

bool overlapped_enough(corr_elem first, corr_elem second);

//-----------------Task 7-----------Template matching
bmpimage grayscaleimage(bmpimage image)
{
    unsigned i, j, t;
    int cursor;
    double aux;

    for(i = 0; i < image.img_height; i++)  {
        for(j = 0; j < image.img_width; j++)  {

            cursor = cursorpos(i, j, image.img_height, image.img_width);
            aux = 0.299 * image.pixel[cursor+2] + 0.587 * image.pixel[cursor+1] + 0.114*image.pixel[cursor];

            for (t = 0; t<=2 ; t++)
                image.pixel[cursor + t] = (unsigned char) aux;
        }
    }
    return image;
}
//-------------------------Task 7---------------------------
vector template_matching(bmpimage ibmp, bmpimage sab, double prag,unsigned int digit)
{
    ibmp = grayscaleimage(ibmp);
    sab = grayscaleimage(sab);

    unsigned int i, j, i1, j1;
    double sablon_average = 0;
    double sablon_deviation = 0;

    vector arr = vectorinitialise();

    for (i = 0 ; i< 15; i++)
        for (j = 0; j< 11 ; j++)
            sablon_average += sab.pixel[cursorpos(i, j,15,11)];
    sablon_average /= PIX_NUMBER;

    for (i = 0 ; i< 15; i++)
        for (j = 0; j< 11 ; j++)
            sablon_deviation += (sab.pixel[cursorpos(i, j,15, 11)] - sablon_average) * (sab.pixel[cursorpos(i,j,15, 11)] - sablon_average);

    sablon_deviation /= (PIX_NUMBER-1);
    sablon_deviation  = sqrt((double)sablon_deviation);

    for (i = 0; i + 14 < ibmp.img_height ; i++)
        for (j = 0 ; j + 10 < ibmp.img_width ; j++) {
            double corr = 0;
            double devf = 0;
            double faverage = 0 ;

            for (i1 = 0; i1 <= 14; i1++)
                for (j1 = 0; j1 <= 10 ; j1++)

            faverage += ibmp.pixel[cursorpos(i + i1, j + j1,
                                             ibmp.img_height, ibmp.img_width)];

            faverage /= PIX_NUMBER;

            for (i1 = 0; i1 <= 14; i1++)
                for (j1 = 0; j1 <= 10 ; j1++)
                    devf += (ibmp.pixel[cursorpos(i + i1, j + j1 , ibmp.img_height, ibmp.img_width)] - faverage) *
                            (ibmp.pixel[cursorpos(i + i1, j + j1 , ibmp.img_height, ibmp.img_width)] - faverage);

            devf /= (PIX_NUMBER-1);
            devf =  sqrt ((double)devf);

            for (i1 = 0; i1 <= 14; i1++)
                for (j1 = 0; j1 <= 10 ; j1++) {
                    corr += (ibmp.pixel[cursorpos(i + i1, j + j1 , ibmp.img_height, ibmp.img_width)] - faverage) / devf*
                            (sab.pixel[cursorpos(i1, j1, 15, 11)] - sablon_average) / sablon_deviation ;

                }
            corr /= PIX_NUMBER;
            if (corr >= prag)
                push_back(&arr, i, j, corr, digit);
        }
    free(sab.pixel);
    return arr;
}
//-------------------------------------Task 8------------------
void contouring(bmpimage *image, unsigned int x, unsigned int y, const unsigned char *rgb) {

    int i, j;
    for (i = 0; i <= 14 && x + i < image->img_height; i++) {
        if (y - 1 >= 0) {
            image->pixel[cursorpos(x + i, y - 1, image->img_height, image->img_width)] = rgb[2];
            image->pixel[cursorpos(x + i, y - 1, image->img_height, image->img_width)+1] = rgb[1];
            image->pixel[cursorpos(x + i, y - 1, image->img_height, image->img_width)+2] = rgb[0];
        }
        if (y + 11 < image->img_width) {
            image->pixel[cursorpos(x + i, y + 11, image->img_height, image->img_width)] = rgb[2];
            image->pixel[cursorpos(x + i, y + 11, image->img_height, image->img_width)+1] = rgb[1];
            image->pixel[cursorpos(x + i, y + 11, image->img_height, image->img_width)+2] = rgb[0];
        }
    }
    for (j = 0; j <= 10 && y + j < image->img_width; j++) {
        if (x - 1 >= 0) {
            image->pixel[cursorpos(x - 1, y + j, image->img_height, image->img_width)] = rgb[2];
            image->pixel[cursorpos(x - 1, y + j, image->img_height, image->img_width)+1] = rgb[1];
            image->pixel[cursorpos(x - 1, y + j, image->img_height, image->img_width)+2] = rgb[0];
        }
        if (x + 15 < image->img_height) {
            image->pixel[cursorpos(x + 15, y + j, image->img_height, image->img_width)] = rgb[2];
            image->pixel[cursorpos(x + 15, y + j, image->img_height, image->img_width)+1] = rgb[1];
            image->pixel[cursorpos(x + 15, y + j, image->img_height, image->img_width)+2] = rgb[0];
        }
    }
}
//--------------------Task 9-------------------

int compare(const void *a,const void *b)
{
    corr_elem * p = (corr_elem *) a;
    corr_elem * q = (corr_elem *) b;
    return (int) (10000 * ((q->correlation) - (p->correlation)));
}
void sortingthematches(vector *arr) {

    //Quicksorting
    qsort(arr->elem, arr->size, sizeof(corr_elem), compare);
}
//----------------Task 10-------------
vector nonmax_elimination(vector arr) { 

    vector sol_arr = vectorinitialise();
    //Using a binary vector, 0 meaning still on the vector, 1 taken out;
    char *off = calloc (arr.size, sizeof(char));

    int i, j;
    for (i = 0; i < arr.size; i++)
        if (off[i] == 0) {
            push_back(&sol_arr, arr.elem[i].px, arr.elem[i].py,
                    arr.elem[i].correlation, arr.elem[i].digit);
            //Copying the solutions to the other vector
            for (j = i + 1 ; j < arr.size ; j++)
                if (overlapped_enough(arr.elem[i], arr.elem[j]))
                    off[j] = 1;
            //If overlapped enough, and i being a solution, then j becomes redundant
        }
    free(off);
    free(arr.elem);
    return sol_arr;
}
bool overlapped_enough(corr_elem first, corr_elem second) {

    //If rectangles to far away, on the Ox, or Oy, then overlap = 0
    if (first.px + 14 < second.px || second.px + 14 < first.px)
        return false;
    if (first.py + 10 < second.py || second.py + 10 < first.py)
        return false;
    //Otherwise, we create a new rectangle, the intersection rectangle
    //Left is the smallest Ox in this rectangle
    int left = first.px;
    if (second.px > left)
        left = second.px;
    //Right is the largest Ox in this rectangle
    int right = first.px + 14;
    if (second.px + 14 < right)
        right = second.px +14;
    //Top is the smallest Oy in this rectangle
    int top = first.py;
    if (second.py > top)
        top = second.py;
    //Bottom is the biggest Oy in this rectangle
    int bottom = first.py + 10;
    if (second.py + 10 < bottom)
        bottom = second.py + 10;
    //As follows, the intersection area is the area of this rectangle
    //This is calculated as width * height
    int intersection = (right - left + 1) * (bottom - top +1);
    //Returning true if the given coefficient is larger then OP_PRAG, a constant of 0.2
    return ((1.0 * intersection / (2 * PIX_NUMBER - intersection))>OP_PRAG);
}
