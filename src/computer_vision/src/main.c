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

#include "../includes/image_handling.h"
#include "../includes/image_processing.h"

int main(int argc, char ** argv) {

    int height, width, channels, color_type, bit_depth;

    unsigned char ** pixels = read_image(argv[1], &height, &width, &channels, &bit_depth, &color_type);

    // move data in and out of structures
    image_data * image = initialise_data(pixels, height, width, 3);
    image->colour_to_greyscale(image);
    image->expand_greyscale(image);
    pixels = image->export_pixels(image);
    /* inspect_image(pixels, height, width); */

    write_image(argv[2], pixels, height, width, image->channels, bit_depth, color_type);

    return 0;

}
