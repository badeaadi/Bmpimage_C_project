//Badea Adrian Cătălin , 135
//Task 1-6 header file
#include <stdio.h>
#include <stdlib.h>

//A generator for randomized numbers using xor and shift
//---------------Task 1-------------
unsigned int xorshift32(unsigned int last)
{
	unsigned int x = last;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return x;
}


//-------------------Task 3------------
unsigned int* creatersequence(unsigned int secretkey,unsigned int rnumber) {

    int i;
    //Allocation of rnumber unsigned ints, in order to contain the randomized sequence
    unsigned int *rseq = malloc(rnumber*  sizeof(unsigned int));
    //Creating the array with the seed, from the secretfile
    rseq[0] = secretkey;
    //Creating the sequence using the xorshift32 generator
    for (i = 1; i < rnumber ; i++)
        rseq[i] = xorshift32(rseq[i-1]);
    return rseq;
}
unsigned int* createpermutation(const unsigned int *randoms, unsigned int permsize)
{
    //Create permutation based on the given randoms, and the permutation size
    unsigned int i, j;
    //Allocation of permsize unsigned ints, in order to
    unsigned int *perm = malloc(permsize * sizeof(unsigned int));
    for (i = 0; i < permsize; i++)
        perm[i] = i;
    //Using the Durstenfeld algorithm to create a properly pseudo-randomised sequence
    unsigned int cursor = 1;
    for (i = permsize - 1; i > 0; --i) {
        j = (randoms[cursor++] % (i+1));

        unsigned int aux = perm[j];
        perm[j] = perm[i];
        perm[i] = aux;
    }
    return perm;
}
unsigned int* inversateperm(const unsigned int *perm, unsigned int permsize)
{
    unsigned int i;
    unsigned int *invperm = malloc(permsize * sizeof(unsigned int));
    for (i = 0; i < permsize; i++)
        invperm[perm[i]] = i;

    return invperm;
}
pix xoratepixelint(pix p,unsigned int x)
{
    pix pix_new;

    unsigned char x0 = (unsigned char) (x & MASK);
    x>>=8;
    unsigned char x1 = (unsigned char) (x & MASK);
    x>>=8;
    unsigned char x2 = (unsigned char) (x & MASK);

    pix_new.blue = p.blue ^ x0;
    pix_new.green = p.green ^ x1;
    pix_new.red = p.red ^ x2;
    return pix_new;
}
pix xoratepixels(pix p1, pix p2)
{
    pix newpix;
    newpix.blue = p1.blue ^ p2.blue;
    newpix.green = p1.green ^ p2.green;
    newpix.red = p1.red ^ p2.red;
    return newpix;
}
void encrypt_image(char *filesource, char *fileout, char *secretfile) {

    FILE *secret;
    bmpimage image = matrix_linearisation(filesource);
    secret= fopen(secretfile, "r");

    unsigned int secretkey;
    fscanf(secret, "%u", &secretkey);

    unsigned int *rseq = creatersequence(secretkey, image.img_width * image.img_height *2 );

    unsigned int *perm = createpermutation(rseq, image.img_width * image.img_height);

    bmpimage encrypted = createbmp(image.h, image.img_size, image.img_width,image.img_height);
    encrypted.pixel = (unsigned char*) malloc (image.img_size * 3 * sizeof(unsigned char));

    int i ;
    for (i = 0 ; i< image.img_width * image.img_height ; i++) {
        int cursor = 3 * perm[i];
        int cursor2 = 3 * i;
        encrypted.pixel[cursor++] = image.pixel [cursor2++];
        encrypted.pixel[cursor++] = image.pixel [cursor2++];
        encrypted.pixel[cursor++] = image.pixel [cursor2++];
    }
    unsigned int starting_value = 0;
    fscanf(secret, "%u", &starting_value);

    pix aux,aux2;

    aux.blue = encrypted.pixel[0];
    aux.green = encrypted.pixel[1];
    aux.red = encrypted.pixel[2];

    aux = xoratepixelint(aux, starting_value);
    aux = xoratepixelint(aux, rseq[image.img_width * image.img_height] );

    aux2 =  aux;

    encrypted.pixel[0] = aux.blue;
    encrypted.pixel[1] = aux.green;
    encrypted.pixel[2] = aux.red;

    for (i = 1; i< image.img_width * image.img_height ; i++) {
        int cursor = 3 * i;
        aux.blue = encrypted.pixel[cursor++];
        aux.green = encrypted.pixel[cursor++];
        aux.red = encrypted.pixel[cursor++];
        aux = xoratepixels(aux, aux2);
        aux = xoratepixelint(aux, rseq[image.img_width * image.img_height + i]);

        cursor-=3;
        encrypted.pixel[cursor++] = aux.blue;
        encrypted.pixel[cursor++] = aux.green;
        encrypted.pixel[cursor++] = aux.red;

        aux2 = aux;
    }
    save_linearisation(encrypted, fileout);
    free(rseq);
    free(perm);
    free(image.pixel);
    free(encrypted.pixel);
    fclose(secret);
}
void decrypt_image(char *filesource, char *fileout, char *secretfile) {

    FILE *secret;
    bmpimage image = matrix_linearisation(filesource);

    unsigned int secretkey, starting_value;

    secret = fopen(secretfile, "r");
    fscanf(secret, "%u", &secretkey);
    fscanf(secret, "%u", &starting_value);

    unsigned int *rseq = creatersequence(secretkey, image.img_width * image.img_height * 2);

    unsigned int *perm = createpermutation(rseq, image.img_width * image.img_height);
    int i;
    pix aux, aux2;

    for (i = image.img_height * image.img_width - 1; i >= 0; i--) {
        int cursor = 3 * i;
        int cursor2 = 3 * (i - 1);

        aux.blue = image.pixel[cursor++];
        aux.green = image.pixel[cursor++];
        aux.red = image.pixel[cursor++];

        aux = xoratepixelint(aux, rseq[image.img_height * image.img_width + i]);

        if (i) {
            aux2.blue = image.pixel[cursor2++];
            aux2.green = image.pixel[cursor2++];
            aux2.red = image.pixel[cursor2++];

            aux = xoratepixels(aux, aux2);
        } else
            aux = xoratepixelint(aux, starting_value);

        cursor -= 3;
        image.pixel[cursor++] = aux.blue;
        image.pixel[cursor++] = aux.green;
        image.pixel[cursor++] = aux.red;
    }
    bmpimage decrypted = createbmp(image.h, image.img_size, image.img_width, image.img_height);
    decrypted.pixel = (unsigned char*) malloc (image.img_size * 3 * sizeof(unsigned char));

    unsigned int *invperm = inversateperm(perm, image.img_width * image.img_height);

    for (i = 0; i < image.img_width * image.img_height; i++) {
        int cursor = 3 * invperm[i];
        int cursor2 = 3 * i;
        decrypted.pixel[cursor++] = image.pixel[cursor2++];
        decrypted.pixel[cursor++] = image.pixel[cursor2++];
        decrypted.pixel[cursor++] = image.pixel[cursor2++];
    }
    save_linearisation(decrypted,fileout);
    free(rseq);
    free(perm);
    free(invperm);
    free(image.pixel);
    free(decrypted.pixel);
    fclose(secret);
}
void chi_function(char *filesource)
{
    bmpimage image = matrix_linearisation(filesource);
    int i,j;
    double average = (image.img_width * image.img_height)/256.0;

    double bgr[3];
    bgr[0]=  bgr[1] = bgr[2] = 0;
    for (j = 0 ; j<= 2; j++) {

        unsigned int *color = malloc(sizeof(unsigned int) * 256);
        for (i = 0; i<= 255; i++)
            color[i]=0;

        for (i = 0; i < image.img_height * image.img_width; i++)
            color[image.pixel[3 * i + j]]++;

        for (i = 0 ; i<=255 ; i++) {
            bgr[j] += (color[i]-average)*(color[i]-average)/average;
            color[i] = 0;
        }
        free(color);
    }
    printf("%.2f %.2f %.2f\n",bgr[2],bgr[1],bgr[0]);
    free(image.pixel);
}