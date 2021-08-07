#include "../includes/convolution.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

float * create_kernel(kernel_configuration type, int kernel_dimensions) {

    float * kernel = (float *) malloc(sizeof(float) * kernel_dimensions * kernel_dimensions);
    /* float kernel[kernel_dimensions * kernel_dimensions]; */

    if (type == Mean) {
        for (int i = 0; i < kernel_dimensions * kernel_dimensions; i++) {
            kernel[i] = 1;
        }

    } else if (type == Gaussian) {

        if (kernel_dimensions == 3) {
            float weights[] = {1.0,2.0,1.0,
                               2.0,4.0,2.0,
                               1.0,2.0,1.0};

            memcpy(kernel, weights, sizeof(float) * 9);

        } else if (kernel_dimensions == 5) {
            float weights[] = {1.0,  4.0,  6.0,  4.0,  1.0,
                               4.0,  16.0, 24.0, 16.0, 4.0, 
                               6.0,  24.0, 36.0, 24.0, 6.0,
                               4.0,  16.0, 24.0, 16.0, 4.0,
                               1.0,  4.0,  6.0,  4.0,  1.0};

            memcpy(kernel, weights, sizeof(float) * 25);
        }

    } else if (type == Sharpen) {
        
        //radius must equal 3
        float weights[] = {0.0,  -1.0, 0.0,
                           -1.0, 5.0,  -1.0,
                           0.0,  -1.0, 0.0};

        memcpy(kernel, weights, sizeof(float) * 9);
        
    }

    return kernel;
}

float * apply_convolution(float * pixels, int height, int width, kernel_configuration type, float * kernel, int kernel_dimensions) {

    // create a tree to store the data
    /* node * root = create_node(); */
    float * blurred_image = (float *) malloc(sizeof(float) * height * width);
    int kernel_center = ((kernel_dimensions + 1) / 2) -1;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) { // for every pixel

            float sum_used_weights = 0;
            float weighted_sum = 0;

            for (int j = 0; j < kernel_dimensions * kernel_dimensions; j++) {
                /* printf("%f\n", kernel[j]); */
                // get x and y displacement from center (within the kernel)
                int x_pos = j % kernel_dimensions;
                int y_pos = j / kernel_dimensions;
                x_pos = x_pos - kernel_center;
                y_pos = y_pos - kernel_center;

                // if parts of the kernel fall outside the image boundaries
                // those parts are ignored when calculating the convolved value
                if( y+y_pos >= 0 &&
                    y+y_pos < height &&
                    x+x_pos >= 0 &&
                    x+x_pos < width ) {

                    sum_used_weights += kernel[j];
                    weighted_sum += (pixels[((y+y_pos) * width) + (x+x_pos)] * kernel[j]);

                }
            }
            float intensity;

            if (type == Gaussian || type == Mean) {
                intensity = weighted_sum / sum_used_weights;
            } else if (type == Sharpen){
                intensity = weighted_sum;
            } 
            blurred_image[y*width + x] = intensity;

            /* node_data current_pixel = {(y*image->width + x), intensity}; */
            /* insert_data(root, &current_pixel); */

        }
    }

    return blurred_image;
    /* return root; */
};

float * mean_pool_image(float * pixels, int step, int * height, int * width) {

    int new_height = (* height) / step;
    int new_width = (* width) / step;

    float * pixelated_image = (float *) malloc(sizeof(float) * (new_height) * (new_width));

    for (int y = 0; y < new_height; y+=step) {
        for (int x = 0; x < new_width; x+=step) { // for every pixel
            float kernel_sum  = 0.0;
            if (x + step < new_width && y + step < new_height) {
                for (int j = 0; j < step; j++ ) {
                    for (int i = 0; i < step; i++) {
                        kernel_sum += pixels[(y + j) * (*width) + (x + i)];
                    }
                }
            pixelated_image[(y/step) * new_width + (x/step)] = (kernel_sum / (step * step));
            }
        }
    }

    //update the height and width pointers
    (* height) = new_height;
    (* width) = new_width;

    return pixelated_image;
};

float * max_pool_image(float * pixels, int step, int * height, int * width) {

    int new_height = (* height) / step;
    int new_width = (* width) / step;

    float * pixelated_image = (float *) malloc(sizeof(float) * (new_height) * (new_width));

    for (int y = 0; y < new_height; y+=step) {
        for (int x = 0; x < new_width; x+=step) { // for every pixel
            float kernel_maximum  = 255.0;
            if (x + step < new_width && y + step < new_height) {
                // find the darkest pixel in the image
                for (int j = 0; j < step; j++ ) {
                    for (int i = 0; i < step; i++) {
                        if (pixels[(y + j) * (*width) + (x + i)] < kernel_maximum) {
                            kernel_maximum = pixels[(y + j) * (*width) + (x + i)];
                        }
                    }
                }
            pixelated_image[(y/step) * new_width + (x/step)] = kernel_maximum;
            }
        }
    }
    (* height) = new_height;
    (* width) = new_width;

    return pixelated_image;
};
