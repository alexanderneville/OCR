#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../includes/convolution.h"
#include "../includes/matrix.h"

matrix * create_kernel(kernel_configuration type, int kernel_dimensions, float strength) {

    if ((kernel_dimensions % 2) == 0)
        kernel_dimensions ++;

    matrix * kernel_matrix;

    if (type == Mean) {
        kernel_matrix = create_matrix(kernel_dimensions, kernel_dimensions);
        for (int i = 0; i < kernel_dimensions * kernel_dimensions; i++) {
            kernel_matrix->array[i] = 1;
        }

    } else if (type == Gaussian) {

        float weights[] = {1.0,  4.0,  6.0,  4.0,  1.0,
                           4.0,  16.0, 24.0, 16.0, 4.0, 
                           6.0,  24.0, 36.0, 24.0, 6.0,
                           4.0,  16.0, 24.0, 16.0, 4.0,
                           1.0,  4.0,  6.0,  4.0,  1.0};

        for (int i = 0; i < 25; i++) {
            weights[i] = weights[i] + (weights[i] * strength);
        }
        float scale_factor = kernel_dimensions / 5.0;
        matrix * tmp_kernel = create_matrix(5, 5);
        memcpy(tmp_kernel->array, weights, sizeof(float) * 25);
        kernel_matrix = scale_matrix(tmp_kernel, scale_factor, false);

    } else if (type == Sharpen) {

        //radius must equal 3
        float weights[] = { 0.0,  -1.0,  0.0,
                           -1.0,   5.0, -1.0,
                            0.0,  -1.0,  0.0 };

        for (int i = 0; i < 9; i++) {
            weights[i] = weights[i] + weights[i] * strength;
        }
        float scale_factor = kernel_dimensions / 3.0;
        matrix * tmp_kernel = create_matrix(3, 3);
        memcpy(tmp_kernel->array, weights, sizeof(float) * 9);
        kernel_matrix = tmp_kernel->scale_matrix(tmp_kernel, scale_factor, false);

    }

    return kernel_matrix;
}

matrix * apply_convolution(matrix * matrix_p, kernel_configuration type, matrix * kernel, int kernel_dimensions) {

    // create a tree to store the data
    /* node * root = create_node(); */
    matrix * blurred_image = create_matrix(matrix_p->y, matrix_p->x);
    int kernel_center = ((kernel_dimensions + 1) / 2) - 1;

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

                    sum_used_weights += kernel->array[j];
                    weighted_sum += (matrix_p->array[((y+y_pos) * matrix_p->x) + (x+x_pos)] * kernel->array[j]);

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

void * convultion_thread_func(void * arg) {

    convultion_arg * arguements = (convultion_arg *) arg;
    /* printf("%d\n", arguements->offset); */

    int kernel_center = ((arguements->kernel_dimensions + 1) / 2) - 1;

    for (int y = arguements->offset; y < arguements->matrix_p->y; y += arguements->step) {
        for (int x = 0; x < arguements->matrix_p->x; x++) { // for every pixel

            float sum_used_weights = 0;
            float weighted_sum = 0;

            for (int j = 0; j < arguements->kernel_dimensions * arguements->kernel_dimensions; j++) {
                /* printf("%f\n", kernel[j]); */
                // get x and y displacement from center (within the kernel)
                int x_pos = j % arguements->kernel_dimensions;
                int y_pos = j / arguements->kernel_dimensions;
                x_pos = x_pos - kernel_center;
                y_pos = y_pos - kernel_center;

                // if parts of the kernel fall outside the image boundaries
                // those parts are ignored when calculating the convolved value
                if( y+y_pos >= 0 &&
                    y+y_pos < arguements->matrix_p->y &&
                    x+x_pos >= 0 &&
                    x+x_pos < arguements->matrix_p->x ) {

                    sum_used_weights += arguements->kernel->array[j];
                    weighted_sum += (arguements->matrix_p->array[((y+y_pos) * arguements->matrix_p->x) + (x+x_pos)] * arguements->kernel->array[j]);

                }
            }
            float intensity;

            if (arguements->type == Gaussian || arguements->type == Mean) {
                intensity = weighted_sum / sum_used_weights;
            } else if (arguements->type == Sharpen){
                intensity = weighted_sum;
            } 
            arguements->dest->array[(y * arguements->dest->x) + x] = intensity;
        }
    }

    return NULL;
}

matrix * mean_pool_image(matrix * matrix_p, int step) {

    int new_height = matrix_p->y / step;
    int new_width = matrix_p->x / step;

    matrix * pixelated_image = create_matrix(new_height, new_width);

    for (int y = 0; y < matrix_p->y; y+=step) {
        for (int x = 0; x < matrix_p->x; x+=step) { // for every pixel
            float kernel_sum  = 0.0;
            if (x + step < matrix_p->x && y + step < matrix_p->y) {
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

    for (int y = 0; y < matrix_p->y; y+=step) {
        for (int x = 0; x < matrix_p->x; x+=step) { // for every pixel
            float kernel_maximum  = 255.0;
            if (x + step < matrix_p->x && y + step < matrix_p->y) {
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
