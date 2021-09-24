#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../includes/matrix.h"


matrix * create_matrix(int height, int width) {
    
    matrix * new_matrix_p = (matrix *) malloc(sizeof(matrix));
    new_matrix_p->array = (float *) malloc(sizeof(float) * height * width);
    new_matrix_p->x = width;
    new_matrix_p->y = height;


    //methods

    new_matrix_p->scale_matrix       =     &scale_matrix;
    new_matrix_p->select_region      =     &select_region;
    new_matrix_p->average_darkness   =     &average_darkness;
    new_matrix_p->horiz_density      =     &horiz_density;
    new_matrix_p->vert_density       =     &vert_density;
    new_matrix_p->paste              =     &paste;
    new_matrix_p->translation        =     &translation;

    return new_matrix_p;

}
void destroy_matrix(matrix * old_matrix) {

    if (!old_matrix)
        return;

    free(old_matrix->array);
    free(old_matrix);
    return;
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
    int escape_counter = 0; 

    while (valid_factor == false) {

        (*scale_factor) += step;
        escape_counter ++;

        bool valid_height = test_float((*scale_factor) * (* height));
        bool valid_width = test_float((*scale_factor) * (* width));

        if (valid_height && valid_width) valid_factor = true;

        if (valid_width && escape_counter > 1000000)
            valid_factor = true;

    }

    // update variables
    (* height) *= (*scale_factor);
    (* width) *= (*scale_factor);

}

matrix * scale_matrix(matrix * matrix_p, float scale_factor, bool adjust) {

    /* printf("\n"); */
    /* printf("(%d, %d)\n", matrix_p->x, matrix_p->y); */
    float new_height = matrix_p->y;
    float new_width = matrix_p->x;

    if (adjust) {
        if (scale_factor > 1) {
            adjust_scale_factor(&new_height, &new_width, &scale_factor, 0.0000001);
        } else if (scale_factor < 1) {
            adjust_scale_factor(&new_height, &new_width, &scale_factor, -0.0000001);
        }
    } else {
        new_height *= scale_factor;
        new_width *= scale_factor;
    }

    int int_height = new_height;
    int int_width = new_width;

    /* printf("(%f, %f)\n", new_width, new_height); */
    /* printf("(%d, %d)\n", int_width, int_height); */

    matrix * scaled_matrix = create_matrix(int_height, int_width);

    for (int y = 0; y < int_height; y ++){
        for ( int x = 0; x < int_width; x ++) {

            /* printf("(%d, %d): ", x, y); */
            float x_as_frac = x / new_width;
            float y_as_frac = y / new_height;
            /* printf("(%f, %f)\n", x_as_frac, y_as_frac); */

            pos x_pos = calc_pos(x_as_frac * matrix_p->x);
            pos y_pos = calc_pos(y_as_frac * matrix_p->y);

            float known_points[4] = {
                                    matrix_p->array[((y_pos.floored + 0) * matrix_p->x) + x_pos.floored + 0],
                                    matrix_p->array[((y_pos.floored + 0) * matrix_p->x) + x_pos.floored + 1],
                                    matrix_p->array[((y_pos.floored + 1) * matrix_p->x) + x_pos.floored + 0],
                                    matrix_p->array[((y_pos.floored + 1) * matrix_p->x) + x_pos.floored + 1]
                                   };

            float value = bilinear_approximation(known_points, x_pos.decimal, y_pos.decimal);
            if (isnan(value) || value > 10000 || value < 0)
                value = 0.0;
            /* printf("%f\n", value); */
            scaled_matrix->array[(y * scaled_matrix->x) + x] = value;

        }
    }

    return scaled_matrix;

};

matrix * select_region(matrix * matrix_p, int x, int y, int w, int h) {

    // select a portion of an exiting matrix

    matrix * new_matrix_p = create_matrix(h, w);

    for (int i = 0; i < new_matrix_p->y; i ++) {
        for (int j = 0; j < new_matrix_p->x; j ++) {
            
            new_matrix_p->array[(i * new_matrix_p->x) + j] = matrix_p->array[((y + i) * matrix_p->x) + x + j];

        }
    }
    return new_matrix_p;

}

matrix * horiz_density(matrix * matrix_p){
    
    matrix * densities = create_matrix(matrix_p->y, 1);

    for (int y = 0; y < matrix_p->y; y++) {
        float sum = 0;
        for (int x = 0; x < matrix_p->x; x++) {
            sum += matrix_p->array[(y * matrix_p->x) + x];
        }
        sum /= matrix_p->x;
        sum /= 255.0;
        densities->array[y] = sum;
    }

    return densities;

}

matrix * vert_density(matrix * matrix_p){

    matrix * densities = create_matrix(1, matrix_p->x);

    for (int x = 0; x < matrix_p->x; x++) {
        float sum = 0;
        for (int y = 0; y < matrix_p->y; y++) {
            sum += matrix_p->array[(y * matrix_p->x) + x];
        }
        sum /= matrix_p->y;
        sum /= 255.0;
        densities->array[x] = sum;
    }

    return densities;

}


float average_darkness(matrix * matrix_p) {

    float sum = 0.0;

    for (int i = 0; i < (matrix_p->x * matrix_p->y); i ++) {

        sum += matrix_p->array[i];

    }

    sum /= (matrix_p->x * matrix_p->y);

    return sum;

}

matrix * paste(matrix* fg, matrix * bg) {

    // past the fg matrix centrally on top of the bg matrix.

    matrix * new_matrix = create_matrix(bg->y, bg->x);

    memcpy(new_matrix->array, bg->array, sizeof(float) * bg->y * bg->x);
    
    int x_offset = (bg->x - fg->x) / 2;
    int y_offset = (bg->y - fg->y) / 2;

    for (int y = 0; y < fg->y; y++) {
        for (int x = 0; x < fg->x; x++) {

            new_matrix->array[((y_offset + y) * new_matrix->x) + x_offset + x] = fg->array[(y * fg->x) + x];

        }
    }

    return new_matrix;
}


matrix * translation(matrix * matrix_p, int x_offset, int y_offset) {

    matrix * new_matrix = create_matrix(matrix_p->y, matrix_p->x);

    for (int y = 0; y < matrix_p->y; y ++) {
        for (int x = 0; x < matrix_p->x; x ++) {

            int x_pos = (matrix_p->x + x + x_offset) % matrix_p->x;
            int y_pos = (matrix_p->y + y + y_offset) % matrix_p->y;

            new_matrix->array[(y * new_matrix->x) + x] = matrix_p->array[(y_pos * matrix_p->x) + x_pos];

        }
    }

    return new_matrix;
}
