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

    new_image_p->R = NULL;
    new_image_p->G = NULL;
    new_image_p->B = NULL;
    new_image_p->greyscale = NULL;

    if (channels == 3) {

        new_image_p->R = (float*) malloc(sizeof(float) * height * width);
        new_image_p->G = (float*) malloc(sizeof(float) * height * width);
        new_image_p->B = (float*) malloc(sizeof(float) * height * width);

    } else {

        new_image_p->greyscale = (float *) malloc(sizeof(float) * new_image_p->height * new_image_p->width); 

    }

    for (int y = 0; y < new_image_p->height; y++) {

        unsigned char * row = pixels[y];

        for (int x = 0; x < (new_image_p->width * new_image_p->channels); x += new_image_p->channels) {

            if (new_image_p->channels == 3) {

                new_image_p->R[(y * new_image_p->width) + x/3] = row[x + 0];
                new_image_p->G[(y * new_image_p->width) + x/3] = row[x + 1];
                new_image_p->B[(y * new_image_p->width) + x/3] = row[x + 2];

            } else if (new_image_p->channels == 1) {

                for (int c = 0; c < new_image_p->channels; c ++) {

                    new_image_p->greyscale[(y * new_image_p->width * new_image_p->channels) + x + c] = row[x + c];

                }
            }
        }
    }

    // initialise methods

    new_image_p->export_pixels = &export_pixels;
    new_image_p->rgb_to_greyscale = &rgb_to_greyscale;
    new_image_p->greyscale_to_rgb = &greyscale_to_rgb;
    new_image_p->reduce_noise = &reduce_noise;
    new_image_p->reduce_resolution = & reduce_resolution;
    new_image_p->soften = &soften;
    new_image_p->sharpen = &sharpen;

    return new_image_p;

}

void rgb_to_greyscale(image_data * image) {

    if (image->channels != 3) {
        return;
    }

    // perceptual and gamma correction
    float weights[3] = {0.299, 0.587, 0.114};

    float * tmp_greyscale = (float *) malloc(sizeof(float) * image->height * image->width);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x ++) {

            float intensity = 0.0;
            intensity += weights[0] * (image->R[(y * image->width) + x]);
            intensity += weights[1] * (image->G[(y * image->width) + x]);
            intensity += weights[2] * (image->B[(y * image->width) + x]);
            tmp_greyscale[(y * image->width) + x] = intensity;

        }
    }

    // update the image struct

    image->channels = 1;
    free(image->R);
    free(image->G);
    free(image->B);
    image->R = image->G = image->B = NULL;

    image->greyscale = (float *) malloc(sizeof(float) * image->height * image->width * image->channels);
    memcpy(image->greyscale, tmp_greyscale, sizeof(float) * image->height * image->width * image->channels);
    free(tmp_greyscale);

}

void greyscale_to_rgb(image_data * image){

    if (image->channels == 3) {
        return;
    }

    image->R = (float*) malloc(sizeof(float) * image->height * image->width);
    image->G = (float*) malloc(sizeof(float) * image->height * image->width);
    image->B = (float*) malloc(sizeof(float) * image->height * image->width);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // set each channel of the resulting array to the same value in original array
            /* image->R[(y * image->width) + x] = image->greyscale[(y * image->width) + x]; */
            /* image->G[(y * image->width) + x] = image->greyscale[(y * image->width) + x]; */
            /* image->B[(y * image->width) + x] = image->greyscale[(y * image->width) + x]; */
            image->R[(y * image->width) + x] = image->G[(y * image->width) + x] = image->B[(y * image->width) + x] = image->greyscale[(y * image->width) + x];
        }
    }

    // update the image struct
    image->channels = 3;
    free(image->greyscale);
    image->greyscale = NULL;

};

unsigned char ** export_pixels(image_data * image) {

    // initialise the output array.
    unsigned char ** pixels = (unsigned char **) malloc(sizeof(unsigned char *) * image->height);
    for (int y = 0; y < image->height; y++) {
        pixels[y] = (unsigned char *) malloc(sizeof(unsigned char) * image->width * 3);
    }

    //copy contents of float array into output array
    for (int y = 0; y < image->height; y++) {
        unsigned char * row = pixels[y];
        for (int x = 0; x < image->width * 3; x += 3) {
            if (image->channels == 3) {

                row[x + 0] = image->R[(y * image->width) + x/3];
                row[x + 1] = image->G[(y * image->width) + x/3];
                row[x + 2] = image->B[(y * image->width) + x/3];

            } else if (image->channels == 1) {

                for (int c = 0; c < 3; c ++) {

                    row[x+c] = image->greyscale[(y*image->width) + x/3];

                }
            }
        }
    }

    return pixels;

}

void reduce_noise(image_data * image) {
    
    // initialise the kernel
    int kernel_dimensions = 5;
    kernel_configuration type = Mean; 
    float * kernel = create_kernel(type, kernel_dimensions);

    if (image->channels == 1) {

        image->greyscale = apply_convolution(image->greyscale, image->height, image->width, type, kernel, kernel_dimensions);

    } else if (image->channels == 3) {

        image->R = apply_convolution(image->R, image->height, image->width, type, kernel, kernel_dimensions);
        image->G = apply_convolution(image->G, image->height, image->width, type, kernel, kernel_dimensions);
        image->B = apply_convolution(image->B, image->height, image->width, type, kernel, kernel_dimensions);

    }

}

void soften(image_data * image) {

    // initialise the kernel
    int kernel_dimensions = 5;
    kernel_configuration type = Gaussian; 
    float * kernel = create_kernel(type, kernel_dimensions);

    if (image->channels == 1) {

        image->greyscale = apply_convolution(image->greyscale, image->height, image->width, type, kernel, kernel_dimensions);

    } else if (image->channels == 3) {

        image->R = apply_convolution(image->R, image->height, image->width, type, kernel, kernel_dimensions);
        image->G = apply_convolution(image->G, image->height, image->width, type, kernel, kernel_dimensions);
        image->B = apply_convolution(image->B, image->height, image->width, type, kernel, kernel_dimensions);

    }

}

void sharpen(image_data * image) {

    // initialise the kernel
    int kernel_dimensions = 3;
    kernel_configuration type = Sharpen; 
    float * kernel = create_kernel(type, kernel_dimensions);

    if (image->channels == 1) {

        image->greyscale = apply_convolution(image->greyscale, image->height, image->width, type, kernel, kernel_dimensions);

    } else if (image->channels == 3) {

        image->R = apply_convolution(image->R, image->height, image->width, type, kernel, kernel_dimensions);
        image->G = apply_convolution(image->G, image->height, image->width, type, kernel, kernel_dimensions);
        image->B = apply_convolution(image->B, image->height, image->width, type, kernel, kernel_dimensions);

    }

}

void reduce_resolution(image_data * image) {

    int new_height = image->height;
    int new_width = image->width;

    if (image->channels == 1) {

        float * tmp = image->greyscale;
        image->greyscale = mean_pool_image(image->greyscale, 2, &new_height, &new_width);
        free(tmp);

    } else if (image->channels == 3) {

        float ** tmp = (float **) malloc(sizeof(float*) * 3);
        tmp[0] = image->R; tmp[1] = image->G; tmp[2] = image->B;

        image->R = mean_pool_image(image->R, 2, &new_height, &new_width);
        image->G = mean_pool_image(image->G, 2, &new_height, &new_width);
        image->B = mean_pool_image(image->B, 2, &new_height, &new_width);

        for (int i = 0; i < 3; i++) { free(tmp[i]); }
        free(tmp);


    }

    image->height = new_height;
    image->width = new_width;

    return;
}
