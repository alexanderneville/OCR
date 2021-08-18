#ifndef convolution_H
#define convolution_H

    #include "./image_processing.h"
    #include "./matrix.h"
    #include "./tree.h"

    typedef enum kernel_configuration_E {
        Mean = 0,
        Gaussian = 1,
        Sharpen = 2
    } kernel_configuration; 

    float * create_kernel(kernel_configuration type, int kernel_dimensions);
    matrix * apply_convolution(matrix * matrix_p, kernel_configuration type, float * kernel, int kernel_dimensions);
    matrix * max_pool_image(matrix * matrix_p, int step);
    matrix * mean_pool_image(matrix * matrix_p, int step);

#endif // convolution_H
