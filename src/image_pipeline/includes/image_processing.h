#ifndef image_structure_H
#define image_structure_H

#include "./matrix.h"
#include "./convolution.h"
#include "./character_detection.h"
#include "./dataset.h"


typedef struct image_data_T {

    int height, width;
    int channels;
    matrix * greyscale;
    matrix * R;
    matrix * G;
    matrix * B;
    document * document_p;
    dataset * set;

    unsigned char ** (* export_pixels) (struct image_data_T * image);

    void (* rgb_to_greyscale) (struct image_data_T * self);
    void (* greyscale_to_rgb) (struct image_data_T * self);
    void (* reduce_resolution) (struct image_data_T * self);
    void (* invert) (struct image_data_T * self);
    void (* process) (struct image_data_T *, kernel_configuration type, int kernel_dimensions, float strength);
    void (* resize) (struct image_data_T * self, float scale_factor);
    void (* image_translation) (struct image_data_T * self, int x, int y);
    void (* create_document_outline) (struct image_data_T * self);
    void (* generate_dataset_from_image) (struct image_data_T * self, char * path);

} image_data;

image_data * initialise_data(unsigned char ** pixels, int height, int width, int channels);
unsigned char ** export_pixels(image_data * self);
void destroy_image_data(image_data * old_image);

void rgb_to_greyscale(image_data * self);
void greyscale_to_rgb(image_data * self);
void reduce_resolution(image_data * self);
void invert(image_data * self);
void process(image_data * self, kernel_configuration type, int kernel_dimensions, float strength);
void resize(image_data * self, float scale_factor);
void image_translation(image_data * self, int x, int y);
void create_document_outline(image_data * self);
void generate_dataset_from_image(image_data * self, char * path);

#endif 
