/*
**===============================================================
**  File        : main.c
**  Author      : Giannini Loic and Benoît Tailhades
**  Created on  : 04/2025
**  Description : This file aim to gather all defined and declared function to make them work. This is why the final menu is defined here
**
**===============================================================
*/
#include "bmp8.h"
#include "bmp24.h"
#include <windows.h>

enum {BLUR, GAUSS, OUTLINE, EMBOSS, SHARPEN};

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

void bitmap_8_manipulation(float*** kernels) {

    t_bmp8* bmp8 = (t_bmp8*)malloc(sizeof(t_bmp8));
    bmp8 = bmp8_loadImage("..\\barbara_gray.bmp");
    if (bmp8 == NULL) {
        printf("Error loading image\n");
    }

    bmp8_printInfo(bmp8, 0);

    bmp8_applyFilter(bmp8, kernels[EMBOSS], 3);
    bmp8_saveImage("..\\filtered_8.bmp", bmp8);

    bmp8_free(bmp8);
}

void bitmap_24_manipulation(float*** kernels, int filter_type) {

    t_bmp24 *manipulator = bmp24_loadImage("..\\flowers_color.bmp");

    bmp24_printInfo(manipulator);

    bmp24_applyFilter(manipulator, kernels[filter_type], 3);

    bmp24_saveImage(manipulator, "..\\filtered_24.bmp");
}

int main(void) {
    float*** kernels = init_kernels();
    int choice = 0;
    int filter_type = 0;
    bool show_choices_menu = true;

    printf("HELLLLLLLLLLLLLLLLLLOOOOOOO !!!!!!\n"
                 "Are you the person that want to know everything about images??\n"
                 "You are at the right place!!\n");

    printf("-> Please enter the name of the image you are interested in (format => image_name.bmp):\n"
           "-> ");

    char path[7] = "..\\\\";
    char name[256];
    if (fgets(name, sizeof(name), stdin)){
        size_t len = strlen(name);
        if (len > 0 && name[len-1] == '\n') {
            name[len-1] = '\0';
        }
    }
    strcat(path, name);

    t_bmp8 * img8 = bmp8_loadImage(path);
    unsigned int bmp = 8;
    t_bmp24 * img24 = NULL;


    if (img8 == NULL) {
        free(img8);
        img24 = bmp24_loadImage(path);
        bmp = (img24)->colorDepth;
        if (img24 == NULL) {
            printf("%s is neither bmp8 nor bmp24. It cannot be edited here.\n", name);
            free_kernels(kernels, 5, 3);
            return 1;
        }
    }
    while(show_choices_menu) {
        show_choices_menu = false;
        printf("Your image is in bmp%d. Here is what you can do:\n"
               "1: Print basic informations about the image\n"
               "2: Apply filters\n"
               "3: Equalize Image\n"
               "Enter your choice(1-3): ", bmp);

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input\n");
            (bmp == 8)? bmp8_free(img8): bmp24_free(img24);
            free_kernels(kernels, 5, 3);
            return 1;
        }

        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}

            switch (choice) {

            case 1:

                (bmp == 8)? bmp8_printInfo(img8, 0):bmp24_printInfo(img24);
                break;
            case 2:
                bool valid_input = false;
                printf("Choose a filter between the following:\n"
                       "0.BLUR\n"
                       "1.GAUSS\n"
                       "2.OUTLINE\n"
                       "3.EMBOSS\n"
                       "4.SHARPEN\n"
                       "5.NEGATIVE\n"
                       "6.BRIGHTNESS\n");
                (bmp == 8)? printf("7.THRESHOLD\n"): printf("7.BLACK AND WHITE\n");
                printf("8.GO BACK\n");
                while (!valid_input) {
                    if (scanf("%d", &filter_type) != 1) {
                        printf("You entered an invalid input. Try Again\n");
                    } else if (filter_type < 0 || filter_type > 8) {
                        printf("You entered an invalid input. Try Again\n");
                    } else {
                        valid_input = true;
                    }
                }

                if (filter_type == 5) {
                    (bmp == 8)? bmp8_negative(img8):
                    bmp24_negative(img24);
                } else if (filter_type == 6) {
                    int brightness;
                    printf("Enter the brightness value: ");
                    scanf("%d", &brightness);
                    (bmp == 8)? bmp8_brightness(img8, brightness):
                    bmp24_brightness(img24, brightness);
                } else if (filter_type == 7) {
                    if (bmp == 8) {
                        int threshold;
                        printf("Enter the threshold value: ");
                        scanf("%d", &threshold);
                        bmp8_threshold(img8, threshold);
                    } else if (bmp == 24) {
                        bmp24_grayscale(img24);
                    }

                    bmp24_negative(img24);
                } else if (filter_type == 8) {
                    show_choices_menu = true;
                    break;
                } else {
                    (bmp == 8)? bmp8_applyFilter(img8, kernels[filter_type], 3):
                    bmp24_applyFilter(img24, kernels[filter_type], 3);
                }
                (bmp == 8)? bmp8_saveImage("../filtered_8.bmp", img8):
                bmp24_saveImage(img24, "../filtered_24.bmp");
                printf("Filtered image here \"../filtered_%d.bmp\"\n", bmp);
                break;

            case 3:
                if (bmp == 8) {
                    unsigned int * hist = bmp8_computeHistogram(img8);
                    unsigned int * hist_eq = bmp8_computeCDF(hist);
                    bmp8_equalize(img8, hist_eq);
                    bmp8_saveImage("../filtered_8.bmp",img8);
                } else {
                    bmp24_equalize(img24);
                    bmp24_saveImage(img24, "../filtered_24.bmp");
                }
                break;

            default:
                printf("Invalid choice bot\n");
        }

    }

    (bmp == 8)? bmp8_free(img8): bmp24_free(img24);

    // Libération de la mémoire
    free_kernels(kernels, 5, 3);
    return 0;
}
