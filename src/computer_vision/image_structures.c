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

#include "./includes/image_structures.h"

image_data * initialise_data(unsigned char ** pixels, int height, int width, int channels) {

    image_data * new_image_p = (image_data *) malloc(sizeof(image_data));
    new_image_p->height = height;
    new_image_p->width = width;
    new_image_p->channels = channels;
    new_image_p->pixels = (float *) malloc(sizeof(float) * height * width * 3); 

    for (int y = 0; y < height; y++) {
        unsigned char * row = pixels[y];
        for (int x = 0; x < width * new_image_p->channels; x += new_image_p->channels) {
            for (int c = 0; c < new_image_p->channels; c ++) {
                new_image_p->pixels[(y * new_image_p->width * new_image_p->channels) + x + c] = row[x + c];
            }
        }
    }


    // initialise methods

    new_image_p->export_pixels = &export_pixels;
    new_image_p->colour_to_greyscale = &colour_to_greyscale;
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

void colour_to_greyscale(image_data * image) {}
void reduce_noise(image_data * image) {}
void reduce_resolution(image_data * image) {}
void soften(image_data * image) {}
