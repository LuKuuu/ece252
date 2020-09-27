#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "crc.h"
#include "zutil.h"
#include "lab_png.h"

//return 0 if no error, else return 1
int check_crc(struct chunk *chunk_p)
{

    U64 len_source = CHUNK_TYPE_SIZE + get_chunk_length(chunk_p);
    U8 *source = malloc(len_source * sizeof(U8));
    memcpy(source, chunk_p->type, CHUNK_TYPE_SIZE * sizeof(U8));
    memcpy(source + CHUNK_TYPE_SIZE, chunk_p->p_data, get_chunk_length(chunk_p) * sizeof(U8));
    U32 crc_val = crc(source, len_source);

    if (source != NULL)
    {
        free(source);
        source = NULL;
    }
    if (get_crc(chunk_p) != crc_val)
    {
        printf("%.4s chunk CRC error: computed %x, expected %x\n", chunk_p->type, crc_val, get_crc(chunk_p));
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    char *filename;
    FILE *file;
    U8 *signature_buffer;
    struct data_IHDR *data_ihdr;
    struct simple_PNG *png_data;

    filename = argv[1];
    file = fopen(filename, "rb");

    signature_buffer = malloc(PNG_SIG_SIZE);

    fread(signature_buffer, PNG_SIG_SIZE, 1, file);
    if (is_png(signature_buffer, PNG_SIG_SIZE))
    {
        data_ihdr = (struct data_IHDR *)malloc(sizeof(struct data_IHDR));
        read_png_data_IHDR(data_ihdr, file);
        printf("%s: %u x %u\n", filename, get_png_width(data_ihdr), get_png_height(data_ihdr));

        png_data = (struct simple_PNG *)malloc(sizeof(struct simple_PNG));
        read_png(png_data, file);

        int no_error = 0;
        if (no_error == 0)
            no_error = check_crc(png_data->p_IHDR);
        if (no_error == 0)
            no_error = check_crc(png_data->p_IDAT);
        if (no_error == 0)
            no_error = check_crc(png_data->p_IEND);

        if (data_ihdr != NULL)
        {
            free(data_ihdr);
            data_ihdr = NULL;
        }

        if (png_data->p_IHDR->p_data != NULL)
        {
            free(png_data->p_IHDR->p_data);
            png_data->p_IHDR->p_data = NULL;
        }
        if (png_data->p_IDAT->p_data != NULL)
        {
            free(png_data->p_IDAT->p_data);
            png_data->p_IDAT->p_data = NULL;
        }
        if (png_data->p_IEND->p_data != NULL)
        {
            free(png_data->p_IEND->p_data);
            png_data->p_IEND->p_data = NULL;
        }
        if (png_data->p_IHDR != NULL)
        {
            free(png_data->p_IHDR);
            png_data->p_IHDR = NULL;
        }
        if (png_data->p_IDAT != NULL)
        {
            free(png_data->p_IDAT);
            png_data->p_IDAT = NULL;
        }
        if (png_data->p_IEND != NULL)
        {
            free(png_data->p_IEND);
            png_data->p_IEND = NULL;
        }
        if (png_data != NULL)
        {
            free(png_data);
            png_data = NULL;
        }
    }
    else
    {
        printf("%s: Not a PNG file\n", filename);
    }

    if(signature_buffer != NULL) {
        free(signature_buffer);
        signature_buffer = NULL;
    }
    fclose(file);

    return 0;
}
