#ifndef image_structure_H
#define image_structure_H

typedef struct image_data_T {
    int height, width;
    int channels;
    float * pixels;
    // methods
    unsigned char ** (* export_pixels) (struct image_data_T * image);
    void (* colour_to_greyscale) (struct image_data_T * image);
    void (* expand_greyscale) (struct image_data_T * image);
    void (* reduce_noise) (struct image_data_T * image);
    void (* reduce_resolution) (struct image_data_T * image);
    void (* soften) (struct image_data_T * image);

} image_data;

image_data * initialise_data(unsigned char ** pixels, int height, int width, int channels);
unsigned char ** export_pixels(image_data * image);
void colour_to_greyscale(image_data * image);
void expand_greyscale(image_data * image);
void reduce_noise(image_data * image);
void reduce_resolution(image_data * image);
void soften(image_data * image);

#endif //image_structure_H
