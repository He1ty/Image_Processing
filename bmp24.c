#include "bmp24.h"


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
void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file) {
}
void bmp24_readPixelData (t_bmp24 * bitmap, FILE * file) {


    const int ligne_size = ((bitmap->header_info.width * 3 + 3) / 4) * 4;
    unsigned char ligne_buffer[ligne_size];


    for (int i = 0; i < bitmap->header_info.height; i++) {
        int ligne = bitmap->header_info.height - 1 - i;

        fseek(file, (int)bitmap->header.offset + ligne * ligne_size, SEEK_SET);
        int read_char = fread(ligne_buffer, 1, ligne_size, file);
        if (read_char != ligne_size) {
            printf("Error reading from file\n");
            return;
        }
        for (int j = 0; j < bitmap->header_info.width; j++) {
            int pixelIndex = j * 3;
            bitmap->data[i][j].blue  = ligne_buffer[pixelIndex];
            bitmap->data[i][j].green = ligne_buffer[pixelIndex + 1];
            bitmap->data[i][j].red   = ligne_buffer[pixelIndex + 2];
        }

    }
}
t_bmp24 * bmp24_loadImage (const char * filename) {

    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error in opening file\n");
        return NULL;
    }

    uint32_t width, height;
    uint16_t colorDepth;
    file_rawRead(BITMAP_WIDTH, &width, sizeof(unsigned char), 4, file);
    file_rawRead(BITMAP_HEIGHT, &height, sizeof(unsigned char), 4, file);
    file_rawRead(BITMAP_DEPTH, &colorDepth, sizeof(unsigned char), 2, file);


    t_bmp24* bitmap = bmp24_allocate( (int) width, (int) height, colorDepth);

    file_rawRead(BITMAP_MAGIC, &bitmap->header.type, 1, 2, file);
    file_rawRead(BITMAP_SIZE, &bitmap->header.size, 1, 4, file);
    file_rawRead(BITMAP_RESERVED1, &bitmap->header.reserved1, 1, 2, file);
    file_rawRead(BITMAP_RESERVED2, &bitmap->header.reserved2, 1, 2, file);
    file_rawRead(BITMAP_OFFSET, &bitmap->header.offset, 1, 4, file);

    bitmap->header_info.size = INFO_SIZE;
    bitmap->header_info.width = (int)width;
    bitmap->header_info.height = (int)height;
    file_rawRead(BITMAP_PLANES, &bitmap->header_info.planes, 1, 2, file);
    file_rawRead(BITMAP_BITS, &bitmap->header_info.bits, 1, 2, file);
    file_rawRead(BITMAP_COMPRESSED, &bitmap->header_info.compression, 1, 4, file);
    file_rawRead(BITMAP_SIZE_RAW, &bitmap->header_info.imagesize, 1, 4, file);
    file_rawRead(BITMAP_XRES, &bitmap->header_info.xresolution, 1, 4, file);
    file_rawRead(BITMAP_YRES, &bitmap->header_info.yresolution, 1, 4, file);
    file_rawRead(BITMAP_N_COLORS, &bitmap->header_info.ncolors, 1, 4, file);
    file_rawRead(BITMAP_IMP_COLORS, &bitmap->header_info.importantcolors, 1, 4, file);



    bmp24_readPixelData(bitmap, file);

    printf("Image data loaded\n");

    fclose(file);

    return bitmap;


}



void bmp24_printInfo(t_bmp24 * bitmap) {
    printf("Bitmap info:\n");
    printf("Width: %d\n", bitmap->header_info.width);
    printf("Height: %d\n", bitmap->header_info.height);
    printf("Bits per pixel: %d\n", bitmap->header_info.bits);
    printf("Compression: %d\n", bitmap->header_info.compression);
    printf("Size: %d\n", bitmap->header_info.imagesize);
    printf("Planes: %d\n", bitmap->header_info.planes);
    printf("Type: %02x\n", bitmap->header.type);
    printf("reserved 1: %02x\n", bitmap->header.reserved1);
    printf("reserved 2: %02x\n", bitmap->header.reserved2);

}



void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}
void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file) {



    const int ligne_size = ((image->header_info.width * 3 + 3) / 4) * 4; // calcul de la taille de la ligne pour qu'elle soit modulo 4.
    int position = image->header.offset + (image->header_info.height-1 -y) * ligne_size + x*3;

    //printf("offset(%d) + height-1-y(%d) + x(%d)*3(%d)   = position(%d)\n", image->header.offset, image->header_info.height-1-y, x, x*3, position);

    file_rawWrite(position, &image->data[y][x].blue, 1,1, file);
    file_rawWrite(position+1, &image->data[y][x].green, 1, 1, file);
    file_rawWrite(position+2, &image->data[y][x].red, 1, 1, file);
    //printf("writing pixel %d %d value @ %d \n", x, y, position);


}
void bmp24_writePixelData (t_bmp24 * image, FILE * file) {

    for (int i = 0; i < image->header_info.height; i++) {
        for (int j = 0; j < image->header_info.width; j++) {
            int pixelIndex = j * 3;
            bmp24_writePixelValue(image, j, i, file);
        }
    }

}
void bmp24_saveImage (t_bmp24* img, const char * filename) {

    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error in opening file\n");
        return;
    }

    file_rawWrite(BITMAP_MAGIC, &img->header.type, 1, 2, file);
    file_rawWrite(BITMAP_SIZE,  &img->header.size, 1, 4, file);
    file_rawWrite(BITMAP_RESERVED1, &img->header.reserved1, 1, 2, file);
    file_rawWrite(BITMAP_RESERVED2, &img->header.reserved2, 1, 2, file);
    file_rawWrite(BITMAP_OFFSET,    &img->header.offset, 1, 4, file);


    file_rawWrite(HEADER_SIZE,   &img->header_info.size, 1, 4, file);
    file_rawWrite(BITMAP_WIDTH, &img->header_info.width, 1, 4, file);
    file_rawWrite(BITMAP_HEIGHT, &img->header_info.height, 1, 4, file);
    file_rawWrite(BITMAP_PLANES, &img->header_info.planes, 1, 2, file);
    file_rawWrite(BITMAP_BITS, &img->header_info.bits, 1, 2, file);
    file_rawWrite(BITMAP_COMPRESSED, &img->header_info.compression, 1, 4, file);
    file_rawWrite(BITMAP_SIZE_RAW, &img->header_info.imagesize, 1, 4, file);
    file_rawWrite(BITMAP_XRES, &img->header_info.xresolution, 1, 4, file);
    file_rawWrite(BITMAP_YRES, &img->header_info.yresolution, 1, 4, file);
    file_rawWrite(BITMAP_N_COLORS, &img->header_info.ncolors, 1, 4, file);
    file_rawWrite(BITMAP_IMP_COLORS, &img->header_info.importantcolors, 1, 4, file);


    bmp24_writePixelData(img, file);

}

//data processing
void bmp24_negative (t_bmp24 * img) {
    for (int i = 0; i < img->header_info.height; i++) {
        for (int j = 0; j < img->header_info.width; j++) {
            img->data[i][j].blue =  255 - img->data[i][j].blue;
            img->data[i][j].green = 255 - img->data[i][j].green;
            img->data[i][j].red =   255 - img->data[i][j].red;
        }
    }

}

void bmp24_grayscale (t_bmp24 * img) {
    int gray;
    for (int i = 0; i < img->header_info.height; i++) {
        for (int j = 0; j < img->header_info.width; j++) {
            gray = (img->data[i][j].blue * 114 + img->data[i][j].green * 587 + img->data[i][j].red * 299) / 1000;
            img->data[i][j].blue = gray>255 ? 255 : gray;
            img->data[i][j].green = gray>255 ? 255 : gray;
            img->data[i][j].red = gray>255 ? 255 : gray;
        }
    }

}

void bmp24_brightness (t_bmp24 * img, int value) {

    for (int i = 0; i < img->header_info.height; i++) {
        for (int j = 0; j < img->header_info.width; j++) {
            img->data[i][j].blue    = (img->data[i][j].blue + value) > 255 ? 255 :((img->data[i][j].blue + value) < 0 ? 0 :  img->data[i][j].blue + value);
            img->data[i][j].green   = (img->data[i][j].green + value) > 255 ? 255 : ((img->data[i][j].green + value) < 0 ? 0 :  img->data[i][j].green + value);
            img->data[i][j].red     = (img->data[i][j].red + value) > 255 ? 255 : ((img->data[i][j].red + value) < 0 ? 0 :  img->data[i][j].red + value);
        }
    }


}

t_pixel bmp24_convolution (t_bmp24 * img, int x, int y, float ** kernel, int kernelSize) {

    t_pixel result;
    result.blue = 0;
    result.green = 0;
    result.red = 0;

    int offset = (kernelSize-1)/2;

    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {

            int J = x - offset + j;
            int I = y - offset + i;

            if(I < 0 || I >= img->height || J < 0 || J >= img->width)
                continue;


            result.red += img->data[I][J].red * kernel[i][j];
            result.green += img->data[I][J].green * kernel[i][j];
            result.blue += img->data[I][J].blue * kernel[i][j];


        }
    }

    result.red = result.red > 255 ? 255 : (result.red < 0 ? 0 : result.red);
    result.blue = result.blue > 255 ? 255 : (result.blue < 0 ? 0 : result.blue);
    result.green = result.green > 255 ? 255 : (result.green < 0 ? 0 : result.green);


    return result;
}



void bmp24_apply_filter(t_bmp24 * img,float ** kernel, int kernelSize) {

    t_bmp24* copy = (t_bmp24*)malloc(sizeof(t_bmp24));
    copy->data = (t_pixel**) malloc(sizeof(t_pixel*) * img->header_info.height);
    for (int i = 0; i < img->header_info.height; i++) {
        copy->data[i] = (t_pixel*) malloc(sizeof(t_pixel) * img->header_info.width);
    }

    for (int i = 0; i < img->header_info.height; i++) {
        for (int j = 0; j < img->header_info.width; j++) {
            copy->data[i][j].red = img->data[i][j].red;
            copy->data[i][j].green = img->data[i][j].green;
            copy->data[i][j].blue = img->data[i][j].blue;
        }
    }
    copy->height = img->header_info.height;
    copy->width = img->header_info.width;

    for (int i = 0; i < img->header_info.height; i++) {
        for (int j = 0; j < img->header_info.width; j++) {

            img->data[i][j] = bmp24_convolution(copy, j, i, kernel, kernelSize);

        }
    }


    bmp24_free(copy);

}

