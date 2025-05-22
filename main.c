#include "bmp8.h"
#include "bmp24.h"
#include <windows.h>

enum {BLUR, GAUSS, OUTLINE, EMBOSS, SHARPEN};

float*** init_kernels() {
    float*** kernels = (float***)malloc(sizeof(float**) * 5);

    // Gaussian kernel
    float** kernel_gauss = (float**)malloc(sizeof(float*) * 3);
    for (int i = 0; i < 3; i++) kernel_gauss[i] = (float*)malloc(sizeof(float) * 3);
    kernel_gauss[0][0] = 1.0f/16.0f; kernel_gauss[0][1] = 2.0f/16.0f; kernel_gauss[0][2] = 1.0f/16.0f;
    kernel_gauss[1][0] = 2.0f/16.0f; kernel_gauss[1][1] = 4.0f/16.0f; kernel_gauss[1][2] = 2.0f/16.0f;
    kernel_gauss[2][0] = 1.0f/16.0f; kernel_gauss[2][1] = 2.0f/16.0f; kernel_gauss[2][2] = 1.0f/16.0f;

    // Outline detection kernel
    float** kernel_outline = (float**)malloc(sizeof(float*) * 3);
    for (int i = 0; i < 3; i++) kernel_outline[i] = (float*)malloc(sizeof(float) * 3);
    kernel_outline[0][0] = -1.0f; kernel_outline[0][1] = -1.0f; kernel_outline[0][2] = -1.0f;
    kernel_outline[1][0] = -1.0f; kernel_outline[1][1] =  8.0f; kernel_outline[1][2] = -1.0f;
    kernel_outline[2][0] = -1.0f; kernel_outline[2][1] = -1.0f; kernel_outline[2][2] = -1.0f;

    // Sharpen kernel
    float** kernel_sharpen = (float**)malloc(sizeof(float*) * 3);
    for (int i = 0; i < 3; i++) kernel_sharpen[i] = (float*)malloc(sizeof(float) * 3);
    kernel_sharpen[0][0] = 0.0f; kernel_sharpen[0][1] = -1.0f; kernel_sharpen[0][2] = 0.0f;
    kernel_sharpen[1][0] = -1.0f; kernel_sharpen[1][1] = 5.0f; kernel_sharpen[1][2] = -1.0f;
    kernel_sharpen[2][0] = 0.0f; kernel_sharpen[2][1] = -1.0f; kernel_sharpen[2][2] = 0.0f;

    // Emboss kernel
    float** kernel_emboss = (float**)malloc(sizeof(float*) * 3);
    for (int i = 0; i < 3; i++) kernel_emboss[i] = (float*)malloc(sizeof(float) * 3);
    kernel_emboss[0][0] = -2.0f; kernel_emboss[0][1] = -1.0f; kernel_emboss[0][2] = 0.0f;
    kernel_emboss[1][0] = -1.0f; kernel_emboss[1][1] =  1.0f; kernel_emboss[1][2] = 1.0f;
    kernel_emboss[2][0] =  0.0f; kernel_emboss[2][1] =  1.0f; kernel_emboss[2][2] = 2.0f;

    // Blur kernel
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
            free(kernels[k][i]);
        }
        free(kernels[k]);
    }
    free(kernels);
}

// Function to display image using Windows system command
void display_image(const char* image_path, const char* description) {
    printf("\n%s\n", description);
    printf("Opening image: %s\n", image_path);

    char command[512];
    snprintf(command, sizeof(command), "start \"\" \"%s\"", image_path);

    int result = system(command);
    if (result != 0) {
        printf("Warning: Could not open image viewer. You can manually open: %s\n", image_path);
    } else {
        printf("Image opened successfully!\n");
    }

    // Give user time to view the image
    printf("Press Enter to continue...");
    getchar();
}

// Function to get filter name for display
const char* get_filter_name(int filter_type) {
    switch(filter_type) {
        case BLUR: return "BLUR";
        case GAUSS: return "GAUSSIAN";
        case OUTLINE: return "OUTLINE";
        case EMBOSS: return "EMBOSS";
        case SHARPEN: return "SHARPEN";
        default: return "UNKNOWN";
    }
}

int main(void) {
    float*** kernels = init_kernels();
    char name[256] = {0};
    int choice = 0;
    int filter_type = 0;
    t_bmp8* img8 = NULL;
    t_bmp24* img24 = NULL;

    printf("-------------------------------------------\n");
    printf("    WELCOME TO THE IMAGE PROCESSOR!\n");
    printf("-------------------------------------------\n");
    printf("Discover everything about BMP images!\n\n");

    printf("Please enter the image path (format: ..\\\\image_name.bmp):\n> ");

    if (fgets(name, sizeof(name), stdin) == NULL) {
        printf("Error reading input\n");
        free_kernels(kernels, 5, 3);
        return 1;
    }

    size_t len = strlen(name);
    if (len > 0 && name[len-1] == '\n') {
        name[len-1] = '\0';
    }

    img8 = bmp8_loadImage(name);

    if (img8 == NULL) {
        printf("Could not load as grayscale image, trying color image...\n");
        img24 = bmp24_loadImage(name);

        if (img24 == NULL) {
            printf("Error: Could not load image '%s'\n", name);
            printf("Please check if the file exists and is a valid BMP image.\n");
            free_kernels(kernels, 5, 3);
            return 1;
        }

        printf("\n--- COLOR IMAGE DETECTED ---\n");

        display_image(name, "--- ORIGINAL COLOR IMAGE ---");

        printf("\nWhat would you like to do with your color image?\n");
        printf("1: Display image information\n");
        printf("2: Apply filters and visualize results\n");
        printf("3: Show a programming joke\n");
        printf("Enter your choice (1-3): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input\n");
            bmp24_free(img24);
            free_kernels(kernels, 5, 3);
            return 1;
        }

        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        switch (choice) {
            case 1:
                printf("\n--- IMAGE INFORMATION ---\n");
                bmp24_printInfo(img24);
                break;

            case 2:
                printf("\n--- FILTER SELECTION ---\n");
                printf("Available filters:\n");
                printf("0 = BLUR (smooth/soften)\n");
                printf("1 = GAUSSIAN (advanced blur)\n");
                printf("2 = OUTLINE (edge detection)\n");
                printf("3 = EMBOSS (3D effect)\n");
                printf("4 = SHARPEN (enhance details)\n");
                printf("Choose filter (0-4): ");

                if (scanf("%d", &filter_type) != 1 || filter_type < 0 || filter_type > 4) {
                    printf("Invalid input. Using BLUR filter as default.\n");
                    filter_type = BLUR;
                }

                printf("\nApplying %s filter...\n", get_filter_name(filter_type));
                bmp24_apply_filter(img24, kernels[filter_type], 3);
                bmp24_saveImage(img24, "..\\filtered_24.bmp");

                printf("Filter applied successfully!\n");

                display_image("..\\filtered_24.bmp","--- FILTERED COLOR IMAGE ---\n""Compare this with the original image!");
                break;

            case 3:
                printf("\n--- Little gimmick ---\n");
                printf("Why do images never lie?\n");
                printf("Because they're always well-developed! 📸\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

        bmp24_free(img24);

    } else {
        printf("\n--- GRAYSCALE IMAGE DETECTED ---\n");

        display_image(name, "--- ORIGINAL"" GRAYSCALE IMAGE ---");

        printf("\nWhat would you like to do with your grayscale image?\n");
        printf("1: Display image information\n");
        printf("2: Apply filters and visualize results\n");
        printf("3: Show a programming joke\n");
        printf("Enter your choice (1-3): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input\n");
            bmp8_free(img8);
            free_kernels(kernels, 5, 3);
            return 1;
        }

        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        switch (choice) {
            case 1:
                printf("\n--- IMAGE INFORMATION ---\n");
                bmp8_printInfo(img8, 0);
                break;

            case 2:
                printf("\n--- FILTER SELECTION ---\n");
                printf("Available filters:\n");
                printf("0 = BLUR (smooth/soften)\n");
                printf("1 = GAUSSIAN (Kind  blur)\n");
                printf("2 = OUTLINE (edge detection)\n");
                printf("3 = EMBOSS (3D effect)\n");
                printf("4 = SHARPEN (enhance details)\n");
                printf("Choose filter (0-4): ");

                if (scanf("%d", &filter_type) != 1 || filter_type < 0 || filter_type > 4) {
                    printf("Invalid input. Using BLUR filter as default.\n");
                    filter_type = BLUR;
                }

                printf("\nApplying %s filter...\n", get_filter_name(filter_type));
                bmp8_applyFilter(img8, kernels[filter_type], 3);
                bmp8_saveImage("..\\filtered_8.bmp", img8);

                printf("Filter applied successfully!\n");

                display_image("..\\filtered_8.bmp","--- FILTERED GRAYSCALE IMAGE ---\n""Compare this with the original image!");
                break;


            default:
                printf("Invalid choice.\n");
        }

        bmp8_free(img8);
    }

    
    free_kernels(kernels, 5, 3);



    return 0;
}