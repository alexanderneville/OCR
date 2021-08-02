#ifndef convolution_H
#define convolution_H

    #include "./image_processing.h"
    #include "./tree.h"

    typedef enum kernel_configuration_E {
        Mean = 0,
        Gaussian = 1,
        Sharpen = 2
    } kernel_configuration; 

    float * create_kernel(kernel_configuration type, int kernel_dimensions);
    float * apply_convolution(image_data * image, kernel_configuration type, float * kernel, int kernel_dimensions);
    float * max_pool_image(image_data * image, int dimensions, int * height, int * width);
    float * mean_pool_image(image_data * image, int dimensions, int * height, int * width);

#endif // convolution_H
