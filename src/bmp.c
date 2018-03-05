#include "bmp.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void write_bmp_header(FILE* fp, int width, int height, int real_width)
{
    unsigned int file_size = height * real_width + BMP_HEADER_SIZE;
    unsigned int offset_to_data = BMP_HEADER_SIZE;
    unsigned long info_header_size = BMP_INFO_HEADER_SIZE;
    unsigned int planes = 1;
    unsigned int color = 24;
    unsigned long compress = 0;
    unsigned long data_size = height * real_width;
    long xppm = 1;
    long yppm = 1;

    unsigned char header_buf[BMP_HEADER_SIZE];
    header_buf[0] = 'B';
    header_buf[1] = 'M';
    memcpy(header_buf + 2, &file_size, sizeof(file_size));
    header_buf[6] = 0;
    header_buf[7] = 0;
    header_buf[8] = 0;
    header_buf[9] = 0;
    memcpy(header_buf + 10, &offset_to_data, sizeof(offset_to_data));
    header_buf[11] = 0;
    header_buf[12] = 0;
    header_buf[13] = 0;

    memcpy(header_buf + 14, &info_header_size, sizeof(info_header_size));
    header_buf[15] = 0;
    header_buf[16] = 0;
    header_buf[17] = 0;
    memcpy(header_buf + 18, &width, sizeof(width));
    memcpy(header_buf + 22, &height, sizeof(height));
    memcpy(header_buf + 26, &planes, sizeof(planes));
    memcpy(header_buf + 28, &color, sizeof(color));
    memcpy(header_buf + 30, &compress, sizeof(compress));
    memcpy(header_buf + 34, &data_size, sizeof(data_size));
    memcpy(header_buf + 38, &xppm, sizeof(xppm));
    memcpy(header_buf + 42, &yppm, sizeof(yppm));
    header_buf[46] = 0;
    header_buf[47] = 0;
    header_buf[48] = 0;
    header_buf[49] = 0;
    header_buf[50] = 0;
    header_buf[51] = 0;
    header_buf[52] = 0;
    header_buf[53] = 0;

    fwrite(header_buf, sizeof(unsigned char), BMP_HEADER_SIZE, fp);

}

int write_bmp(char *filename, int width, int height, struct Rgb rgb_data[], int sprite_num)
{
    FILE* fp = fopen(filename, "wb");
    if(!fp){
        perror(filename);
        return 1;
    }

    // 1 pixcel needs 3 bytes(RGB).
    // width should be multiple of 4.
    int real_width = width * 3 + width % 4;

    write_bmp_header(fp, width, height, real_width);

    unsigned char* bmp_line_data = (unsigned char *)malloc(sizeof(unsigned char)*real_width);
    if (!bmp_line_data)
    {
        perror("malloc");
        fclose(fp);
        return 1;
    }

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            // RGB data
            bmp_line_data[j*3]     = rgb_data[(height - i - 1)*width + j].b; // B
            bmp_line_data[j*3 + 1] = rgb_data[(height - i - 1)*width + j].g; // G
            bmp_line_data[j*3 + 2] = rgb_data[(height - i - 1)*width + j].r; // R
        }
        // Align 4byte
        for (int j = width * 3; j < real_width; ++j)
        {
            bmp_line_data[j] = 0;
        }
        fwrite(bmp_line_data, sizeof(unsigned char), real_width, fp);
    }

    free(bmp_line_data);

    fclose(fp);

    return 0;
}

