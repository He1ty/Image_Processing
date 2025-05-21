# Image Processing in C

## 📌 Project Overview

Efrei Paris P1 int-3 C semester project


The project is divided into three parts:

1. **Grayscale image processing**
2. **Color image processing**
3. **Histogram equalization**

## 📂 Repository Structure

```
📁 src/
├── bmp8.c / bmp8.h            # Grayscale image structures & functions
├── bmp24.c / bmp24.h          # Color image structures & functions
├── filters.c / filters.h      # Image filter implementations
├── histogram.c / histogram.h  # Histogram equalization
├── main.c                     # Main program with menu interface
├── utils.c / utils.h          # Utility functions (file I/O, memory, etc.)
📁 images/
├── barbara_gray.bmp           # Sample grayscale image
├── flowers_color.bmp          # Sample color image
📁 bin/
├── image_processing.exe       # Compiled executable
📄 README.md                   # Project documentation
📄 Makefile                    # Compilation instructions
```

Filters available:

* Negative
* Brightness Adjustment
* Thresholding (Black and White)
* Box Blur
* Gaussian Blur
* Sharpen
* Emboss
* Outline



##  Features by Part

###  Part 1: 8-bit Grayscale Image

* Load/Save BMP images
* Display image metadata
* Apply basic pixel operations:

  * Negative
  * Brightness change
  * Threshold
* Apply convolution filters:

  * Box blur
  * Gaussian blur
  * Emboss, Sharpen, Outline

###  Part 2: 24-bit Color Image

* Load/Save 24-bit color BMP
* Convert to grayscale
* Modify brightness
* Apply same convolution filters (adapted for RGB)

###  Part 3: Histogram Equalization

* Compute histogram & CDF
* Equalize contrast in grayscale images
* Convert RGB → YUV and apply equalization on Y channel

##  Learning Objectives

* Handle low-level memory operations in C.
* Understand bitmap image structure.
* Perform matrix-based transformations on image pixels.
* Implement common image filters using convolution.
* Apply histogram equalization for contrast enhancement.


##  Team 

* Giannini Loic 
* Tailhades Benoit




