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
        struct matrix_T * (* scale_matrix)  (struct matrix_T * matrix_p, float scale_factor, bool adjust);
        struct matrix_T * (* select_region) (struct matrix_T * matrix_p, int x, int y, int w, int h);
        struct matrix_T * (* horiz_density) (struct matrix_T * matrix_p);
        struct matrix_T * (* vert_density)  (struct matrix_T * matrix_p);
        float (* average_darkness)          (struct matrix_T * matrix_p);
        struct matrix_T * (* paste) (struct matrix_T * fg, struct matrix_T * bg);
        struct matrix_T * (* translation) (struct matrix_T * matrix_p, int x_offset, int y_offset);

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

    matrix * scale_matrix(matrix * matrix_p, float scale_factor, bool adjust);
    matrix * select_region(matrix * matrix_p, int x, int y, int w, int h);

    matrix * horiz_density(matrix * matrix_p);
    matrix * vert_density(matrix * matrix_p);
    float average_darkness(matrix * matrix_p);

    matrix * paste(matrix* fg, matrix * bg);
    matrix * translation(matrix * matrix_p, int x_offset, int y_offset);

#endif
