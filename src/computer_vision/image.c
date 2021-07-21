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

#include "./includes/image_structures.h"

#define SIGNATURE 8
#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stderr, "ERROR  (" __FILE__ ":%d) -- %s\n", __LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

png_byte color_type;
png_byte bit_depth;
int channels;

unsigned char ** read_image(char * file_name, int * height, int * width) {

    FILE *fp = fopen(file_name, "rb");

    if (!fp) {
        return NULL;
    }

    char header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, SIGNATURE)){
        printf("not a png file\n");
        return NULL;
    } 

    png_struct * png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); //use default error handling
    if (!png_ptr){
        exit(-1);
        fclose(fp);
    }

    // note that png_infopp & png_infop structs are a type alias for pointers to struct png_info.
    png_info * info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr){
        png_destroy_read_struct(
           &png_ptr,
           (png_infopp)NULL, 
           (png_infopp)NULL);
        return NULL;
    }

    png_info * end_info = png_create_info_struct(png_ptr);
    if (!end_info){
        png_destroy_read_struct(
            &png_ptr, 
            &info_ptr,
            (png_infopp)NULL);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {

        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return NULL;

    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, SIGNATURE); // first 8 bytes already read
    png_read_info(png_ptr, info_ptr);

    // queue transformations
    if (color_type == PNG_COLOR_TYPE_PALETTE &&
        bit_depth <= 8) png_set_expand(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY &&
        bit_depth < 8) png_set_expand(png_ptr);

    if (png_get_valid(png_ptr, info_ptr,
        PNG_INFO_tRNS)) png_set_expand(png_ptr);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    // strip the alpha channel
    if (color_type & PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    * height = png_get_image_height(png_ptr, info_ptr);
    * width = png_get_image_width(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    channels = png_get_channels(png_ptr, info_ptr);

    /* printf("Colour Type: %d\n", color_type); */
    /* printf("Desired type: %d\n",PNG_COLOR_TYPE_RGB); */
    /* printf("Bit Depth: %d\n", bit_depth); */
    /* printf("Channels: %d\n", channels); */
    /* printf("(width, height) = (%d, %d)\n", width, height); */
    /* printf("\n\n"); */

    // pixels is a pointer to an array contating pointers to arrays of png_bytes
    unsigned char ** pixels = (unsigned char **) malloc(sizeof(unsigned char *) * (* height));

    // initialise malloced memory
    for (int y = 0; y < (* height); y++) {
        // each png_bytep in the pixels array is initialised to the size of one row
        pixels[y] = (unsigned char *) malloc(png_get_rowbytes(png_ptr, info_ptr));
    }

    png_read_image(png_ptr, pixels);

    // cleanup
    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);

    return pixels;

}

void inspect_image(unsigned char ** pixels, int height, int width) {

    for (int y = 0; y < height; y++) {
        // retrive array pointer for every row 
        unsigned char * row = pixels[y];
        for (int x = 0; x < width*3; x+=3) {
            // one pixel takes three indexes under RGB
            int R = row[x];
            int G = row[x+1];
            int B = row[x+2];
            printf("pixel at position ( %d , %d ) has RGB values: %d - %d - %d\n", x/3, y, R, G, B);
        }
    }
}

void write_image(char * file_name, unsigned char ** pixels, int height, int width) {

    FILE *fp = fopen(file_name, "wb");

    if (!fp) {
        exit(-1);
    }

    png_struct * png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_info * info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, pixels);
    png_write_end(png_ptr, NULL);

    //cleanup
    for (int y = 0; y < height; y++)
            free(pixels[y]);
    free(pixels);

    fclose(fp);

}

int main(int argc, char ** argv) {

    int height, width;
    unsigned char ** pixels = read_image(argv[1], &height, &width);
    image_data * image = initialise_data(pixels, height, width, 3);
    inspect_image(pixels, height, width);
    pixels = image->export_pixels(image);
    inspect_image(pixels, height, width);
    write_image(argv[2], pixels, height, width);
    return 0;

}
