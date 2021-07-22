#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <png.h>

#include "../includes/image_processing.h"
#include "../includes/tree.h"
#include "../includes/convolution.h"

image_data * initialise_data(unsigned char ** pixels, int height, int width, int channels) {

    image_data * new_image_p = (image_data *) malloc(sizeof(image_data));
    new_image_p->height = height;
    new_image_p->width = width;
    new_image_p->channels = channels;
    new_image_p->pixels = (float *) malloc(sizeof(float) * height * width * 3); 

    for (int y = 0; y < height; y++) {
        //row
        unsigned char * row = pixels[y];
        for (int x = 0; x < width * new_image_p->channels; x += new_image_p->channels) {
            //pixel
            for (int c = 0; c < new_image_p->channels; c ++) {
                //channel
                new_image_p->pixels[(y * new_image_p->width * new_image_p->channels) + x + c] = row[x + c];
            }
        }
    }

    // initialise methods

    new_image_p->export_pixels = &export_pixels;
    new_image_p->colour_to_greyscale = &colour_to_greyscale;
    new_image_p->expand_greyscale = &expand_greyscale;
    new_image_p->reduce_noise = &reduce_noise;
    new_image_p->reduce_resolution = & reduce_resolution;
    new_image_p->soften = &soften;

    return new_image_p;
}

unsigned char ** export_pixels(image_data * image) {

    // initialise the output array.
    unsigned char ** pixels = (unsigned char **) malloc(sizeof(unsigned char *) * image->height);
    for (int y = 0; y < image->height; y++) {
        pixels[y] = (unsigned char *) malloc(sizeof(unsigned char) * image->width * image->channels);
    }

    //copy contents of float array into output array
    for (int y = 0; y < image->height; y++) {
        unsigned char * row = pixels[y];
        for (int x = 0; x < image->width * image->channels; x += image->channels) {
            for (int c = 0; c < image->channels; c ++) {
                row[x + c] = image->pixels[(y * image->width * image->channels) + x + c];
            }
        }
    }

    return pixels;

}

void colour_to_greyscale(image_data * image) {

    if (image->channels != 3) {
        return;
    }

    // perceptual and gamma correction
    float weights[3] = {0.299, 0.587, 0.114};

    float * greyscale = (float *) malloc(sizeof(float) * image->height * image->width);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x ++) {
            float intensity = 0.0;
            for (int c = 0; c < image->channels; c ++) {
                intensity += weights[c] * (image->pixels[(y * image->width * image->channels) + (x * image->channels) + c]);
                /* intensity += image->pixels[(y * image->width * image->channels) + (x * image->channels) + c] / 3.0; */
            }
            greyscale[y * image->width + x] = intensity;
        }
    }

    /* for (int y = 0; y < image->height; y++) { */
    /*     for (int x = 0; x < image->width; x++) { */
    /*         printf("%f\n", greyscale[y * image->width + x]); */
    /*     } */
    /* } */

    // update the image struct
    image->channels = 1;
    free(image->pixels);
    image->pixels = (float *) malloc(sizeof(float) * image->height * image->width * image->channels);

    memcpy(image->pixels, greyscale, sizeof(float) * image->height * image->width * image->channels);

}

void expand_greyscale(image_data * image){

    if (image->channels == 3) {
        return;
    }

    float * rgb = (float *) malloc(sizeof(float) * image->height * image->width * 3);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // set each channel of the resulting array to the same value in original array
            for (int c = 0; c < 3; c ++) {
                rgb[(y * image->width * 3) + (x * 3) + c] = image->pixels[y * image->width + x];
            }
        }
    }

    // update the image struct
    image->channels = 3;
    free(image->pixels);
    image->pixels = (float *) malloc(sizeof(float) * image->height * image->width * image->channels);

    memcpy(image->pixels, rgb, sizeof(float) * image->height * image->width * image->channels);

};

void reduce_noise(image_data * image) {
    
    // initialise the kernel
    int kernel_dimensions = 5;
    kernel_configuration type = Mean; 
    float * kernel = create_kernel(type, kernel_dimensions);
    node * root = apply_convolution(image, kernel, kernel_dimensions);

    //update the list of pixels
    traverse_tree(root, image);

}

void reduce_resolution(image_data * image) {}
void soften(image_data * image) {}
