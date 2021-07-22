#include "./includes/convolution.h"
#include <stdlib.h>

float * create_kernel(kernel_configuration type, int kernel_dimensions, float * sum) {

    float * kernel = (float *) malloc(sizeof(float) * kernel_dimensions * kernel_dimensions);

    if (type == Mean) {
        for (int i = 0; i < kernel_dimensions * kernel_dimensions; i++) {
            kernel[i] = 1.0;
        }
        (* sum) = kernel_dimensions * kernel_dimensions;

    } else if (type == Gaussian) {

        

    }



    return kernel;

};
void apply_convolution(image_data * image, float * kernel, int kernel_dimensions) {

};
