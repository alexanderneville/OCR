#ifndef handling_H
#define handling_H

#define SIGNATURE 8
#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stderr, "ERROR  (" __FILE__ ":%d) -- %s\n", __LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

unsigned char ** read_image(char * file_name, int * height, int * width, int * channels, int * bit_depth, int * color_type);
void inspect_image(unsigned char ** pixels, int height, int width);
void write_image(char * file_name, unsigned char ** pixels, int height, int width, int channels, int bit_depth, int color_type);

#endif // !handling_H

