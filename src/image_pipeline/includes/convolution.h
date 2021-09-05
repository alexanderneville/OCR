#ifndef convolution_H
#define convolution_H

    #include "./matrix.h"

    typedef enum kernel_configuration_E {
        Mean = 0,
        Gaussian = 1,
        Sharpen = 2
    } kernel_configuration; 

    matrix * create_kernel(kernel_configuration type, int kernel_dimensions, float strength);
    matrix * apply_convolution(matrix * matrix_p, kernel_configuration type, matrix * kernel, int kernel_dimensions);
    matrix * max_pool_image(matrix * matrix_p, int step);
    matrix * mean_pool_image(matrix * matrix_p, int step);

#endif // convolution_H
