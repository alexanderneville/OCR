#ifndef matrix_H
#define matrix_H

/* why define a matrix type:

- to simplify function parameters.
- avoid mallocing everywhere.
- can be used as an image channel or an individual letter.

*/
typedef struct matrix_T {
    float * array;
    int x, y;
} matrix;

matrix * create_matrix(int height, int width);

#endif
