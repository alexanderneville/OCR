#ifndef matrix_H
#define matrix_H

typedef struct matrix_T {
    float * array;
    int x, y;
} matrix;

matrix * create_matrix(int height, int width);

#endif
