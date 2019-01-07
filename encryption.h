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


//-------------------Task 4------------
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
        //Choosing an index j to swap values with the one on index i
        unsigned int aux = perm[j];
        perm[j] = perm[i];
        perm[i] = aux;
    }
    //Returning the pointer to the permutation
    return perm;
}
//Inversating the permutation for decryption
unsigned int* inversateperm(const unsigned int *perm, unsigned int permsize)
{
    unsigned int i;
    //Allocation permsize unsigned ints to contain the permutation
    unsigned int *invperm = malloc(permsize * sizeof(unsigned int));
    //Making the inverse permutation from the first one
    for (i = 0; i < permsize; i++)
        invperm[perm[i]] = i;
    //Returning the inverse permutation
    return invperm;
}
//A function which xorates a pixel with an int
pix xoratepixelint(pix p,unsigned int x)
{
    pix pix_new;
    //Allocating another pixel
    unsigned char x0 = (unsigned char) (x & MASK);
    x>>=8;
    unsigned char x1 = (unsigned char) (x & MASK);
    x>>=8;
    unsigned char x2 = (unsigned char) (x & MASK);
    //Xorating the color spectrum of the given pixel with
    //groups of 8 pixels from the given unsigned int
    pix_new.blue = p.blue ^ x0;
    pix_new.green = p.green ^ x1;
    pix_new.red = p.red ^ x2;
    //Returning the newly formed pixel
    return pix_new;

}
//A function which xorates two pixels
pix xoratepixels(pix p1, pix p2)
{
    //Allocating a new pixel
    pix newpix;
    //Xorating the color spectrum of the pixel
    newpix.blue = p1.blue ^ p2.blue;
    newpix.green = p1.green ^ p2.green;
    newpix.red = p1.red ^ p2.red;
    return newpix;
}
//Encryption of the image from the filesource to the fileout, using the keys from the secretfile
void encrypt_image(char *filesource, char *fileout, char *secretfile) {

    FILE *secret;
    //Linearisation of the matrix from the filesource
    bmpimage image = matrix_linearisation(filesource);
    //Openinf for reading the secretfile
    secret= fopen(secretfile, "r");

    unsigned int secretkey;
    fscanf(secret, "%u", &secretkey);
    //Creating the random sequence using the function creatersequence, with 2 * n random numbers
    unsigned int *rseq = creatersequence(secretkey, image.img_width * image.img_height *2 );

    //Creating the permutation sequence using the function createpermutation
    unsigned int *perm = createpermutation(rseq, image.img_width * image.img_height);

    //Creating the encrypted file , using the initialisation function from above
    bmpimage encrypted = createbmp(image.h, image.img_size, image.img_width,image.img_height);

    //Allocating the encrypted file's pixel
    encrypted.pixel = (unsigned char*) malloc (image.img_size * 3 * sizeof(unsigned char));

    int i ;
    for (i = 0 ; i< image.img_width * image.img_height ; i++) {
        //Using the permutation created above to design the encryption pattern
        int cursor = 3 * perm[i];
        int cursor2 = 3 * i;
        encrypted.pixel[cursor++] = image.pixel [cursor2++];
        encrypted.pixel[cursor++] = image.pixel [cursor2++];
        encrypted.pixel[cursor++] = image.pixel [cursor2++];
    }
    //Initialisation and reading the starting value from the secretfile
    unsigned int starting_value = 0;
    fscanf(secret, "%u", &starting_value);
    //Using two auxiliary pixels
    pix aux,aux2;

    aux.blue = encrypted.pixel[0];
    aux.green = encrypted.pixel[1];
    aux.red = encrypted.pixel[2];
    //Xorating the auxiliary pixels
    aux = xoratepixelint(aux, starting_value);
    aux = xoratepixelint(aux, rseq[image.img_width * image.img_height] );

    aux2 =  aux;
    encrypted.pixel[0] = aux.blue;
    encrypted.pixel[1] = aux.green;
    encrypted.pixel[2] = aux.red;
    //Iterating through de cursor position in the encrypted image, which is, at the moment
    //only the permutation of pixels of the first one
    for (i = 1; i< image.img_width * image.img_height ; i++) {
        int cursor = 3 * i;
        //Copying the pixels from the permutated image to the auxiliary
        //in order to change them properly
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
    //Saving the linearisation from the encrypted image to the fileout
    save_linearisation(encrypted, fileout);
    //Freeing auxiliary memory, the randomized sequence of numbers, the permutation itself
    //The image and the encrypted image, as these are already saved in the external storage
    free(rseq);
    free(perm);
    free(image.pixel);
    free(encrypted.pixel);
    //Closing the secretfile
    fclose(secret);
}
//-----------------------Task 5---------------------
void decrypt_image(char *filesource, char *fileout, char *secretfile) {

    FILE *secret;
    //Reading the matrix from the filesource in its linearised form
    bmpimage image = matrix_linearisation(filesource);

    unsigned int secretkey, starting_value;

    secret = fopen(secretfile, "r");
    fscanf(secret, "%u", &secretkey);
    fscanf(secret, "%u", &starting_value);
    //Creating the same random sequence as the encryption, with the same secretkey
    unsigned int *rseq = creatersequence(secretkey, image.img_width * image.img_height * 2);
    //Creating the same permutation as the encryption, starting from the random sequence
    unsigned int *perm = createpermutation(rseq, image.img_width * image.img_height);
    int i;
    pix aux, aux2;
    //Xorating the pixels as in the encryption, but the other way around
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
    //Creating the inversated permutation of the one from the encryption
    unsigned int *invperm = inversateperm(perm, image.img_width * image.img_height);
    //Permutating the pixels with the inversate permutation
    for (i = 0; i < image.img_width * image.img_height; i++) {
        int cursor = 3 * invperm[i];
        int cursor2 = 3 * i;
        decrypted.pixel[cursor++] = image.pixel[cursor2++];
        decrypted.pixel[cursor++] = image.pixel[cursor2++];
        decrypted.pixel[cursor++] = image.pixel[cursor2++];
    }
    //Saving the decrypted image to fileout
    save_linearisation(decrypted,fileout);
    //Freeing the used memoriy, the random sequence, the permutation and the inversated permutation
    free(rseq);
    free(perm);
    free(invperm);
    free(image.pixel);
    free(decrypted.pixel);
    fclose(secret);
}
//--------------------Task 6---------
void chi_function(char *filesource)
{
    bmpimage image = matrix_linearisation(filesource);
    int i,j;
    //Calculating the normal average of every color in the image
    double average = (image.img_width * image.img_height)/256.0;

    double bgr[3];
    bgr[0]=  bgr[1] = bgr[2] = 0;
    for (j = 0 ; j<= 2; j++) {

        unsigned int *color = malloc(sizeof(unsigned int) * 256);
        for (i = 0; i<= 255; i++)
            color[i]=0;
        //Addition of every color of every pixel to the frequency array allocated above(color)
        for (i = 0; i < image.img_height * image.img_width; i++)
            color[image.pixel[3 * i + j]]++;

        for (i = 0 ; i<=255 ; i++) {
            bgr[j] += (color[i]-average)*(color[i]-average)/average;
            color[i] = 0;
        }
        free(color);
    }
    printf("R: %.2f\nG: %.2f\nB: %.2f\n",bgr[2],bgr[1],bgr[0]);
    free(image.pixel);
}