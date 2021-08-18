#ifndef scaling_H
#define scaling_H


typedef struct remainder_T {
    int floored;
    float decimal;
} remainder;


void bilinear_approximation(float * pixels, float * gradient, float * intercept);
void bicubic_approximation(float * pixels, float * a, float * b, float * c, float * d);

void adjust_scale_factor(int * height, int * width, float * scale_factor, float step);

float * scale_matrix(float * pixels, int * height, int * width, float scale_factor);

#endif

