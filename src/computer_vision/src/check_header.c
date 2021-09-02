#include <stdlib.h>
#include <png.h>
#include "../includes/image_io.h"

int check_header(char * file_name) {

    FILE *fp = fopen(file_name, "rb");

    if (!fp) {
        return 0;
    }

    unsigned char header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, SIGNATURE)) return -1;

    fclose(fp);
    return 0;

}

int main(int argc, char * argv[]) {

    int rc = check_header(argv[1]);

    if (rc < 0) { printf("not a png file\n"); } else { printf("valid png file\n"); }

    return 0;

}
