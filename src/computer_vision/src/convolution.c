#include "../includes/convolution.h"
#include <stdlib.h>

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
            kernel = weights;

        } else if (kernel_dimensions == 5) {
            float weights[] = {1.0, 4.0, 6.0, 4.0, 1.0,
                               4.0, 16.0, 24.0, 16.0, 4.0, 
                               6.0, 24.0, 36.0, 24.0, 6.0,
                               4.0, 16.0, 24.0, 16.0, 4.0,
                               1.0, 4.0, 6.0, 4.0, 1.0};
            kernel = weights;
        }

    }

    return kernel;
}

node * apply_convolution(image_data * image, float * kernel, int kernel_dimensions) {

    // create a tree to store the data
    node * root = create_node();
    int kernel_center = ((kernel_dimensions + 1) / 2) -1;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) { // for every pixel

            float sum_used_weights = 0;
            float weighted_sum = 0;

            for (int j = 0; j < kernel_dimensions * kernel_dimensions; j++) {
                // get x and y displacement from center (within the kernel)
                int x_pos = j % kernel_dimensions;
                int y_pos = j / kernel_dimensions;
                x_pos = x_pos - kernel_center;
                y_pos = y_pos - kernel_center;

                // if parts of the kernel fall outside the image boundaries
                // those parts are ignored when calculating the convolved value
                if( y+y_pos >= 0 &&
                    y+y_pos < image->height &&
                    x+x_pos >= 0 &&
                    x+x_pos < image->width ) {

                    sum_used_weights += kernel[j];
                    weighted_sum += (image->pixels[((y+y_pos) * image->width) + (x+x_pos)] * kernel[j]);

                }
            }

            node_data current_pixel = {(y*image->width + x), weighted_sum / sum_used_weights};
            insert_data(root, &current_pixel);

        }
    }
    return root;
};
