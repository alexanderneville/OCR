#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <png.h>

#include "../includes/image_io.h"
#include "../includes/image_processing.h"
#include "../includes/clustering.h"

image_data *
setup()
{
  int height, width, channels, color_type, bit_depth;
  unsigned char **pixels1 =
    read_image("./k_means_test.png", &height, &width, &channels, &bit_depth,
               &color_type);
  image_data *image_p = initialise_data(pixels1, height, width, 3);

  return image_p;
}

int
main(int argc, char **argv)
{

  image_data *image_p = setup();

  image_p->rgb_to_greyscale(image_p);
  image_p->invert(image_p);
  point ** centroids = k_means_segmentation(image_p->greyscale, 4);

  for (int centroid = 0; centroid < 4; centroid++) {
      printf("x: %d, y: %d\n", centroids[centroid]->x, centroids[centroid]->y);
  }
  image_p->invert(image_p);
  image_p->greyscale_to_rgb(image_p);
  outline_clusters(image_p->R, centroids, 4);

  unsigned char **pixels = image_p->export_pixels(image_p);
  write_image("test_output_dir/test_kmeans.png", pixels, image_p->height,
              image_p->width, image_p->channels, 8, 2);
  return 0;

}
