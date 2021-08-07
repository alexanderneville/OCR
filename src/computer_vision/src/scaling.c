#include <stdlib.h>

void bilinear_approximation(float * pixels, float * gradient, float * intercept) {
    (*intercept) = pixels[0];
    (*gradient) = pixels[1] - pixels[0];
};
void bicubic_approximation(float * pixels, float * a, float * b, float * c, float * d) {};
