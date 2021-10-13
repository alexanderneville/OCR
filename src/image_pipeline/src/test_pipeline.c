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

void
fail(int number, char *message)
{
  printf("Test %d failed!\n", number);
  printf("%s", message);
  exit(-1);
}

image_data *
setup()
{
  int height, width, channels, color_type, bit_depth;
  unsigned char **pixels1 =
    read_image("test_image.png", &height, &width, &channels, &bit_depth,
               &color_type);
  image_data *image_p = initialise_data(pixels1, height, width, 3);

  return image_p;
}

void
teardown(image_data * image_p)
{
  destroy_image_data(image_p);
}

void
test_load()
{
  printf("testing read_image() and initialise_data() ... ");
  image_data *image_p = setup();

  teardown(image_p);
  printf("passed!\n");
}

void
test_export()
{
  printf("testing export_pixels() ... ");
  image_data *image_p = setup();
  unsigned char **pixels = image_p->export_pixels(image_p);

  teardown(image_p);
  printf("passed!\n");
}

void
test_save()
{
  printf("testing write_image() ... ");
  image_data *image_p = setup();
  unsigned char **pixels = image_p->export_pixels(image_p);

  write_image("test_output_dir/test_save.png", pixels, image_p->height,
              image_p->width, image_p->channels, 8, 2);
  teardown(image_p);
  printf("passed!\n");
}

void
test_switch_channel_num()
{
  printf("testing rgb_to_greyscale() and greyscale_to_rgb() ... ");
  image_data *image_p = setup();

  assert(image_p->channels == 3);
  image_p->rgb_to_greyscale(image_p);
  assert(image_p->channels == 1);
  image_p->greyscale_to_rgb(image_p);
  assert(image_p->channels == 3);
  teardown(image_p);
  printf("passed!\n");
}

void
test_invert_colours()
{
  printf("testing invert() ... ");
  image_data *image_p = setup();

  image_p->invert(image_p);
  unsigned char **pixels = image_p->export_pixels(image_p);

  write_image("test_output_dir/test_invert_colours.png", pixels,
              image_p->height, image_p->width, image_p->channels, 8, 2);
  teardown(image_p);
  printf("passed!\n");
}

void
test_scale_smaller()
{
  printf("testing resize(0.6) ... ");
  image_data *image_p = setup();
  int original = image_p->width;

  image_p->resize(image_p, 0.6);
  assert(image_p->width < original);
  unsigned char **pixels = image_p->export_pixels(image_p);

  write_image("test_output_dir/test_scale_smaller.png", pixels,
              image_p->height, image_p->width, image_p->channels, 8, 2);
  teardown(image_p);
  printf("passed!\n");
}

void
test_scale_larger()
{
  printf("testing resize(1.2) ... ");
  image_data *image_p = setup();
  int original = image_p->width;

  image_p->resize(image_p, 1.2);
  assert(image_p->width > original);
  unsigned char **pixels = image_p->export_pixels(image_p);

  write_image("test_output_dir/test_scale_larger.png", pixels,
              image_p->height, image_p->width, image_p->channels, 8, 2);
  teardown(image_p);
  printf("passed!\n");
}

void
test_pooling()
{
  printf("testing reduce_resolution() ... ");
  image_data *image_p = setup();
  int original = image_p->width;

  image_p->reduce_resolution(image_p);
  int after = image_p->width;

  assert(after < original);
  unsigned char **pixels = image_p->export_pixels(image_p);

  write_image("test_output_dir/test_pooling.png", pixels, image_p->height,
              image_p->width, image_p->channels, 8, 2);
  image_p->rgb_to_greyscale(image_p);
  image_p->reduce_resolution(image_p);
  assert(image_p->width < after);
  teardown(image_p);
  printf("passed!\n");
}

void
test_convolution()
{
  printf("testing process() ... ");
  image_data *image_p = setup();

  /* image_p->rgb_to_greyscale(image_p); */
  /* image_p->process(image_p, Mean, 5); */
  image_p->process(image_p, Gaussian, 7);
  unsigned char **pixels = image_p->export_pixels(image_p);

  write_image("test_output_dir/test_convolution.png", pixels, image_p->height,
              image_p->width, image_p->channels, 8, 2);
  teardown(image_p);
  printf("passed!\n");
}

void
test_translation()
{
  printf("testing image_translation() ... ");
  image_data *image_p = setup();

  image_p->image_translation(image_p, 40, 40);
  image_p->image_translation(image_p, -80, -80);
  unsigned char **pixels = image_p->export_pixels(image_p);

  write_image("test_output_dir/test_translation.png", pixels, image_p->height,
              image_p->width, image_p->channels, 8, 2);
  teardown(image_p);
  printf("passed!\n");
}

void
test_scan_image()
{

  printf("testing create_document_outline() ... ");
  image_data *image_p = setup();

  image_p->create_document_outline(image_p);
  assert(image_p->document_p != NULL);
  unsigned char **pixels = image_p->export_pixels(image_p);

  write_image("test_output_dir/test_scan_image.png", pixels, image_p->height,
              image_p->width, image_p->channels, 8, 2);
  teardown(image_p);
  printf("passed!\n");
}

void
test_gernerate_dataset()
{
  printf("testing generate_dataset_from_image() ... ");
  image_data *image_p = setup();

  image_p->create_document_outline(image_p);
  image_p->generate_dataset_from_image(image_p, "test_output_dir/dataset.txt",
                                       "test_output_dir/sample.txt",
                                       "test_output_dir/info.json");
  teardown(image_p);
  printf("passed!\n");
}

int
main(int argc, char **argv)
{
  printf("Running tests...\n\n");

  test_load();
  test_export();
  test_save();
  test_switch_channel_num();
  test_invert_colours();
  test_pooling();
  test_convolution();
  test_translation();
  test_scale_larger();
  test_scale_smaller();
  test_scan_image();
  test_gernerate_dataset();

  printf("\nAll tests passed!\n");
  return 0;
}
