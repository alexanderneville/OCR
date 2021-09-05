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
/* #include "../includes/tree.h" */
#include "../includes/convolution.h"
#include "../includes/character_detection.h"
#include "../includes/dataset.h"

image_data * initialise_data(unsigned char ** pixels, int height, int width, int channels) {

    image_data * new_image_p = (image_data *) malloc(sizeof(image_data));
    new_image_p->height = height;
    new_image_p->width = width;
    new_image_p->channels = channels;
    new_image_p->document_p = initialise_document();
    new_image_p->data = NULL;

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
    new_image_p->reduce_resolution = & reduce_resolution;
    new_image_p->invert = &invert;
    new_image_p->process = &process;
    new_image_p->resize = &resize;
    new_image_p->create_document_outline = &create_document_outline;
    new_image_p->generate_dataset_from_image = &generate_dataset_from_image;

    return new_image_p;
}

void destroy_image_data(image_data * old_image) {

    destroy_matrix(old_image->R);
    destroy_matrix(old_image->G);
    destroy_matrix(old_image->B);
    destroy_matrix(old_image->greyscale);
    destroy_document(old_image->document_p);

    for (int i = 0; i < old_image->total_characters; i++) {
        destroy_dataset_element(old_image->data[i]);
    }

    free(old_image->data);
    destroy_document(old_image->document_p);

    free(old_image);
    return;
}

void rgb_to_greyscale(image_data * self) {

    if (self->channels != 3) {
        return;
    }

    // perceptual and gamma correction
    float weights[3] = {0.299, 0.587, 0.114};

    matrix * tmp_greyscale = create_matrix(self->height, self->width);

    for (int y = 0; y < self->height; y++) {
        for (int x = 0; x < self->width; x ++) {

            float intensity = 0.0;
            intensity += weights[0] * (self->R->array[(y * self->width) + x]);
            intensity += weights[1] * (self->G->array[(y * self->width) + x]);
            intensity += weights[2] * (self->B->array[(y * self->width) + x]);
            tmp_greyscale->array[(y * self->width) + x] = intensity;

        }
    }

    // update the image struct

    self->channels = 1;
    destroy_matrix(self->R);
    destroy_matrix(self->G);
    destroy_matrix(self->B);
    self->R = self->G = self->B = NULL;

    self->greyscale = create_matrix(self->height, self->width);
    memcpy(self->greyscale->array, tmp_greyscale->array, sizeof(float) * self->height * self->width * self->channels);
    destroy_matrix(tmp_greyscale);

}

void greyscale_to_rgb(image_data * self){

    if (self->channels == 3) {
        return;
    }

    self->R = create_matrix(self->height, self->width);
    self->G = create_matrix(self->height, self->width);
    self->B = create_matrix(self->height, self->width);

    for (int y = 0; y < self->height; y++) {
        for (int x = 0; x < self->width; x++) {
            // set each channel of the resulting array to the same value in original array
            /* image->R[(y * image->width) + x] = image->greyscale[(y * image->width) + x]; */
            /* image->G[(y * image->width) + x] = image->greyscale[(y * image->width) + x]; */
            /* image->B[(y * image->width) + x] = image->greyscale[(y * image->width) + x]; */
            self->R->array[(y * self->width) + x] = self->G->array[(y * self->width) + x] = self->B->array[(y * self->width) + x] = self->greyscale->array[(y * self->width) + x];
        }
    }

    // update the image struct
    self->channels = 3;
    destroy_matrix(self->greyscale);
    self->greyscale = NULL;

};

unsigned char ** export_pixels(image_data * self) {

    // initialise the output array.
    unsigned char ** pixels = (unsigned char **) malloc(sizeof(unsigned char *) * self->height);
    for (int y = 0; y < self->height; y++) {
        pixels[y] = (unsigned char *) malloc(sizeof(unsigned char) * self->width * 3);
    }

    //copy contents of float array into output array
    for (int y = 0; y < self->height; y++) {
        unsigned char * row = pixels[y];
        for (int x = 0; x < self->width * 3; x += 3) {
            if (self->channels == 3) {

                row[x + 0] = self->R->array[(y * self->width) + x/3];
                row[x + 1] = self->G->array[(y * self->width) + x/3];
                row[x + 2] = self->B->array[(y * self->width) + x/3];

            } else if (self->channels == 1) {

                for (int c = 0; c < 3; c ++) {

                    row[x+c] = self->greyscale->array[(y*self->width) + x/3];

                }
            }
        }
    }

    return pixels;

}

void process(image_data * self, kernel_configuration type, int kernel_dimensions, float strength) {

    matrix * kernel = create_kernel(type, kernel_dimensions, strength);

    if (self->channels == 1) {

        matrix * tmp = self->greyscale;
        self->greyscale = apply_convolution(self->greyscale, type, kernel, kernel_dimensions);
        destroy_matrix(tmp);

    } else if (self->channels == 3) {

        matrix ** tmp = (matrix **) malloc(sizeof(matrix*) * 3);
        tmp[0] = self->R; tmp[1] = self->G; tmp[2] = self->B;

        self->R = apply_convolution(self->R, type, kernel, kernel_dimensions);
        self->G = apply_convolution(self->G, type, kernel, kernel_dimensions);
        self->B = apply_convolution(self->B, type, kernel, kernel_dimensions);

        for (int i = 0; i < 3; i++) { destroy_matrix(tmp[i]); }
        free(tmp);
    }

}

void reduce_resolution(image_data * self) {

    int step = 2;

    if (self->channels == 1) {

        matrix * tmp = self->greyscale;
        self->greyscale = max_pool_image(self->greyscale, step);
        destroy_matrix(tmp);

    } else if (self->channels == 3) {

        matrix ** tmp = (matrix **) malloc(sizeof(matrix*) * 3);
        tmp[0] = self->R; tmp[1] = self->G; tmp[2] = self->B;

        self->R = mean_pool_image(self->R, step);
        self->G = mean_pool_image(self->G, step);
        self->B = mean_pool_image(self->B, step);

        for (int i = 0; i < 3; i++) { destroy_matrix(tmp[i]); }
        free(tmp);

    }

    self->height /= step;
    self->width /= step;

    return;
}

void invert(image_data * self) {

    for (int y = 0; y < self->height; y ++){
        for (int x = 0; x < self->width; x ++){

            if (self->channels == 3) {

                self->R->array[(y * self->width) + x] = 255.0 - self->R->array[(y * self->width) + x];
                self->G->array[(y * self->width) + x] = 255.0 - self->G->array[(y * self->width) + x];
                self->B->array[(y * self->width) + x] = 255.0 - self->B->array[(y * self->width) + x];

            } else if (self->channels == 1) {

                self->greyscale->array[(y * self->width) + x] = 255.0 - self->greyscale->array[(y * self->width) + x];

            }

        }
    }

}

void resize(image_data * self, float scale_factor) {

    if (self->channels == 1) {

        matrix * tmp = self->greyscale;
        self->greyscale = scale_matrix(self->greyscale, scale_factor, true);
        destroy_matrix(tmp);

        self->height = self->greyscale->y;
        self->width = self->greyscale->x;

    } else if (self->channels == 3) {

        matrix ** tmp = (matrix **) malloc(sizeof(matrix*) * 3);
        tmp[0] = self->R; tmp[1] = self->G; tmp[2] = self->B;

        self->R = scale_matrix(self->R, scale_factor, true);
        self->G = scale_matrix(self->G, scale_factor, true);
        self->B = scale_matrix(self->B, scale_factor, true);

        for (int i = 0; i < 3; i++) { destroy_matrix(tmp[i]); }
        free(tmp);

        self->height = self->R->y;
        self->width = self->R->x;

    }

};

void create_document_outline(image_data * self) {

    /* self->greyscale = translation(self->greyscale, 40, 40); */

    if (self->greyscale == NULL)
        self->rgb_to_greyscale(self);

    float darkness = average_darkness(self->greyscale) / 255.0;

    if (darkness > 0.5)
        self->invert(self);

    self->document_p->scan_image(self->document_p, self->greyscale);
    self->document_p->draw_outlines(self->document_p, self->greyscale);

};

void generate_dataset_from_image(image_data * self, char * path) {

    if (!self->document_p)
        return;

    int total_characters = count_characters_in_document(self->document_p);
    self->data = doc_to_dataset(self->document_p);

    /* for (int i = 0; i < total_characters; i++) */
    /*     printf("Line: %d, Word: %d, Character: %d\n", self->data[i].line_number, self->data[i].word_number, self->data[i].character_number); */

    extend_dataset(self->data, total_characters);
    export_dataset(self->data, total_characters, path);

}

