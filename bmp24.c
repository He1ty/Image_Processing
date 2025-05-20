#include "bmp24.h"

#include "bmp8.h"


/*debugging
void compare_da_images() {

    unsigned char original_buffer[54];
    unsigned char saved_buffer[54];

    FILE* file = fopen("bmp24.bmp", "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    fread(original_buffer, sizeof(unsigned char), 54, file);
    fclose(file);

    file = fopen("test_save_24.bmp", "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    fread(saved_buffer, sizeof(unsigned char), 54, file);
    fclose(file);


    for (int i = 0; i < 54; i++) {
        if (original_buffer[i] != saved_buffer[i]) {
            printf("difference spotted @ i:%d", i);
        }
    }



}
*/

//utils
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
    long position = image->header.offset + (image->height - 1 - y) * image->width + x * 3;

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
    long position = image->header.offset + (image->height - 1 - y) * image->width + x * 3;

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

    file_rawWrite(0, &image->header.type, sizeof(uint16_t), 1, file);
    file_rawWrite(BITMAP_SIZE, &image->header.size, sizeof(uint32_t), 1, file);
    file_rawWrite(BITMAP_OFFSET, &image->header.offset, sizeof(uint32_t), 1, file);
    file_rawWrite(HEADER_SIZE, &image->header_info, sizeof(t_bmp_info), 1, file);

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

