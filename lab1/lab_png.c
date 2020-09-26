#include "lab_png.h"
#include <arpa/inet.h>

//check if the buffer and the expected signature are the same
//return 0 if false, 1 if true
int is_png(U8 *buf, size_t n)
{
    const U8 expected[PNG_SIG_SIZE] = {137, 80, 78, 71, 13, 10, 26, 10};
    for (int i = 0; i < n; i++)
    {
        if (buf[i] != expected[i])
        {
            return 0;
        }
    }
    return 1;
}


int get_png_height(struct data_IHDR *buf)
{
    return buf->height;
}


int get_png_width(struct data_IHDR *buf)
{
    return buf->width;
}


int get_png_data_IHDR(struct data_IHDR *out, FILE *fp)
{

    fseek(fp, 16, SEEK_SET);
    fread(&out->width, sizeof(out->width), 1, fp);
    fread(&out->height, sizeof(out->height), 1, fp);

    out->width = ntohl(out->width);
    out->height = ntohl(out->height);

    fread(&out->bit_depth, sizeof(out->bit_depth), 1, fp);
    fread(&out->color_type, sizeof(out->color_type), 1, fp);
    fread(&out->compression, sizeof(out->compression), 1, fp);
    fread(&out->filter, sizeof(out->filter), 1, fp);
    fread(&out->interlace, sizeof(out->interlace), 1, fp);

    return 0;
}
