/*
**===============================================================
**  File        : bmp24.c
**  Author      : Giannini Loic and Benoît Tailhades
**  Created on  : 04/2025
**  Description : This file contains all the definitions of functions that aim to manage the filters and processing of colored images
**
**===============================================================
*/

#include "bmp24.h"
#include <math.h>
#include "bmp8.h"


t_pixel ** bmp24_allocateDataPixels (int width, int height) {

    t_pixel ** pixels = (t_pixel**) malloc(height * sizeof(t_pixel*));
    for (int i = 0; i < height; i++) {
        pixels[i] = (t_pixel*) malloc(width * sizeof(t_pixel));
    }

    if (!pixels) {
        printf("Error in memory allocation of the pixel matrix\n");
        return NULL;
    }

    return pixels;
}
void bmp24_freeDataPixels (t_pixel ** pixels, int height) {

    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);

}
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth) {

    t_bmp24* bitmap = (t_bmp24*) malloc(sizeof(t_bmp24));

    if (!bitmap) {
        printf("Error in memory allocation of the bitmap\n");
        return NULL;
    }

    bitmap->width = width;
    bitmap->height = height;
    bitmap->colorDepth = colorDepth;
    bitmap->data = bmp24_allocateDataPixels(width, height);

    return bitmap;

}
void bmp24_free (t_bmp24 * img) {

    bmp24_freeDataPixels(img->data, img->height);
    free(img);

}
void file_rawRead (long position, void * buffer, uint32_t size, size_t n, FILE* file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}
void file_rawWrite (long position, void * buffer, uint32_t size, size_t n, FILE* file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}
void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file)
{
    int rowSize = ((image->width * 3 + 3)/4)*4;
    uint32_t position = image->header.offset + (image->height - 1 - y) * rowSize + x * 3;

    uint8_t pixelData[3];
    file_rawRead(position, pixelData, sizeof(uint8_t), 3, file);

    image->data[y][x].blue = pixelData[0];
    image->data[y][x].green = pixelData[1];
    image->data[y][x].red = pixelData[2];
}
void bmp24_readPixelData (t_bmp24 * image, FILE * file) {
    for (int j = 0; j < image->height; j++) {
        for (int i = 0; i < image->width; i++) {
            bmp24_readPixelValue(image, i, j, file);
        }
    }
}

void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file) {
    int rowSize = ((image->width * 3 + 3)/4)*4;
    uint32_t position = image->header.offset + (image->height - 1 - y) * rowSize + x * 3;

    uint8_t pixelData[3];

    pixelData[0] = image->data[y][x].blue;
    pixelData[1] = image->data[y][x].green;
    pixelData[2] = image->data[y][x].red;

    file_rawWrite(position, pixelData, sizeof(uint8_t), 3, file);
}
void bmp24_writePixelData (t_bmp24 * image, FILE * file) {
    for (int j = 0; j < image->height; j++) {
        for (int i = 0; i < image->width; i++) {
            bmp24_writePixelValue(image, i, j, file);
        }
    }
}
t_bmp24 * bmp24_loadImage (const char * filename) {
    FILE * file = fopen(filename, "rb");
    if (!file) {
        printf("Error in opening file\n");
        return NULL;
    }
    t_bmp_header header;
    t_bmp_info header_info;
    file_rawRead(BITMAP_MAGIC, &header.type, sizeof(uint16_t), 1, file);
    file_rawRead(BITMAP_SIZE, &header.size, sizeof(uint32_t), 1, file);
    file_rawRead(BITMAP_OFFSET, &header.offset, sizeof(uint32_t), 1, file);
    file_rawRead(HEADER_SIZE, &header_info, sizeof(t_bmp_info), 1, file);

    if (header.type != BMP_TYPE) {
        printf("Error file is not a BMP file! \n");
        fclose(file);
        return NULL;
    }

    if (header_info.bits != 24) {
        printf("%d", header_info.bits);
        printf("Error file is not 24 bit! \n");
        fclose(file);
        return NULL;
    }

    t_bmp24 * img = bmp24_allocate( (int)header_info.width, (int)header_info.height, header_info.bits);
    if (!img) {
        printf("Error in memory allocation of the bitmap\n");
        return NULL;
    }

    img->header = header;
    img->header_info = header_info;

    bmp24_readPixelData(img, file);
    fclose(file);
    printf("image successfully loaded!\n");

    return img;
}

void bmp24_saveImage (t_bmp24 * image, const char * filename) {
    FILE * file = fopen(filename, "wb");
    if (!file) {
        printf("Error in opening file\n");
        return;
    }

    file_rawWrite(BITMAP_MAGIC, &image->header.type, sizeof(uint16_t), 1, file);
    file_rawWrite(BITMAP_SIZE, &image->header.size, sizeof(uint32_t), 1, file);
    file_rawWrite(BITMAP_OFFSET, &image->header.offset, sizeof(uint32_t), 1, file);
    file_rawWrite(HEADER_SIZE, &(image->header_info), sizeof(t_bmp_info), 1, file);

    bmp24_writePixelData(image, file);
    fclose(file);
    printf("image successfully saved!\n");

}
void bmp24_printInfo(t_bmp24 *img){

    printf("____________Image info:____________\n\n");
    printf("width = %d\n", img->width);
    printf("height = %d\n", img->height);
    printf("color depth = %d\n", img->colorDepth);
    printf("bits: %d\n", img->header_info.bits);
    printf("size = %d\n", img->header.size);
    printf("offset = %d\n", img->header.offset);
}
void bmp24_negative (t_bmp24 * img) {
    if(!img){
        printf("error pointer null\n");
        return;
    }
    for (int y=0; y<img->height; y++) {
        for (int x=0; x<img->width; x++) {

            img->data[y][x].blue = 255 - img->data[y][x].blue;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].red = 255 - img->data[y][x].red;
        }
    }

}
void bmp24_grayscale (t_bmp24 * img) {
    if(!img){
        printf("error pointer null\n");
        return;
    }
    for (int y=0; y<img->height; y++) {
        for (int x=0; x<img->width; x++) {
            int avg = (img->data[y][x].blue + img->data[y][x].green + img->data[y][x].red)/3;

            img->data[y][x].blue = avg;
            img->data[y][x].green = avg;
            img->data[y][x].red = avg;
        }
    }

}
void bmp24_brightness (t_bmp24 * img, int value) {
    if(!img){
        printf("error pointer null\n");
        return;
    }
    for (int y=0; y<img->height; y++) {
        for (int x=0; x<img->width; x++) {

            img->data[y][x].blue += (img->data[y][x].blue + value > 255)? 255 - img->data[y][x].blue : value;
            img->data[y][x].green += (img->data[y][x].green + value > 255)? 255 - img->data[y][x].green : value;
            img->data[y][x].red += (img->data[y][x].red + value > 255)? 255 - img->data[y][x].red : value;
        }
    }
}
t_pixel bmp24_convolution (t_bmp24 * img, int x, int y, float ** kernel, int kernelSize) {
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
    t_pixel newPixel;
    if (!&newPixel) {
        printf("memory allocation failed\n");
        return;
    }
    int offset = kernelSize / 2;
    int sum_b = 0;
    int sum_g = 0;
    int sum_r = 0;

    // Apply kernel to the neighborhood
    for (int ky = 0; ky < kernelSize; ky++) {
        for (int kx = 0; kx < kernelSize; kx++) {
            // Calculate corresponding image coordinates
            int imgX = x + kx - offset;
            int imgY = y + ky - offset;

            // Handle border conditions (zero padding)
            if (imgX >= 0 && imgX < img->width && imgY >= 0 && imgY < img->height) {
                sum_b += img->data[imgY][imgX].blue * kernel[ky][kx];
                sum_g += img->data[imgY][imgX].green * kernel[ky][kx];
                sum_r += img->data[imgY][imgX].red * kernel[ky][kx];

            }
        }
    }

    //printf("oldPixel: b:%d g:%d r:%d\n", img->data[y][x].blue, img->data[y][x].green, img->data[y][x].red);
    //printf("newPixel: b:%d g:%d r:%d\n", newPixel.blue, newPixel.green, newPixel.red);
    sum_b = (sum_b > 255)? 255 : (sum_b < 0? 0 : sum_b);
    sum_g = (sum_g > 255)? 255 : (sum_g < 0? 0 : sum_g);
    sum_r = (sum_r > 255)? 255 : (sum_r < 0? 0 : sum_r);
    // Clamp the result to valid pixel values
    newPixel.blue = sum_b;
    newPixel.green = sum_g;
    newPixel.red = sum_r;

    return newPixel;
}
void bmp24_apply_filter(t_bmp24 * img,float ** kernel, int kernelSize) {

    t_bmp24 * copy = (t_bmp24 *)malloc(sizeof(t_bmp24));
    copy->data = (t_pixel**)malloc(sizeof(t_pixel*) * img->height);
    for (int y = 0; y < img->height; y++) {
        copy->data[y] = (t_pixel*)malloc(sizeof(t_pixel) * img->width);
        for (int x = 0; x < img->width; x++) {
            copy->data[y][x].blue = img->data[y][x].blue;
            copy->data[y][x].green = img->data[y][x].green;
            copy->data[y][x].red = img->data[y][x].red;
        }
    }
    copy->height = img->height;
    copy->width = img->width;

    for (int y=0; y<img->height; y++) {
        for (int x=0; x<img->width; x++) {
            img->data[y][x] = bmp24_convolution(copy, x, y, kernel, kernelSize);
        }
    }
}

unsigned int * bmp24_computeHistogram(t_bmp24 * img) {
    unsigned int * histogram = calloc(256, sizeof(unsigned int));
    for (int c=0; c < 256; c++) {
        for (int y=0; y < img->height; y++) {
            for (int x=0; x < img->width; x++) {
            float Y = 0.299*img->data[y][x].red + 0.587*img->data[y][x].green + 0.114*img->data[y][x].blue;
            Y = (int)Y>255? 255 : Y;
                if ((int)round(Y) == c) {
                    histogram[c]++;
                }
            }
        }
    }
    //for (int c=0; c < 256; c++) printf("histogram[%d]: %d\n",c ,histogram[c]);
    return histogram;
}

unsigned int * bmp24_computeCDF(unsigned int * hist) {
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
        cdf[i] = (float)(cdf[i]-cdfMin)/(N-cdfMin)*255;
        cdf[i] = ((float)cdf[i] > (float)((int)cdf[i]+1)/2 ? (int)cdf[i]+1 : (int)cdf[i]);
        //printf("cdf[%d] = %d\n", i, cdf[i]);
    }

    return cdf;
}

void bmp24_equalize(t_bmp24 * img) {
    unsigned int * hist = bmp24_computeHistogram(img);
    unsigned int * hist_eq = bmp24_computeCDF(hist);
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            float U = -0.14713*img->data[y][x].red - 0.28886*img->data[y][x].green + 0.436*img->data[y][x].blue;
            float V = 0.615*img->data[y][x].red - 0.51499*img->data[y][x].green - 0.10001*img->data[y][x].blue;
            float Y = 0.299*img->data[y][x].red + 0.587*img->data[y][x].green + 0.114*img->data[y][x].blue;
            int new_Y = (int)round(Y);
            new_Y = (new_Y > 255) ? 255 : new_Y;
            img->data[y][x].red = round(hist_eq[new_Y] + 1.13983*V);
            img->data[y][x].red = (img->data[y][x].red > 255)? 255: img->data[y][x].red < 0 ? 0 : img->data[y][x].red;
            img->data[y][x].green = round(hist_eq[new_Y] - 0.39465*U - 0.58060*V);
            img->data[y][x].green = (img->data[y][x].green > 255)? 255: img->data[y][x].green < 0 ? 0 : img->data[y][x].green;
            img->data[y][x].blue = round(hist_eq[new_Y] + 2.03211*U);
            img->data[y][x].blue = (img->data[y][x].blue > 255)? 255: img->data[y][x].blue < 0 ? 0 : img->data[y][x].blue;



        }
    }
    free(hist);
    free(hist_eq);
}






