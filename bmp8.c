/*
**===============================================================
**  File        : bmp24.c
**  Author      : Giannini Loic and Benoît Tailhades
**  Created on  : 04/2025
**  Description : This file contains all the definitions of functions that aim to manage the filters and processing of gray scale  images
**
**===============================================================
*/
#include "bmp8.h"
#include "bmp24.h"
#include <math.h>


t_bmp8 * bmp8_loadImage(const char * filename) {
    //takes a string of char as input name  and output
    //header,width, height, color_depth, color table data size and data. All of those elements are stored in a bmp8 structure
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf( "Error opening file %s\n", filename);
        return NULL;
    }
    t_bmp8* bmp8 = (t_bmp8*)malloc(sizeof(t_bmp8));

    // lecture du header de l'image
    fread(bmp8->header, sizeof(unsigned char), 54, file);


    bmp8->width  = *(int*)&bmp8->header[18];

    bmp8->height = *(int*)&bmp8->header[22];
    bmp8->colorDepth = *(short*)&bmp8->header[28];

    if (bmp8->colorDepth != 8) {
        fclose(file);
        return NULL;
    }

    fread(bmp8->colorTable, sizeof(unsigned char), 1024, file);


    bmp8->dataSize = bmp8->width * bmp8->height;

    bmp8->data = malloc(sizeof(unsigned char) * bmp8->dataSize);
    if (bmp8->data == NULL) {
        printf("Error allocating memory\n");
        fclose(file);
        return NULL;
    }

    fread(bmp8->data, sizeof(unsigned char), bmp8->dataSize, file);

    fclose(file);
    printf("Image successfully loaded!\n");

    return bmp8;
}

void bmp8_saveImage(const char * filename, t_bmp8 * img){
    //Take as input and image of type bmp8(structure).
    // Open a file in write mode then write all informations(well formated) contained in the structure


  FILE* file = fopen(filename, "wb");
  if (file == NULL) {
    printf("error opening file %s\n", filename);
    return;
  }
  printf("Saving image to %s\n", filename);


  if(fwrite(img->header, sizeof(unsigned char), 54, file) != 54){
      printf("error saving image header\n");
      fclose(file);
      return;
  }
  if(fwrite(img->colorTable, sizeof(unsigned char), 1024, file) != 1024){
    printf("error saving image color table\n");
    fclose(file);
    return;
  }
  if(fwrite(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize){
    printf("error saving image data\n");
    fclose(file);
    return;
  }

  printf("Image saved successfully\n");


  fclose(file);

}

void bmp8_free(t_bmp8 * img){
    //takes an image of type bmp8(structure) and release the memory block pointed to by img-> data
  if (img){
      free(img->data);
      free(img);
      return;
  }
  printf( "error pointer null\n");

}

void bmp8_printInfo(t_bmp8 *img, bool verbose){
//takes an image of type bmp8( structure) and print all data stored in this structure
    printf("____________Image info:____________\n\n");
    printf("width = %d\n", img->width);
    printf("height = %d\n", img->height);
    printf("color depth = %d\n", img->colorDepth);
    printf("data size = %d\n", img->dataSize);

    if(verbose){
      int index;
      printf("color table:\n");
      for (int i = 0; i < 256; i++){
        index = i*4;
        printf("rgb(%d, %d, %d) @ %d\n", img->colorTable[index+2], img->colorTable[index+1], img->colorTable[index], index);
      }
    }
}

void bmp8_negative(t_bmp8 * img){
    // takes as input an image of type bmp8(structure)
    //For every pixel contained in the image and inverse it's color which means black goes to white and vice versa
    // For example black is 255. Hence we compute 255 - 255 which makes white = 0
    if(!img){
      printf("error pointer null\n");
      return;
    }
    int inverse;
    int index;
    for (int i = 0; i < 256; i++) {
        index = i*4;
        inverse = 255 - img->colorTable[index];


        img->colorTable[index] = inverse;
        img->colorTable[index+1] = inverse;
        img->colorTable[index+2] = inverse;

    }
}

void bmp8_brightness(t_bmp8 * img, int value){
    // takes as input an image of type bmp8(structure) and a int value
    //the aim is to add a certain value amount of brightness.
    //to do so we add the value to each pixel's color. Of course if the value + the already existing color is over 255 then it remains 255


    if(!img){
        printf("error pointer null\n");
        return;
    }


    int mod;
    int index;
    for (int i = 0; i < 256; i++) {
        index = i*4;
        mod = img->colorTable[index] + value > 255 ? 255 : img->colorTable[index] + value;

        img->colorTable[index] = mod;
        img->colorTable[index+1] = mod;
        img->colorTable[index+2] = mod;

    }


}

void bmp8_threshold(t_bmp8 * img, int threshold){
    // takes as input an image of type bmp8(structure) and a int threshold
    //for every pixel: if the pixel's color is above the threshold then the pixel is turrned to white(0)

    if(!img){
        printf("error pointer null\n");
        return;
    }
    int mod;
    int index = 0;
    for (int i = 0; i < 256; i++) {
        index = i*4;
        mod = img->colorTable[index] > threshold ? 255 : 0;


        img->colorTable[index] = mod;
        img->colorTable[index+1] = mod;
        img->colorTable[index+2] = mod;

    }

}

void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize{
    // takes as input an image of type bmp8(structure) a pointer to a pointer pointing to float and an int kernel size
    //First make sure that the kernel and image are valid. kernel must be odd
    //Then apply average based formulas for every pixel.
    if(!img){
        printf("error pointer null\n");
        return;
    }
    if(!kernel){
        printf("error kernel null\n");
        return;
    }
    if (kernelSize % 2 == 0){
        printf("kernel size must be odd\n");
        return;
    }

    // Allocate memory for the new image
    unsigned char* newImage = (unsigned char*)malloc(img->dataSize);
    if (!newImage) {
        printf("memory allocation failed\n");
        return;
    }

    int offset = kernelSize / 2;

    // Apply the filter to each pixel
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            float sum = 0.0f;

            // Apply kernel to the neighborhood
            for (int ky = 0; ky < kernelSize; ky++) {
                for (int kx = 0; kx < kernelSize; kx++) {
                    // Calculate corresponding image coordinates
                    int imgX = x + kx - offset;
                    int imgY = y + ky - offset;

                    // Handle border conditions (zero padding)
                    if (imgX >= 0 && imgX < img->width && imgY >= 0 && imgY < img->height) {
                        int index = imgY * img->width + imgX;
                        sum += img->data[index] * kernel[ky][kx];
                    }
                }
            }

            // Clamp the result to valid pixel values
            int result = (int)sum;
            result = result < 0 ? 0 : (result > 255 ? 255 : result);

            // Set the output pixel
            int index = y * img->width + x;
            newImage[index] = (unsigned char)result;
        }
    }

    // Copy the new image back to the original
    memcpy(img->data, newImage, img->dataSize);

    // Free the temporary image
    free(newImage);
}

unsigned int * bmp8_computeHistogram(t_bmp8 * img) {

    unsigned int * histogram = calloc(256, sizeof(unsigned int));
    for (int i=0; i < img->width*img->height; i++) {
        histogram[img->data[i]]++;
    }

    //for (int c=0; c < 256; c++) printf("histogram[%d]: %d\n",c ,histogram[c]);
    return histogram;
}

unsigned int * bmp8_computeCDF(unsigned int * hist) {
    //takes as input pointer to an int hist.
    //the aim of this function is to compute the CDF (Cumulative Distribution Function)
    //it is very useful to detect a "too black" or "too white" image and build a better contrasted image

    unsigned int * cdf = calloc(256, sizeof(unsigned int));
    int cdfMin = 0;
    int N = 0;
    for (int i=0; i < 256; i++) {
        for (int j=0; j<i+1; j++) {
            cdf[i] += (int)hist[j];
        }
        if (cdf[i] > 0) {
            cdfMin = (int)cdf[i];
            N = (int)cdf[i];
        }
    }

    for (int i=0; i < 256; i++) {
        if (cdf[i] < cdfMin && cdf[i] > 0) {
            cdfMin = (int)cdf[i];
        }
    }
    for (int i=0; i < 256; i++) {
        cdf[i] = (int)round((float)(cdf[i]-cdfMin)/(N-cdfMin)*255);
        //printf("cdf[%d] = %d\n", i, cdf[i]);
    }
    free(hist);
    return cdf;
}

void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq) {
// Takes as input an image of type bmp8(structure) and a pointer to an int hist_eq
    // just replace the original image by the equalized one . Which means better contrast
    for (int i=0; i < img->dataSize; i++) {
        img->data[i] = hist_eq[img->data[i]];
    }
    free(hist_eq);
}