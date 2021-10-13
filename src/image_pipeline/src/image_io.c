#include <stdlib.h>
#include <png.h>
#include <unistd.h>

#include "../includes/image_io.h"

int
check_header(char *file_name)
{
  /* Given the path to a file, verify that it is a valid PNG file */
  FILE *fp = fopen(file_name, "rb");

  if (!fp)
    return -1;
  unsigned char header[8];

  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, SIGNATURE))
    return -1;
  fclose(fp);
  return 0;
}

unsigned char **
read_image(char *file_name, int *height, int *width, int *channels,
           int *bit_depth, int *color_type)
{
  // open a file pointer for buffered io
  FILE *fp = fopen(file_name, "rb");

  if (!fp) {
    return NULL;
  }
  unsigned char header[8];

  // check the file header, to verify that this is a PNG file
  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, SIGNATURE)) {
    printf("not a png file\n");
    return NULL;
  }
  // initialise libpng
  png_struct *png_ptr =
    png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  // catch possible exception from libpng
  if (!png_ptr) {
    fclose(fp);
    return NULL;
  }
  png_info *info_ptr = png_create_info_struct(png_ptr);

  // catch possible exception from libpng
  if (!info_ptr) {
    // if the info struct is not created, deallocate the read struct
    png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
    return NULL;
  }
  png_info *end_info = png_create_info_struct(png_ptr);

  if (!end_info) {
    // if the info struct is not created, deallocate the read struct
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
    return NULL;
  }
  // from the libpng documentation; use defalut error handling
  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return NULL;
  }
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, SIGNATURE);
  png_read_info(png_ptr, info_ptr);
  // image format normalisation handled by libpng
  if (*color_type == PNG_COLOR_TYPE_PALETTE && *bit_depth <= 8)
    png_set_expand(png_ptr);
  if (*color_type == PNG_COLOR_TYPE_GRAY && *bit_depth < 8)
    png_set_expand(png_ptr);
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_expand(png_ptr);
  if (*bit_depth == 16)
    png_set_strip_16(png_ptr);
  if (*color_type & PNG_COLOR_MASK_ALPHA)
    png_set_strip_alpha(png_ptr);

  // update the libpng instance with any operations queued above
  png_read_update_info(png_ptr, info_ptr);
  // update args
  *height = png_get_image_height(png_ptr, info_ptr);
  *width = png_get_image_width(png_ptr, info_ptr);
  *color_type = png_get_color_type(png_ptr, info_ptr);
  *bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  *channels = png_get_channels(png_ptr, info_ptr);
  // read pixel data into 2d array
  unsigned char **pixels =
    (unsigned char **) malloc(sizeof(unsigned char *) * (*height));
  for (int y = 0; y < (*height); y++) {
    pixels[y] = (unsigned char *) malloc(png_get_rowbytes(png_ptr, info_ptr));
  }
  //cleanup
  png_read_image(png_ptr, pixels);
  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
  return pixels;
}

void
inspect_image(unsigned char **pixels, int height, int width)
{
  for (int y = 0; y < height; y++) {
    unsigned char *row = pixels[y];

    for (int x = 0; x < width * 3; x += 3) {
      int R = row[x];
      int G = row[x + 1];
      int B = row[x + 2];

      printf("pixel at position ( %d , %d ) has RGB values: %d - %d - %d\n",
             x / 3, y, R, G, B);
    }
  }
}

void
write_image(char *file_name, unsigned char **pixels, int height, int width,
            int channels, int bit_depth, int color_type)
{
  // open the output file
  FILE *fp = fopen(file_name, "wb");

  if (!fp) {
    return;
  }
  // opposite to previous function; open write struct from libpng
  png_struct *png_ptr =
    png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_info *info_ptr = png_create_info_struct(png_ptr);

  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, info_ptr, width, height,
               bit_depth, color_type, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr, info_ptr);
  // write output data to file
  png_write_image(png_ptr, pixels);
  // cleanup
  png_write_end(png_ptr, NULL);
  fclose(fp);
}
