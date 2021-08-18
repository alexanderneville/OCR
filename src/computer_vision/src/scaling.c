#include <stdlib.h>
#include <stdbool.h>

#include "../includes/scaling.h"

void bilinear_approximation(float * pixels, float * gradient, float * intercept) {
    (*intercept) = pixels[0];
    (*gradient) = pixels[1] - pixels[0];
};

void bicubic_approximation(float * pixels, float * a, float * b, float * c, float * d) {};

remainder calc_remainder(float N){

    remainder result;
    result.floored = N;
    result.decimal = N - result.floored;
    return result;

}

bool test_float(float x) {

    remainder result = calc_remainder(x);
    if (result.decimal > 0) {
        return false;
    } else {
        return true;
    }

}
void adjust_scale_factor(int * height, int * width, float * scale_factor, float step) {
    
    // make sure the scale is valid

    bool valid_factor = false;

    (*scale_factor) -= step; 

    while (valid_factor == false) {

        (*scale_factor) += step;

        bool valid_height = test_float((*scale_factor) * (* height));
        bool valid_width = test_float((*scale_factor) * (* width));

        if (valid_height && valid_factor) valid_factor = true;

    }

    // update variables
    (* height) *= (*scale_factor);
    (* width) *= (*scale_factor);

}

float * scale_matrix(float * pixels, int * height, int * width, float scale_factor) {

    int new_height, new_width;
    float * new_pixels;

    if (scale_factor == 1) {

        return pixels;

    } else if (scale_factor < 1) {

        adjust_scale_factor(&new_height, &new_width, &scale_factor, -0.01);
        new_pixels = (float *) malloc(sizeof(float) * new_height * new_width); // create a new matrix with the determined size

    } else {

        adjust_scale_factor(height, width, &scale_factor, 0.01);
        new_pixels = (float *) malloc(sizeof(float) * new_height * new_width); // create a new matrix with the determined size

    }

    for ( int y = 0; y < new_height; y ++){
        for ( int x = 0; x < new_width; x ++) {

        }
    }

    return new_pixels;
};
