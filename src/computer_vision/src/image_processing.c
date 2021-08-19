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
#include "../includes/matrix.h"
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

        new_image_p->R = create_matrix(new_image_p->height, new_image_p->width);
        new_image_p->G = create_matrix(new_image_p->height, new_image_p->width);
        new_image_p->B = create_matrix(new_image_p->height, new_image_p->width);

    } else {

        new_image_p->greyscale = create_matrix(new_image_p->height, new_image_p->width);

    }

    for (int y = 0; y < new_image_p->height; y++) {

        unsigned char * row = pixels[y];

        for (int x = 0; x < (new_image_p->width * new_image_p->channels); x += new_image_p->channels) {

            if (new_image_p->channels == 3) {

                new_image_p->R->array[(y * new_image_p->width) + x/3] = row[x + 0];
                new_image_p->G->array[(y * new_image_p->width) + x/3] = row[x + 1];
                new_image_p->B->array[(y * new_image_p->width) + x/3] = row[x + 2];

            } else if (new_image_p->channels == 1) {

                for (int c = 0; c < new_image_p->channels; c ++) {

                    new_image_p->greyscale->array[(y * new_image_p->width * new_image_p->channels) + x + c] = row[x + c];

                }
            }
        }
    }

    // initialise methods

    new_image_p->export_pixels = &export_pixels;
    new_image_p->rgb_to_greyscale = &rgb_to_greyscale;
    new_image_p->greyscale_to_rgb = &greyscale_to_rgb;
    /* new_image_p->reduce_noise = &reduce_noise; */
    new_image_p->reduce_resolution = & reduce_resolution;
    /* new_image_p->soften = &soften; */
    /* new_image_p->sharpen = &sharpen; */
    new_image_p->invert = &invert;
    new_image_p->process = &process;

    return new_image_p;

}

void rgb_to_greyscale(image_data * image) {

    if (image->channels != 3) {
        return;
    }

    // perceptual and gamma correction
    float weights[3] = {0.299, 0.587, 0.114};

    matrix * tmp_greyscale = create_matrix(image->height, image->width);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x ++) {

            float intensity = 0.0;
            intensity += weights[0] * (image->R->array[(y * image->width) + x]);
            intensity += weights[1] * (image->G->array[(y * image->width) + x]);
            intensity += weights[2] * (image->B->array[(y * image->width) + x]);
            tmp_greyscale->array[(y * image->width) + x] = intensity;

        }
    }

    // update the image struct

    image->channels = 1;
    free(image->R);
    free(image->G);
    free(image->B);
    image->R = image->G = image->B = NULL;

    image->greyscale = create_matrix(image->height, image->width);
    memcpy(image->greyscale->array, tmp_greyscale->array, sizeof(float) * image->height * image->width * image->channels);
    free(tmp_greyscale);

}

void greyscale_to_rgb(image_data * image){

    if (image->channels == 3) {
        return;
    }

    image->R = create_matrix(image->height, image->width);
    image->G = create_matrix(image->height, image->width);
    image->B = create_matrix(image->height, image->width);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // set each channel of the resulting array to the same value in original array
            /* image->R[(y * image->width) + x] = image->greyscale[(y * image->width) + x]; */
            /* image->G[(y * image->width) + x] = image->greyscale[(y * image->width) + x]; */
            /* image->B[(y * image->width) + x] = image->greyscale[(y * image->width) + x]; */
            image->R->array[(y * image->width) + x] = image->G->array[(y * image->width) + x] = image->B->array[(y * image->width) + x] = image->greyscale->array[(y * image->width) + x];
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

                row[x + 0] = image->R->array[(y * image->width) + x/3];
                row[x + 1] = image->G->array[(y * image->width) + x/3];
                row[x + 2] = image->B->array[(y * image->width) + x/3];

            } else if (image->channels == 1) {

                for (int c = 0; c < 3; c ++) {

                    row[x+c] = image->greyscale->array[(y*image->width) + x/3];

                }
            }
        }
    }

    return pixels;

}

void process(image_data * image, kernel_configuration type, int kernel_dimensions) {

    float * kernel = create_kernel(type, kernel_dimensions);

    if (image->channels == 1) {

        matrix * tmp = image->greyscale;
        image->greyscale = apply_convolution(image->greyscale, type, kernel, kernel_dimensions);
        free(tmp);

    } else if (image->channels == 3) {

        matrix ** tmp = (matrix **) malloc(sizeof(matrix*) * 3);
        tmp[0] = image->R; tmp[1] = image->G; tmp[2] = image->B;

        image->R = apply_convolution(image->R, type, kernel, kernel_dimensions);
        image->G = apply_convolution(image->G, type, kernel, kernel_dimensions);
        image->B = apply_convolution(image->B, type, kernel, kernel_dimensions);

        for (int i = 0; i < 3; i++) { free(tmp[i]); }
        free(tmp);
    }

}

void reduce_resolution(image_data * image) {

    int step = 2;

    if (image->channels == 1) {

        matrix * tmp = image->greyscale;
        image->greyscale = max_pool_image(image->greyscale, step);
        free(tmp);

    } else if (image->channels == 3) {

        matrix ** tmp = (matrix **) malloc(sizeof(matrix*) * 3);
        tmp[0] = image->R; tmp[1] = image->G; tmp[2] = image->B;

        image->R = mean_pool_image(image->R, step);
        image->G = mean_pool_image(image->G, step);
        image->B = mean_pool_image(image->B, step);

        for (int i = 0; i < 3; i++) { free(tmp[i]); }
        free(tmp);

    }

    image->height /= step;
    image->width /= step;

    return;
}

void invert(image_data * image) {

    for (int y = 0; y < image->height; y ++){
        for (int x = 0; x < image->width; x ++){

            if (image->channels == 3) {
                image->R->array[(y * image->width) + x] = 255.0 - image->R->array[(y * image->width) + x];
                image->G->array[(y * image->width) + x] = 255.0 - image->G->array[(y * image->width) + x];
                image->B->array[(y * image->width) + x] = 255.0 - image->B->array[(y * image->width) + x];

            } else if (image->channels == 1) {

                image->greyscale->array[(y * image->width) + x] = 255.0 - image->greyscale->array[(y * image->width) + x];

            }

        }
    }
        
}
