#include "../includes/image_processing.h"
#include "../includes/character_detection.h"


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
