#include "lab_png.h"
#include <arpa/inet.h>
#include <stdlib.h>

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

void read_png_data_IHDR(struct data_IHDR *out, FILE *fp)
{

    fseek(fp, 16, SEEK_SET);
    fread(&out->width, sizeof(out->width), 1, fp);
    fread(&out->height, sizeof(out->height), 1, fp);
    fread(&out->bit_depth, sizeof(out->bit_depth), 1, fp);
    fread(&out->color_type, sizeof(out->color_type), 1, fp);
    fread(&out->compression, sizeof(out->compression), 1, fp);
    fread(&out->filter, sizeof(out->filter), 1, fp);
    fread(&out->interlace, sizeof(out->interlace), 1, fp);
}

int get_png_height(struct data_IHDR *buf)
{
    return ntohl(buf->height);
}

int get_png_width(struct data_IHDR *buf)
{
    return ntohl(buf->width);
}

void read_chunk(struct chunk *out, FILE *fp)
{
    fread(&out->length, CHUNK_LEN_SIZE, 1, fp);
    fread(&out->type, sizeof(U8), sizeof(out->type), fp);
    out->p_data = (U8 *)malloc(get_chunk_length(out) * sizeof(U8));
    for (int i = 0; i < get_chunk_length(out); i++)
    {
        fread(&out->p_data[i], sizeof(U8), 1, fp);
    }
    fread(&out->crc, CHUNK_CRC_SIZE, 1, fp);
}

U32 get_chunk_length(struct chunk *chunk_p)
{
    return ntohl(chunk_p->length);
}

U32 get_crc(struct chunk *chunk_p)
{
    return ntohl(chunk_p->crc);
}

void read_png(struct simple_PNG *out, FILE *fp)
{
    out->p_IHDR = (struct chunk *)malloc(sizeof(struct chunk));
    out->p_IDAT = (struct chunk *)malloc(sizeof(struct chunk));
    out->p_IEND = (struct chunk *)malloc(sizeof(struct chunk));

    fseek(fp, 8, SEEK_SET);
    read_chunk(out->p_IHDR, fp);
    read_chunk(out->p_IDAT, fp);
    read_chunk(out->p_IEND, fp);
}
