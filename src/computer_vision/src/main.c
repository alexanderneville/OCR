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

int main(int argc, char ** argv) {

    int height, width, channels, color_type, bit_depth;

    unsigned char ** pixels = read_image(argv[1], &height, &width, &channels, &bit_depth, &color_type);

    printf("height: %d, width: %d\n", height, width);
    printf("channels: %d\n", channels);

    image_data * image_p = initialise_data(pixels, height, width, 3);

    pixels = (* image_p).export_pixels(image_p);

    write_image(argv[2], pixels, image_p->height, image_p->width, image_p->channels, bit_depth, color_type);

    return 0;

}
