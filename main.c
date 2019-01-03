#include <stdio.h>
#include "vector.h"
#include "bmp.h"
#include "encryption.h"
#include "templatematching.h"
const double PRAG = 0.5;
//Inclusion of four headers within the project


//----------------------Task 11------------------
int main()  {

    //Opening the locations.txt textfile
    FILE *locations = fopen("locations.txt", "r");

    //Reading the location of the image
    char *image_location = malloc(30 * sizeof(char));
    fscanf(locations,"%s\n",image_location);

    //Reading the encryption location
    char *encryptedimage_location = malloc(30 * sizeof(char));
    fscanf(locations,"%s\n",encryptedimage_location);

    //Reading the secretfile location, prior to reading the seed and the random key
    char *secretfile_location = malloc(30 * sizeof(char));
    fscanf(locations,"%s\n", secretfile_location);

    //Encrypting the image from the location, to the location desired
    encrypt_image(image_location, encryptedimage_location, secretfile_location);

    //Reading the location desired for the decryption
    char *decryptedimage_location = malloc(30* sizeof(char));
    fscanf(locations,"%s\n",decryptedimage_location);

    //Decrypting the image at the desired location
    decrypt_image(encryptedimage_location, decryptedimage_location, secretfile_location);

    //Applying the chi function to both the first image and its encryption
    chi_function(image_location);
    chi_function(encryptedimage_location);
    //First phase of the project DONE

    //Allocating memory for the locations of the 10 patterns
    char *cifra = malloc(30 * sizeof(char));
    int i, j;
    //Reading the location of the test
    char *test_location = malloc(30* sizeof(char));
    fscanf(locations,"%s\n",test_location);
    bmpimage image = matrix_linearisation(test_location);
    //Reading the location for the detection
    char *detection_location = malloc(30 * sizeof(char));
    fscanf(locations,"%s\n", detection_location);

    //Saving and reading the location of the file which will have present the amtches
    save_linearisation(image, detection_location);
    bmpimage detections = matrix_linearisation(detection_location);

    //Initialising the vector with the matches
    vector matches = vectorinitialise();

    for (i = 0; i <= 9 ; i++) {
        //Reading the location of the pattern of the digits
        memset(cifra,0,30 * sizeof(char));
        fscanf(locations,"%s\n", cifra);
        bmpimage sablon = matrix_linearisation(cifra);
        //Auxiliary vector for the arrays for each pattern
        vector matches_aux;
        matches_aux = template_matching(image, sablon, 0.5, (unsigned int) i);

        printf("CIFRA %d\n", i);
        for (j = 0; j< matches_aux.size; j++) {
            push_back(&matches, matches_aux.elem[j].px, matches_aux.elem[j].py,
                      matches_aux.elem[j].correlation, matches_aux.elem[j].digit);
        }
        free(matches_aux.elem);
    }
    //Sorting the array of matches(contained as a dinamically sized array)
    sortingthematches(&matches);

    for (i = 0 ; i < matches.size; i++)
        printf("%lf\n", matches.elem[i].correlation);
    //Eliminating the overlapped matches from the foudn array, using the 0.2 overlap index
    vector sols = nonmax_elimination(matches);

    unsigned char rgb[11][3];
    //Reading the color spectrum of the locations
    for (i = 0; i <= 9 ; i++)
        for (j = 0; j <= 2; j++)
            fscanf(locations, "%hhu", &rgb[i][j]);
    //Applying the contouring for the found matches and the given colors
    for (j = 0; j < sols.size; j++)
        contouring(&detections, sols.elem[j].px, sols.elem[j].py, rgb[sols.elem[j].digit]);
    //Saving the detections to the certain detection location
    save_linearisation(detections, detection_location);
    //Freeing the used memory
    free(sols.elem);
    free(image.pixel);
    free(detections.pixel);

    free(image_location);
    free(encryptedimage_location);
    free(decryptedimage_location);
    free(secretfile_location);
    free(detection_location);
    free(test_location);
    free(cifra);

    fclose(locations);
    return 0;
}