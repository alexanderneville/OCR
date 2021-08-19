#ifndef matrix_H
#define matrix_H

#include <stdbool.h>

    /* why define a matrix type:

    - to simplify function parameters.
    - avoid mallocing everywhere.
    - can be used as an image channel or an individual letter.

    */
    typedef struct matrix_T {
        float * array;
        int x, y;
    } matrix;

    typedef struct pos_T {
        int floored;
        float decimal;
    } pos;


    /* void bicubic_approximation(float * pixels, float * a, float * b, float * c, float * d); */


    matrix * create_matrix(int height, int width);

    pos calc_pos(float N);
    bool test_float(float x);
    void adjust_scale_factor(float * height, float * width, float * scale_factor, float step);

    matrix * scale_matrix(matrix * matrix_p, float scale_factor);

#endif
