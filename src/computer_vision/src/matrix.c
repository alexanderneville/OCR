#include <stdlib.h>
#include "../includes/matrix.h"


matrix * create_matrix(int height, int width) {
    
    matrix * new_matrix = (matrix *) malloc(sizeof(matrix));
    new_matrix->array = (float *) (sizeof(float) * height * width);
    new_matrix->x = width;
    new_matrix->y = height;
    return new_matrix;

}
