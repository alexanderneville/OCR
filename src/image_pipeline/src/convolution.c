#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../includes/convolution.h"
#include "../includes/matrix.h"

matrix *
create_kernel(kernel_configuration type, float kernel_dimensions)
{
  int trunc_dimensions = kernel_dimensions;

  // ensure dimensions are not evil
  if ((trunc_dimensions % 2) == 0)
    kernel_dimensions++;
  matrix *kernel_matrix;

  if (type == Mean) {
    // fill with the value 1.0
    kernel_matrix = create_matrix(kernel_dimensions, kernel_dimensions);
    for (int i = 0; i < kernel_dimensions * kernel_dimensions; i++) {
      kernel_matrix->array[i] = 1;
    }
  } else if (type == Gaussian) {
    // template kernel for preset "Gaussion"
    float weights[] = { 1.0, 4.0, 6.0, 4.0, 1.0,
      4.0, 16.0, 24.0, 16.0, 4.0,
      6.0, 24.0, 36.0, 24.0, 6.0,
      4.0, 16.0, 24.0, 16.0, 4.0,
      1.0, 4.0, 6.0, 4.0, 1.0
    };
    float scale_factor = kernel_dimensions / 5.0;
    matrix *tmp_kernel = create_matrix(5, 5);

    memcpy(tmp_kernel->array, weights, sizeof(float) * 25);
    kernel_matrix = scale_matrix(tmp_kernel, scale_factor, false);
  } else if (type == Sharpen) {
    // template kernel for preset "Sharpen"
    float weights[] = { 0.0, -1.0, 0.0,
      -1.0, 5.0, -1.0,
      0.0, -1.0, 0.0
    };
    float scale_factor = kernel_dimensions / 3.0;
    matrix *tmp_kernel = create_matrix(3, 3);

    // copy values into a matrix
    memcpy(tmp_kernel->array, weights, sizeof(float) * 9);
    // scale the matrix to reach desired convolution strength
    kernel_matrix = tmp_kernel->scale_matrix(tmp_kernel, scale_factor, false);
  }
  return kernel_matrix;
}

matrix *
apply_convolution(matrix * matrix_p, kernel_configuration type,
                  matrix * kernel, int kernel_dimensions)
{
  matrix *blurred_image = create_matrix(matrix_p->y, matrix_p->x);
  int kernel_center = ((kernel_dimensions + 1) / 2) - 1;

  for (int y = 0; y < matrix_p->y; y++) {
    for (int x = 0; x < matrix_p->x; x++) {
      float sum_used_weights = 0;
      float weighted_sum = 0;

      for (int j = 0; j < kernel_dimensions * kernel_dimensions; j++) {
        int x_pos = j % kernel_dimensions;
        int y_pos = j / kernel_dimensions;

        x_pos = x_pos - kernel_center;
        y_pos = y_pos - kernel_center;
        if (y + y_pos >= 0 &&
            y + y_pos < matrix_p->y &&
            x + x_pos >= 0 && x + x_pos < matrix_p->x) {
          sum_used_weights += kernel->array[j];
          weighted_sum +=
            (matrix_p->array[((y + y_pos) * matrix_p->x) + (x + x_pos)] *
             kernel->array[j]);
        }
      }
      float intensity;

      if (type == Gaussian || type == Mean) {
        intensity = weighted_sum / sum_used_weights;
      } else if (type == Sharpen) {
        intensity = weighted_sum;
      }
      blurred_image->array[y * blurred_image->x + x] = intensity;
    }
  }
  return blurred_image;
};

void *
convolution_thread_func(void *arg)
{
  // same as above, although thread safe

  // unpack the agruments passed to the function
  convolution_arg *arguements = (convolution_arg *) arg;

  // find the center of the kernel
  int kernel_center = ((arguements->kernel_dimensions + 1) / 2) - 1;

  for (int y = arguements->offset; y < arguements->matrix_p->y;
       y += arguements->step) {
       // begin at the specified row and step by the specified argument.
       // the caller determines these values to keep threads from colliding in the data structure
    for (int x = 0; x < arguements->matrix_p->x; x++) {
      float sum_used_weights = 0;
      float weighted_sum = 0;

      for (int j = 0;
           j < arguements->kernel_dimensions * arguements->kernel_dimensions;
           j++) {
        // iterate over all positions in the kernel 
                
        // obtain x coordinates by modding the counter by width
        // obtain y coordinates by flooring the counter by width
        int x_pos = j % arguements->kernel_dimensions;
        int y_pos = j / arguements->kernel_dimensions;

        // ensure the the corresponding pixel fits within the image
        x_pos = x_pos - kernel_center;
        y_pos = y_pos - kernel_center;
        if (y + y_pos >= 0 &&
            y + y_pos < arguements->matrix_p->y &&
            x + x_pos >= 0 && x + x_pos < arguements->matrix_p->x) {
          sum_used_weights += arguements->kernel->array[j];
          weighted_sum +=
            (arguements->
             matrix_p->array[((y + y_pos) * arguements->matrix_p->x) +
                             (x + x_pos)] * arguements->kernel->array[j]);
        }
      }
      float intensity;

      // apply weighting depending on kernel type
      if (arguements->type == Gaussian || arguements->type == Mean) {
        intensity = weighted_sum / sum_used_weights;
      } else if (arguements->type == Sharpen) {
        intensity = weighted_sum;
      }
      arguements->dest->array[(y * arguements->dest->x) + x] = intensity;
    }
  }
  return NULL;
}

matrix *
mean_pool_image(matrix * matrix_p, int step)
{
  int new_height = matrix_p->y / step;
  int new_width = matrix_p->x / step;
  matrix *pixelated_image = create_matrix(new_height, new_width);

  for (int y = 0; y < matrix_p->y; y += step) {
    for (int x = 0; x < matrix_p->x; x += step) {
      // iterate over all image pixels
      float kernel_sum = 0.0;

      if (x + step < matrix_p->x && y + step < matrix_p->y) {
        for (int j = 0; j < step; j++) {
          for (int i = 0; i < step; i++) {
            // sum the values within the frame
            kernel_sum += matrix_p->array[(y + j) * matrix_p->x + (x + i)];
          }
        }
        // as this is mean pool, average the sum per frame
        pixelated_image->array[(y / step) * new_width + (x / step)] =
          (kernel_sum / (step * step));
      }
    }
  }
  return pixelated_image;
};

matrix *
max_pool_image(matrix * matrix_p, int step)
{
  int new_height = matrix_p->y / step;
  int new_width = matrix_p->x / step;
  matrix *pixelated_image = create_matrix(new_height, new_width);

  for (int y = 0; y < matrix_p->y; y += step) {
    for (int x = 0; x < matrix_p->x; x += step) {
      // iterate over all image pixels
      // 255 to beat
      float kernel_maximum = 255.0;

      if (x + step < matrix_p->x && y + step < matrix_p->y) {
        for (int j = 0; j < step; j++) {
          for (int i = 0; i < step; i++) {
            if (matrix_p->array[(y + j) * matrix_p->x + (x + i)] <
                kernel_maximum) {
              kernel_maximum =
                matrix_p->array[(y + j) * matrix_p->x + (x + i)];
            }
          }
        }
        // set the output pixel to the largest found
        pixelated_image->array[(y / step) * new_width + (x / step)] =
          kernel_maximum;
      }
    }
  }
  return pixelated_image;
};
