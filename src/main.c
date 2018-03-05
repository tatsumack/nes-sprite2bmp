#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bmp.h"

#define NES_HEADER_SIZE 0x0010
#define PROGRAM_ROM_SIZE 0x4000
#define CHARACTER_ROM_SIZE 0x2000
#define BYTE_PER_SPRITE 16
#define SPRITE_ROW_SIZE 8
#define SPRITE_COL_SIZE 8

#define IMG_WIDTH 400

// SEE: http://wiki.nesdev.com/w/index.php/INES
struct Header {
    char cval[4];
    char prgpages;
    char chrpages;
    char flags6;
    char prgpages8;
    char flags9;
    char flags10;
    char zero[4];
};

void convert_nes_to_bmp(struct Rgb rgb_data[], int nes_data[], int width, int height)
{
    int sprites_num_per_row = width / SPRITE_ROW_SIZE;
    int index = 0;
    for (int i = 0; i < width * height; ++i)
    {
        // nes_data[] index align
        //  0  1  2   9  10 11
        //  3  4  5   12 13 14
        //  6  7  8   15 16 17
        //
        // rgb_data[] index align
        //  0  1  2   3  4  5
        //  6  7  8   9  10 11
        //  12 13 14  15 16 17

        rgb_data[index].r = nes_data[i] * 85;
        rgb_data[index].g = nes_data[i] * 85;
        rgb_data[index].b = nes_data[i] * 85;

        if ((i + 1) % (sprites_num_per_row * SPRITE_COL_SIZE * SPRITE_ROW_SIZE) == 0)
        {
            // to new line sprite
            ++index;
        }
        else if ((i + 1) % (SPRITE_COL_SIZE * SPRITE_ROW_SIZE) == 0)
        {
            // to right sprite
            index = index - width * (SPRITE_ROW_SIZE - 1)  + 1;
        }
        else if ((i + 1) % SPRITE_ROW_SIZE == 0)
        {
            // new line in sprite
            index += width - (SPRITE_ROW_SIZE - 1);
        }
        else
        {
            ++index;
        }
    }

}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s /path/to/input.nes /path/to/output.bmp\n", argv[0]);
        exit(1);
    }

    FILE* fp = fopen(argv[1], "rb");
    if (!fp)
    {
        perror(argv[0]);
        exit(1);
    }

    struct Header header;
    int n;
    if (!(n = fread(&header, sizeof(header), 1, fp)))
    {
        perror(argv[0]);
        exit(1);
    }

    int cstart = NES_HEADER_SIZE + header.prgpages * PROGRAM_ROM_SIZE;
    if (fseek(fp, cstart, SEEK_SET) == -1)
    {
        perror(argv[0]);
        exit(1);
    }
    char* buf = malloc(header.chrpages * CHARACTER_ROM_SIZE);
    if (!buf)
    {
        perror(argv[0]);
        exit(1);
    }

    if (!(n = fread(buf, sizeof(char), header.chrpages * CHARACTER_ROM_SIZE, fp)))
    {
        perror(argv[0]);
        exit(1);
    }

    int sprite_num = header.chrpages * CHARACTER_ROM_SIZE / BYTE_PER_SPRITE;

    int sprites[sprite_num * SPRITE_COL_SIZE * SPRITE_ROW_SIZE];
    for (int sprite_index  = 0; sprite_index < sprite_num; ++sprite_index)
    {
        for (int i = 0; i < BYTE_PER_SPRITE; ++i)
        {
            for (int j = 0; j < SPRITE_COL_SIZE; ++j)
            {
                int index = sprite_index * SPRITE_COL_SIZE * SPRITE_ROW_SIZE + (i % SPRITE_ROW_SIZE) * SPRITE_ROW_SIZE + j;
                if (i / SPRITE_COL_SIZE == 0)
                {
                   sprites[index] = 0;
                }
               if ((buf[sprite_index * BYTE_PER_SPRITE + i] & (0x80 >> j)))
               {
                   sprites[index] += 0x01 << (i / 8);
               }
            }
        }
    }

    int height = (sprite_num / (IMG_WIDTH / SPRITE_ROW_SIZE)) * SPRITE_COL_SIZE;

    struct Rgb rgb_data[IMG_WIDTH * height];
    convert_nes_to_bmp(rgb_data, sprites, IMG_WIDTH, height);
    write_bmp(argv[2], IMG_WIDTH, height, rgb_data, sprite_num);

    fclose(fp);

    return 0;
}

