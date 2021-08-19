#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "../includes/matrix.h"


matrix * create_matrix(int height, int width) {
    
    matrix * new_matrix = (matrix *) malloc(sizeof(matrix));
    new_matrix->array = (float *) malloc(sizeof(float) * height * width);
    new_matrix->x = width;
    new_matrix->y = height;
    return new_matrix;

}

float bilinear_approximation(float * known_points, float x, float y) {

    float output = 0.0;

    output += ((1 - x) * (1  - y) * known_points[0]);
    output += (     x  * (1  - y) * known_points[1]);
    output += ((1 - x) *       y  * known_points[2]);
    output += (     x  *       y  * known_points[3]);

    return output;

};

/* void bicubic_approximation(float * pixels, float * a, float * b, float * c, float * d) {}; */

pos calc_pos(float N){

    pos result;
    result.floored = N;
    result.decimal = N - result.floored;
    return result;

}

bool test_float(float x) {

    pos result = calc_pos(x);
    if (result.decimal > 0) {
        return false;
    } else {
        return true;
    }

}
void adjust_scale_factor(float * height, float * width, float * scale_factor, float step) {
    
    // make sure the scale factor produces a valid dimension

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

matrix * scale_matrix(matrix * matrix_p, float scale_factor) {

    float new_height = matrix_p->y;
    float new_width = matrix_p->x;

    // adjust the scale factor so it produces an exact scale

    if (scale_factor > 1) {

        adjust_scale_factor(&new_height, &new_width, &scale_factor, 0.001);

    } else if (scale_factor < 1) {

        adjust_scale_factor(&new_height, &new_width, &scale_factor, -0.001);

    }

    printf("scale factor: %f, x: %f, y: %f \n", scale_factor, new_width, new_height);
    
    matrix * scaled_matrix = create_matrix(new_height, new_width);

    for (int y = 0; y < new_height; y ++){
        for ( int x = 0; x < new_width; x ++) {

            float x_as_frac = x / new_width;
            float y_as_frac = y / new_height;

            pos x_pos = calc_pos(x_as_frac * matrix_p->x);
            pos y_pos = calc_pos(y_as_frac * matrix_p->y);

            float known_points[4] = {
                                    matrix_p->array[((y_pos.floored + 0) * matrix_p->x) + x_pos.floored + 0],
                                    matrix_p->array[((y_pos.floored + 0) * matrix_p->x) + x_pos.floored + 1],
                                    matrix_p->array[((y_pos.floored + 1) * matrix_p->x) + x_pos.floored + 0],
                                    matrix_p->array[((y_pos.floored + 1) * matrix_p->x) + x_pos.floored + 1]
                                   };

            scaled_matrix->array[(y * scaled_matrix->x) + x] = bilinear_approximation(known_points, x_pos.decimal, y_pos.decimal);


        }
    }

    return scaled_matrix;

};
