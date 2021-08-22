#ifndef image_structure_H
#define image_structure_H

#include "./matrix.h"
#include "./convolution.h"


typedef struct image_data_T {

    int height, width;
    int channels;
    matrix * greyscale;
    matrix * R;
    matrix * G;
    matrix * B;

    unsigned char ** (* export_pixels) (struct image_data_T * image);

    void (* rgb_to_greyscale) (struct image_data_T * image);
    void (* greyscale_to_rgb) (struct image_data_T * image);
    void (* reduce_resolution) (struct image_data_T * image);
    void (* invert) (struct image_data_T * image);
    void (* process) (struct image_data_T *, kernel_configuration type, int kernel_dimensions);
    void (* resize) (struct image_data_T * image, float scale_factor);
    void (* locate_characters) (struct image_data_T * image);

} image_data;

image_data * initialise_data(unsigned char ** pixels, int height, int width, int channels);

unsigned char ** export_pixels(image_data * image);

void rgb_to_greyscale(image_data * image);
void greyscale_to_rgb(image_data * image);
void reduce_resolution(image_data * image);
void invert(image_data * image);
void process(image_data * image, kernel_configuration type, int kernel_dimensions);
void resize(image_data * image, float scale_factor);
void locate_characters(image_data * image);

#endif 
