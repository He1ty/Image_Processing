#ifndef BMP24_H
#define BMP24_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Offsets for the BMP header
#define BITMAP_MAGIC 0x00 // offset 0
#define BITMAP_SIZE 0x02 // offset 2
#define BITMAP_RESERVED1 0x06 // offset 6
#define BITMAP_RESERVED2 0x08 // offset 8
#define BITMAP_OFFSET 0x0A // offset 10
#define BITMAP_WIDTH 0x12 // offset 18
#define BITMAP_HEIGHT 0x16 // offset 22
#define BITMAP_DEPTH 0x1C // offset 28

//offsets for the bmp info_header
#define BITMAP_PLANES 0x1A // offset 26
#define BITMAP_BITS 0x1C
#define BITMAP_COMPRESSED 0x1E
#define BITMAP_SIZE_RAW 0x22 // offset 34
#define BITMAP_XRES 0x26 // offset 38
#define BITMAP_YRES 0x2A
#define BITMAP_N_COLORS 0x2E
#define BITMAP_IMP_COLORS 0x32

// Magical number for BMP files
#define BMP_TYPE 0x4D42 // 'BM' in hexadecimal
// Header sizes
#define HEADER_SIZE 0x0E // offset 14
#define INFO_SIZE 0x28 // 40 octets
// Constant for the color depth
#define DEFAULT_DEPTH 0x18 // 24





typedef struct {
    int red;
    int green;
    int blue;
}t_pixel;




typedef struct {
    uint16_t type;
    uint32_t size;    // size du fichier
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;
typedef struct {
    uint32_t size;  // size du header_info
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits;  // color depth en gros
    uint32_t compression;
    uint32_t imagesize;    // size de la matrice de data
    int32_t xresolution;
    int32_t yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
} t_bmp_info;


typedef struct {
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;
} t_bmp24;


//utils
t_pixel ** bmp24_allocateDataPixels (int width, int height);
void bmp24_freeDataPixels (t_pixel ** pixels, int height);
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
void bmp24_free (t_bmp24 * img);
void bmp24_printInfo(t_bmp24 * bitmap);


t_bmp24 * bmp24_loadImage (const char * filename);
void bmp24_saveImage (t_bmp24 * img, const char * filename);
void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file);
void bmp24_readPixelData (t_bmp24 * image, FILE * file);
void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file);
void bmp24_writePixelData (t_bmp24 * image, FILE * file);


//debug
void compare_da_images();



//data processing
void bmp24_negative (t_bmp24 * img);
void bmp24_grayscale (t_bmp24 * img);
void bmp24_brightness (t_bmp24 * img, int value);
t_pixel bmp24_convolution (t_bmp24 * img, int x, int y, float ** kernel, int kernelSize);
void bmp24_apply_filter(t_bmp24 * img,float ** kernel, int kernelSize);

#endif //BMP24_H

