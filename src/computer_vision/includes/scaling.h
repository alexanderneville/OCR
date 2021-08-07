#ifndef scaling_H
#define scaling_H

void bilinear_approximation(float * pixels, float * gradient, float * intercept);
void bicubic_approximation(float * pixels, float * a, float * b, float * c, float * d);

#endif

