#include "../includes/convolution.h"
#include "../includes/matrix.h"
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

matrix * apply_convolution(matrix * matrix_p, kernel_configuration type, float * kernel, int kernel_dimensions) {

    // create a tree to store the data
    /* node * root = create_node(); */
    matrix * blurred_image = create_matrix(matrix_p->y, matrix_p->x);
    int kernel_center = ((kernel_dimensions + 1) / 2) -1;

    for (int y = 0; y < matrix_p->y; y++) {
        for (int x = 0; x < matrix_p->x; x++) { // for every pixel

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
                    y+y_pos < matrix_p->y &&
                    x+x_pos >= 0 &&
                    x+x_pos < matrix_p->x ) {

                    sum_used_weights += kernel[j];
                    weighted_sum += (matrix_p->array[((y+y_pos) * matrix_p->x) + (x+x_pos)] * kernel[j]);

                }
            }
            float intensity;

            if (type == Gaussian || type == Mean) {
                intensity = weighted_sum / sum_used_weights;
            } else if (type == Sharpen){
                intensity = weighted_sum;
            } 
            blurred_image->array[y*blurred_image->x + x] = intensity;

            /* node_data current_pixel = {(y*image->width + x), intensity}; */
            /* insert_data(root, &current_pixel); */

        }
    }

    return blurred_image;
    /* return root; */
};

matrix * mean_pool_image(matrix * matrix_p, int step) {

    int new_height = matrix_p->y / step;
    int new_width = matrix_p->x / step;

    matrix * pixelated_image = create_matrix(new_height, new_width);

    for (int y = 0; y < new_height; y+=step) {
        for (int x = 0; x < new_width; x+=step) { // for every pixel
            float kernel_sum  = 0.0;
            if (x + step < new_width && y + step < new_height) {
                for (int j = 0; j < step; j++ ) {
                    for (int i = 0; i < step; i++) {
                        kernel_sum += matrix_p->array[(y + j) * matrix_p->x + (x + i)];
                    }
                }
            pixelated_image->array[(y/step) * new_width + (x/step)] = (kernel_sum / (step * step));
            }
        }
    }

    //update the height and width pointers

    return pixelated_image;
};

matrix * max_pool_image(matrix * matrix_p, int step) {

    int new_height = matrix_p->y / step;
    int new_width = matrix_p->x / step;

    matrix * pixelated_image = create_matrix(new_height, new_width);

    for (int y = 0; y < new_height; y+=step) {
        for (int x = 0; x < new_width; x+=step) { // for every pixel
            float kernel_maximum  = 255.0;
            if (x + step < new_width && y + step < new_height) {
                // find the darkest pixel in the image
                for (int j = 0; j < step; j++ ) {
                    for (int i = 0; i < step; i++) {
                        if (matrix_p->array[(y + j) * matrix_p->x + (x + i)] < kernel_maximum) {
                            kernel_maximum = matrix_p->array[(y + j) * matrix_p->x + (x + i)];
                        }
                    }
                }
            pixelated_image->array[(y/step) * new_width + (x/step)] = kernel_maximum;
            }
        }
    }

    return pixelated_image;
};
