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
        

        // methods
        struct matrix_T * (* scale_matrix)  (struct matrix_T * matrix_p, float scale_factor);
        struct matrix_T * (* select_region) (struct matrix_T * matrix_p, int x, int y, int w, int h);
        struct matrix_T * (* horiz_density) (struct matrix_T * matrix_p);
        struct matrix_T * (* vert_density)  (struct matrix_T * matrix_p);
        float (* average_darkness)          (struct matrix_T * matrix_p);

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
    matrix * select_region(matrix * matrix_p, int x, int y, int w, int h);

    matrix * horiz_density(matrix * matrix_p);
    matrix * vert_density(matrix * matrix_p);
    float average_darkness(matrix * matrix_p);

#endif
