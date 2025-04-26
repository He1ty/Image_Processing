#include "bmp8.h"
#include "bmp24.h"


enum {BLUR, GAUSS, OUTLINE, EMBOSS, SHARP};



float*** init_kernels() {
    float*** kernels = (float***)malloc(sizeof(float**) * 5);

    float** kernel_gauss = (float**)malloc(sizeof(float*) * 3);
    for (int i = 0; i < 3; i++) kernel_gauss[i] = (float*)malloc(sizeof(float) * 3);
    kernel_gauss[0][0] = 1.0f/16.0f; kernel_gauss[0][1] = 2.0f/16.0f; kernel_gauss[0][2] = 1.0f/16.0f;
    kernel_gauss[1][0] = 2.0f/16.0f; kernel_gauss[1][1] = 4.0f/16.0f; kernel_gauss[1][2] = 2.0f/16.0f;
    kernel_gauss[2][0] = 1.0f/16.0f; kernel_gauss[2][1] = 2.0f/16.0f; kernel_gauss[2][2] = 1.0f/16.0f;

    float** kernel_outline = (float**)malloc(sizeof(float*) * 3);
    for (int i = 0; i < 3; i++) kernel_outline[i] = (float*)malloc(sizeof(float) * 3);
    kernel_outline[0][0] = -1.0f; kernel_outline[0][1] = -1.0f; kernel_outline[0][2] = -1.0f;
    kernel_outline[1][0] = -1.0f; kernel_outline[1][1] =  8.0f; kernel_outline[1][2] = -1.0f;
    kernel_outline[2][0] = -1.0f; kernel_outline[2][1] = -1.0f; kernel_outline[2][2] = -1.0f;

    float** kernel_sharpen = (float**)malloc(sizeof(float*) * 3);
    for (int i = 0; i < 3; i++) kernel_sharpen[i] = (float*)malloc(sizeof(float) * 3);
    kernel_sharpen[0][0] = 0.0f; kernel_sharpen[0][1] = -1.0f; kernel_sharpen[0][2] = 0.0f;
    kernel_sharpen[1][0] = -1.0f; kernel_sharpen[1][1] = 5.0f; kernel_sharpen[1][2] = -1.0f;
    kernel_sharpen[2][0] = 0.0f; kernel_sharpen[2][1] = -1.0f; kernel_sharpen[2][2] = 0.0f;

    float** kernel_emboss = (float**)malloc(sizeof(float*) * 3);
    for (int i = 0; i < 3; i++) kernel_emboss[i] = (float*)malloc(sizeof(float) * 3);
    kernel_emboss[0][0] = -2.0f; kernel_emboss[0][1] = -1.0f; kernel_emboss[0][2] = 0.0f;
    kernel_emboss[1][0] = -1.0f; kernel_emboss[1][1] =  1.0f; kernel_emboss[1][2] = 1.0f;
    kernel_emboss[2][0] =  0.0f; kernel_emboss[2][1] =  1.0f; kernel_emboss[2][2] = 2.0f;

    float** kernel_blur = (float**)malloc(sizeof(float*) * 3);
    for (int i = 0; i < 3; i++) kernel_blur[i] = (float*)malloc(sizeof(float) * 3);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            kernel_blur[i][j] = 1.0f / 9.0f;

    kernels[0] = kernel_blur;
    kernels[1] = kernel_gauss;
    kernels[2] = kernel_outline;
    kernels[3] = kernel_emboss;
    kernels[4] = kernel_sharpen;

    return kernels;
}
void free_kernels(float*** kernels, int num_kernels, int kernel_size) {
    for (int k = 0; k < num_kernels; k++) {
        for (int i = 0; i < kernel_size; i++) {
            free(kernels[k][i]);  // Libère chaque ligne du kernel
        }
        free(kernels[k]);  // Libère le tableau 2D du kernel
    }
    free(kernels);  // Libère le tableau de pointeurs de kernels
}


void bitmap_24_manipulation(float*** kernels) {

    t_bmp24 *nigga = bmp24_loadImage("bmp24.bmp");

    bmp24_printInfo(nigga);

    //bmp24_negative(nigga);
    //bmp24_grayscale(nigga);
    //bmp24_brightness(nigga, -100);

    bmp24_apply_filter(nigga, kernels[GAUSS], 3);

    bmp24_saveImage(nigga, "filtered_24.bmp");
}

void bitmap_8_manipulation(float*** kernels) {

    t_bmp8* bmp8 = (t_bmp8*)malloc(sizeof(t_bmp8));
    bmp8 = bmp8_loadImage("test_actual_grayscale.bmp");
    if (bmp8 == NULL) {
        printf("Error loading image\n");

    }

    bmp8_printInfo(bmp8, 0);




    bmp8_applyFilter(bmp8, kernels[SHARP], 3);
    bmp8_saveImage("filtered.bmp", bmp8);

    bmp8_free(bmp8);

}



int main(void) {


    // pour garder les choses propres je fais un tableau avec les kernels. Pour pas a se faire chier avec quel kernel est a tel indice on utilise le enum
    // du  début. donc kernels[BLUR] par exemple
    float*** kernels = init_kernels();

    bitmap_24_manipulation(kernels);
    //bitmap_8_manipulation(kernels);


    free_kernels(kernels, 5, 3);


    return 0;
}