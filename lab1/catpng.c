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
    return crc(source, len_source);
}

void concat(struct chunk *res, struct simple_PNG **sub_pngs, struct data_IHDR **sub_data_ihdrs, int sub_png_len)
{
    U64 final_length = 0;
    U64 *lengthes = (U64 *)malloc(sizeof(U64) * sub_png_len);
    U8 **def_array = (U8 **)malloc(sizeof(U8 *) * sub_png_len);

    for (int i = 0; i < sub_png_len; i++)
    {
        printf("here\n");
        printf("%x\n", get_chunk_length(sub_pngs[i]->p_IDAT));
        U32 height = get_png_height(sub_data_ihdrs[i]);
        U32 width = get_png_width(sub_data_ihdrs[i]);
        def_array[i] = (U8 *)malloc(height * (width * 4 + 1));

        mem_inf(def_array[i], &lengthes[i], sub_pngs[i]->p_IDAT->p_data, get_chunk_length(sub_pngs[i]->p_IDAT));
        final_length += lengthes[i];
        printf("%X\n", final_length);
    }

    U8 *def_concat = (U8 *)malloc(sizeof(U8) * final_length);
    int prev_end = 0;
    for (int i = 0; i < sub_png_len; i++)
    {
        printf("%d\n", i);
        memcpy(def_concat + prev_end, def_array[i], lengthes[i] * sizeof(U8));
        prev_end += lengthes[i];
    }

    printf("fl %d\n", final_length);
    printf("pe %d\n", prev_end);

    res->p_data = (U8 *)malloc(sizeof(U8) * final_length);
    U32 result_length = 0;
    int run_res = mem_def(res->p_data, &result_length, def_concat, final_length, Z_DEFAULT_COMPRESSION);
    set_chunk_length(res, result_length);
    if (run_res != 0)
    {
        printf("error\n");
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
        printf("argv[%d]=%s\n", i + 1, argv[i + 1]);
        char *sub_filename = argv[i + 1];
        FILE *sub_file = fopen(sub_filename, "rb");

        sub_data_ihdrs[i] = (struct data_IHDR *)malloc(sizeof(struct data_IHDR));
        read_png_data_IHDR(sub_data_ihdrs[i], sub_file);

        if (get_png_width(data_ihdr) == 0)
        {
            set_png_width(data_ihdr, get_png_width(sub_data_ihdrs[i]));
        }
        set_png_height(data_ihdr, get_png_height(data_ihdr) + get_png_height(sub_data_ihdrs[i]));
        printf("curr: %s: %u x %u\n", filename, get_png_width(data_ihdr), get_png_height(data_ihdr));

        sub_pngs[i] = (struct simple_PNG *)malloc(sizeof(struct simple_PNG));
        read_png(sub_pngs[i], sub_file);
        printf("%s: %d x %d\n", sub_filename, get_png_width(sub_data_ihdrs[i]), get_png_height(sub_data_ihdrs[i]));

        set_chunk_length(png_data->p_IDAT, get_chunk_length(png_data->p_IDAT) + get_chunk_length(sub_pngs[i]->p_IDAT));

        printf("%x\n", get_chunk_length(sub_pngs[i]->p_IDAT));

        fclose(sub_file);
    }

    printf("out loop\n");

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
    return 0;
}
