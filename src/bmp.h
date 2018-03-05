#ifndef BMP_H
#define BMP_H

#define BMP_FILE_HEADER_SIZE 14
#define BMP_INFO_HEADER_SIZE 40
#define BMP_HEADER_SIZE (BMP_FILE_HEADER_SIZE+BMP_INFO_HEADER_SIZE)

struct Rgb
{
    char r;
    char g;
    char b;
};

int write_bmp(char *filename, int width, int height, struct Rgb rgb_data[], int sprite_num);

#endif
