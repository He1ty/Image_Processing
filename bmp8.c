#include "bmp8.h"


t_bmp8 * bmp8_loadImage(const char * filename) {

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf( "error opening file %s\n", filename);
        return NULL;
    }

    t_bmp8* bmp8 = (t_bmp8*)malloc(sizeof(t_bmp8));

    // lecture du header de l'image
    fread(bmp8->header, sizeof(unsigned char), 54, file);


    bmp8->width  = *(int*)&bmp8->header[18];

    // on pourrait aussi le lire de la sorte, en gros on des opérations bitwise.
    // premier bytes OU deuxieme bytes shifté 8 fois a gauche OU 3eme bytes shifté a gauche 16 fois OU 4eme bytes shifté a gauche 24 fois. On shift dans ce sens car on est en litle  endian
    int width = bmp8->header[18] | bmp8->header[19] << 8 | bmp8->header[20] << 16 | bmp8->header[21] << 24;

    bmp8->height = *(int*)&bmp8->header[22];
    bmp8->colorDepth = *(short*)&bmp8->header[28];

    // si y a plus ou moins de 256 couleurs ( 8bits ), on quitte le jeux
    if (bmp8->colorDepth != 8) {
        printf("error color plane (%d bits)\n", bmp8->colorDepth);
        fclose(file);
        return NULL;
    }

    // Dans le fichier bmp y a une liste de valeur pour chaque pixel, mais c est pas directement une couleur, c est un index.
    // dans le header on a la colorTable. On retrouve les valeurs RGB du pixel en allant a colorTable[index]
    fread(bmp8->colorTable, sizeof(unsigned char), 1024, file);


    // taille de la bitmap
    bmp8->dataSize = bmp8->width * bmp8->height;

    // taille de data sur mesure, f(dataSize)
    bmp8->data = malloc(sizeof(unsigned char) * bmp8->dataSize);
    if (bmp8->data == NULL) {
        printf("error allocating memory\n");
        fclose(file);
        return NULL;
    }

    // Lecture des pixels
    fread(bmp8->data, sizeof(unsigned char), bmp8->dataSize, file);




    fclose(file);
    return bmp8;
}

void bmp8_saveImage(const char * filename, t_bmp8 * img){


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
// j'avoue que la fonction me parait un peu débile, y a presque rien a free

  if (img){

      free(img->data);
      free(img);
      return;

  }
  printf( "error pointer null\n");



}

void bmp8_printInfo(t_bmp8 *img, bool verbose){

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
//selon la consigne il faut soustraire a 255 la valeur de chaque pixel, mais on se rappelle qu'on a pas la valeur direct des pixels, on a une lookup table
//La table fait 1024 alors que on est supposé etre sur 8 bits soit 256 couleur nan ? Et ben nan parceque c est 256 pour b, pour g, pour r et pour o, la luminance
//donc il faut adapter la lecture, c est a dire qu'il faut juste pas toucher à o.
    if(!img){
      printf("error pointer null\n");
      return;
    }
    int gray;
    int index;
    for (int i = 0; i < 256; i++) {
        index = i*4;
        gray = 255 - img->colorTable[index];


        img->colorTable[index] = gray;
        img->colorTable[index+1] = gray;
        img->colorTable[index+2] = gray;

    }
}



void bmp8_brightness(t_bmp8 * img, int value){
//meme idée que pour negatif

    if(!img){
        printf("error pointer null\n");
        return;
    }


    int gray;
    int index;
    for (int i = 0; i < 256; i++) {
        index = i*4;
        gray = img->colorTable[index]   + value > 255 ? 255 : img->colorTable[index] + value;// operateur ternaire, chais pas si vous avez vu, en gros c est un if else en une ligne, condition ? si oui : si non


        img->colorTable[index] = gray;
        img->colorTable[index+1] = gray;
        img->colorTable[index+2] = gray;

    }


}


void bmp8_threshold(t_bmp8 * img, int threshold){
//meme idée que pour negatif

    if(!img){
        printf("error pointer null\n");
        return;
    }



    int gray;
    int index = 0;
    for (int i = 0; i < 256; i++) {
        index = i*4;
        gray = img->colorTable[index]   > threshold ? 255 : 0;


        img->colorTable[index] = gray;
        img->colorTable[index+1] = gray;
        img->colorTable[index+2] = gray;

    }

}


int findBestIndex(t_bmp8 * img){





}

void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize){

    if(!img){
        printf("error pointer null\n");
        return;
    }
    if(!kernel){
        printf("error kernel null\n");
        return;
    }
    if(kernelSize % 2 == 0){
        printf("kernel size must be odd\n");
        return;
    }

    // Conversion de l'image indexée en image RGB brute
    int* image = (int*)malloc(sizeof(int) * img->dataSize);
    for(int i = 0; i < img->dataSize; i++){
        int index = img->data[i] * 4;
        image[i]= img->colorTable[index];
    }

    // Copie de l'image d'origine
    int* newImage = (int*)malloc(sizeof(int) * img->dataSize);
    for (int i = 0; i < img->dataSize; i++) {
        newImage[i] = image[i];
    }

    int sum;
    int offset = (kernelSize - 1) / 2;

    // Application du filtre
    for(int j = 0; j < img->height; j++){
        for(int i = 0; i < img->width; i++){

            sum=0;

            for(int k = 0; k < kernelSize; k++){
                for(int l = 0; l < kernelSize; l++){
                    int I = i - offset + l;
                    int J = j - offset + k;

                    if(I < 0 || I >= img->width || J < 0 || J >= img->height)
                        continue;

                    int index = J * img->width + I;

                    sum += image[index] * kernel[k][l];

                }
            }


            // on s'assure que sum est bien dans les limites des stocks disponibles
            sum = sum < 0 ? 0 : (sum > 255 ? 255 : sum);

            int index = j * img->width + i;
            newImage[index] = sum;

        }
    }

    // Reprojection des couleurs vers la palette
    for(int i = 0; i < img->dataSize; i++){
        img->data[i] = newImage[i];
    }

    free(image);
    free(newImage);
}
