#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "crc.h"
#include "zutil.h"
#include "lab_png.h"

//return 0 if no error, else return 1
U32 get_crc_value(struct chunk *chunk_p, struct data_IHDR *data_ihdr)
{

    U64 len_source = CHUNK_TYPE_SIZE + get_chunk_length(chunk_p);
    U8 *source = malloc(len_source * sizeof(U8));
    memcpy(source, chunk_p->type, CHUNK_TYPE_SIZE * sizeof(U8));

    if (data_ihdr != NULL)
    {
        memcpy(source + CHUNK_TYPE_SIZE, data_ihdr, get_chunk_length(chunk_p) * sizeof(U8));
    }
    else
    {
        memcpy(source + CHUNK_TYPE_SIZE, chunk_p->p_data, get_chunk_length(chunk_p) * sizeof(U8));
    }

    U32 res = crc(source, len_source);
    if (source != NULL)
    {
        free(source);
        source = NULL;
    }
    return res;
}

void concat(struct chunk *res, struct simple_PNG **sub_pngs, struct data_IHDR **sub_data_ihdrs, int sub_png_len)
{
    U64 final_length = 0;
    U64 *lengthes = (U64 *)malloc(sizeof(U64) * sub_png_len);
    U8 **def_array = (U8 **)malloc(sizeof(U8 *) * sub_png_len);

    for (int i = 0; i < sub_png_len; i++)
    {
        U32 height = get_png_height(sub_data_ihdrs[i]);
        U32 width = get_png_width(sub_data_ihdrs[i]);
        def_array[i] = (U8 *)malloc(height * (width * 4 + 1));

        mem_inf(def_array[i], &lengthes[i], sub_pngs[i]->p_IDAT->p_data, get_chunk_length(sub_pngs[i]->p_IDAT));
        final_length += lengthes[i];
    }

    U8 *def_concat = (U8 *)malloc(sizeof(U8) * final_length);
    int prev_end = 0;
    for (int i = 0; i < sub_png_len; i++)
    {
        memcpy(def_concat + prev_end, def_array[i], lengthes[i] * sizeof(U8));
        prev_end += lengthes[i];
    }

    res->p_data = (U8 *)malloc(sizeof(U8) * final_length);
    U64 result_length = 0;
    mem_def(res->p_data, &result_length, def_concat, final_length, Z_DEFAULT_COMPRESSION);
    set_chunk_length(res, (U32)result_length);

    if (lengthes != NULL)
    {
        free(lengthes);
        lengthes = NULL;
    }
    for (int i = 0; i < sub_png_len; i++)
    {
        if (def_array[i] != NULL)
        {
            free(def_array[i]);
            def_array[i] = NULL;
        }
    }
    if (def_array != NULL)
    {
        free(def_array);
        def_array = NULL;
    }

    if (def_concat != NULL)
    {
        free(def_concat);
        def_concat = NULL;
    }
}

int main(int argc, char **argv)
{

    char *filename = "all.png";
    FILE *file = fopen(filename, "wb+");
    U8 *signature_buffer = PNG_SIG;
    struct data_IHDR *data_ihdr = (struct data_IHDR *)malloc(sizeof(struct data_IHDR));
    init_png_data_IHDR(data_ihdr);
    struct simple_PNG *png_data = (struct simple_PNG *)malloc(sizeof(struct simple_PNG));
    init_png(png_data);

    int sub_png_len = argc - 1;
    struct simple_PNG **sub_pngs = malloc(sub_png_len * sizeof(struct simple_PNG *));
    struct data_IHDR **sub_data_ihdrs = malloc(sub_png_len * sizeof(struct data_IHDR *));

    for (int i = 0; i < sub_png_len; i++)
    {
        char *sub_filename = argv[i + 1];
        FILE *sub_file = fopen(sub_filename, "rb");

        sub_data_ihdrs[i] = (struct data_IHDR *)malloc(sizeof(struct data_IHDR));
        read_png_data_IHDR(sub_data_ihdrs[i], sub_file);

        if (get_png_width(data_ihdr) == 0)
        {
            set_png_width(data_ihdr, get_png_width(sub_data_ihdrs[i]));
        }
        set_png_height(data_ihdr, get_png_height(data_ihdr) + get_png_height(sub_data_ihdrs[i]));

        sub_pngs[i] = (struct simple_PNG *)malloc(sizeof(struct simple_PNG));
        read_png(sub_pngs[i], sub_file);

        set_chunk_length(png_data->p_IDAT, get_chunk_length(png_data->p_IDAT) + get_chunk_length(sub_pngs[i]->p_IDAT));

        fclose(sub_file);
    }

    fwrite(signature_buffer, PNG_SIG_SIZE, 1, file);

    fwrite(&png_data->p_IHDR->length, CHUNK_LEN_SIZE, 1, file);
    fwrite(png_data->p_IHDR->type, CHUNK_TYPE_SIZE, 1, file);
    fwrite(data_ihdr, DATA_IHDR_SIZE, 1, file);
    set_crc(png_data->p_IHDR, get_crc_value(png_data->p_IHDR, data_ihdr));
    fwrite(&png_data->p_IHDR->crc, CHUNK_CRC_SIZE, 1, file);

    concat(png_data->p_IDAT, sub_pngs, sub_data_ihdrs, sub_png_len);
    fwrite(&png_data->p_IDAT->length, CHUNK_LEN_SIZE, 1, file);
    fwrite(png_data->p_IDAT->type, CHUNK_TYPE_SIZE, 1, file);
    fwrite(png_data->p_IDAT->p_data, get_chunk_length(png_data->p_IDAT), 1, file);
    set_crc(png_data->p_IDAT, get_crc_value(png_data->p_IDAT, NULL));
    fwrite(&png_data->p_IDAT->crc, CHUNK_CRC_SIZE, 1, file);

    fwrite(&png_data->p_IEND->length, CHUNK_LEN_SIZE, 1, file);
    fwrite(png_data->p_IEND->type, CHUNK_TYPE_SIZE, 1, file);
    set_crc(png_data->p_IEND, get_crc_value(png_data->p_IEND, NULL));
    fwrite(&png_data->p_IEND->crc, CHUNK_CRC_SIZE, 1, file);

    fclose(file);

    for (int i = 0; i < sub_png_len; i++)
    {
        if (sub_data_ihdrs[i] != NULL)
        {
            free(sub_data_ihdrs[i]);
            sub_data_ihdrs[i] = NULL;
        }

        if (sub_pngs[i]->p_IHDR->p_data != NULL)
        {
            free(sub_pngs[i]->p_IHDR->p_data);
            sub_pngs[i]->p_IHDR->p_data = NULL;
        }
        if (sub_pngs[i]->p_IDAT->p_data != NULL)
        {
            free(sub_pngs[i]->p_IDAT->p_data);
            sub_pngs[i]->p_IDAT->p_data = NULL;
        }
        if (sub_pngs[i]->p_IEND->p_data != NULL)
        {
            free(sub_pngs[i]->p_IEND->p_data);
            sub_pngs[i]->p_IEND->p_data = NULL;
        }
        if (sub_pngs[i]->p_IHDR != NULL)
        {
            free(sub_pngs[i]->p_IHDR);
            sub_pngs[i]->p_IHDR = NULL;
        }
        if (sub_pngs[i]->p_IDAT != NULL)
        {
            free(sub_pngs[i]->p_IDAT);
            sub_pngs[i]->p_IDAT = NULL;
        }
        if (sub_pngs[i]->p_IEND != NULL)
        {
            free(sub_pngs[i]->p_IEND);
            sub_pngs[i]->p_IEND = NULL;
        }
        if (sub_pngs[i] != NULL)
        {
            free(sub_pngs[i]);
            sub_pngs[i] = NULL;
        }
    }

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
    if (sub_pngs != NULL)
    {
        free(sub_pngs);
        sub_pngs = NULL;
    }
    if (sub_data_ihdrs != NULL)
    {
        free(sub_data_ihdrs);
        sub_data_ihdrs = NULL;
    }

    return 0;
}
