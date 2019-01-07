//Creation of structures
#include <stdlib.h>
#include <string.h>
#define header_size 54
//Unsigned char with only bytes of 1
const unsigned char MASK = (1<<8) -1 ;
//Constant number of a certain number of pixels
const unsigned int PIX_NUMBER  = 11 * 15;

typedef struct{
    unsigned char header_info[header_size];
} header;
typedef struct {
    unsigned char blue, green, red;
} pix;
//Pixel type of a bitmap image in BGR order)
typedef struct {
    unsigned int img_size, img_width, img_height;
    header h;
    unsigned char *pixel;
} bmpimage;


int cursorpos(unsigned int lin,unsigned int col,unsigned int height,unsigned int width)
{
    return 3 * (width * lin + col);
}

//-----------------Task 2---------------
// Initialization of bmp image using a function
bmpimage createbmp(header given_header, unsigned int size, unsigned int width, unsigned int  height)
{
    bmpimage obj;
    obj.h = given_header;
    obj.img_size = size;
    obj.img_width = width;
    obj.img_height = height;
    return obj;
}
// Linearisation of a matrix, having as parameter the source file of the matrix
bmpimage matrix_linearisation(char *filename) {

    FILE *fin; //pointer to input file
    unsigned int img_size, img_width, img_height; //Size, width and height of the image
    unsigned char pixrgb[3]; //auxiliar vector for storing three pixels at a time
    header header_curr; //current header of the image in question

    fin = fopen(filename, "rb"); // Opening the file for binary reading

    fseek(fin, 0, SEEK_SET);
    int i = 0, j;
    for (i = 0 ; i<= 53 ; i++)
        fread(&header_curr.header_info[i], sizeof(unsigned char), 1, fin);

    fseek(fin, 2, SEEK_SET);
    fread(&img_size, sizeof(unsigned int), 1, fin);

    //Reading width and height, which are found at the 18th byte in the header
    img_width = 0;  //Width
    for (i = 21 ; i >= 18; i--)
          img_width = img_width * 256 + header_curr.header_info[i];

    img_height = 0; //Height
    for (i = 25 ; i >= 22; i--)
          img_height = img_height * 256 + header_curr.header_info[i];

    //Moving the cursor after the header
    fseek(fin,54,SEEK_SET);
    //Padding at the end of each line
    int padding = (3 * img_width) % 4;
    if (padding)
        padding = 4 - padding;

    //Transforms the input data in internal storage
    bmpimage newimage = createbmp(header_curr, img_size, img_width, img_height) ;
    //Allocating 3 * height * width pixels
    newimage.pixel = malloc(3* img_height * img_width * sizeof(unsigned char));

    int cursor=0;
    //Reading from the (h-1, 0) pixel, as in the bmp format
    for (i = img_height - 1; i >= 0 ; i--) {

        for (j = 0; j < img_width; ++j) {
            fread(&pixrgb, 3, 1, fin);
            cursor = cursorpos(i, j, img_height, img_width);
            //BGR reading
            newimage.pixel[cursor++]=pixrgb[0];
            newimage.pixel[cursor++]=pixrgb[1];
            newimage.pixel[cursor]=pixrgb[2];
        }
        fseek(fin,padding,SEEK_CUR);
    }
    fclose(fin);
    return newimage;
}
//---------------------------Task 3--------------
//Saving the linearisation of a bmp image to a certain file
void save_linearisation(bmpimage img, char *filename)
{
    FILE *fout;
    fout = fopen(filename, "wb+");
    int i;
    //Writing the header information
    for (i= 0; i <= 53; i++)
        fwrite(&img.h.header_info[i], 1, 1, fout);
    unsigned char pixrgb[3];
    unsigned char null_char = 0;

    //The padding is equal to the quantity of bytes needed at the end of each line in order to make it
    //divisible by 4
    int padding = (3 * img.img_width) % 4;
    if (padding)
        padding = 4 - padding;

    int j, cursor = 0;
    //Writing the image, pixel by pixel;
    for (i = img.img_height - 1; i >=0 ; i--) {
        for (j = 0; j < img.img_width; ++j) {
            cursor = cursorpos((unsigned int) i, (unsigned int) j, img.img_height, img.img_width);

            pixrgb[0] = img.pixel[cursor++]; //B
            pixrgb[1] = img.pixel[cursor++]; //G
            pixrgb[2] = img.pixel[cursor++]; //R
            //Writing the pixels with the fwrite, function which takes as parameters the address,
            //the number of pixels and the cursor
            fwrite(&pixrgb, 3, 1, fout);
        }
        int j = padding;
        while (j) {
            fwrite(&null_char,1,1,fout);
            j--;
        }
    }
    //Closing the locations file
    fclose(fout);
}