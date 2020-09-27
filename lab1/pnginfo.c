#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "crc.h"
#include "zutil.h"
#include "lab_png.h"

//return 0 if no error, else return 1
int check_crc(struct chunk *chunk_p)
{

    U64 len_def = 0;
    U64 len_source = 4 + get_chunk_length(chunk_p);
    printf("length: %d\n",get_chunk_length(chunk_p));
    U8 *gp_buf_def = (U8 *)malloc(len_source * 2 * sizeof(U8));
    // U8 *source = malloc(len_source * sizeof(U8));
    // for (int i = 0; i < 4; i++) {
    //     source[i] = chunk_p->type[i];
    // }
    // for (int i = 4; i < len_source; i++) {
    //     source[i] = chunk_p->p_data[i - 4];
    // }
    // // memcpy(source, chunk_p->type, 4 * sizeof(U8));
    // // memcpy(source + 4, chunk_p->p_data, get_chunk_length(chunk_p) * sizeof(U8));

    mem_def(gp_buf_def, &len_def, chunk_p->p_data, get_chunk_length(chunk_p), Z_DEFAULT_COMPRESSION);
    printf("%c\n", chunk_p->type[3]);
    U32 crc_val = crc(gp_buf_def, len_def);
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
        printf("%s: %d x %d\n", filename, get_png_width(data_ihdr),get_png_height(data_ihdr));
    }
    else
    {
        printf("%s: Not a PNG file\n", filename);
        return 0;
    }

    png_data = (struct simple_PNG *)malloc(sizeof(struct simple_PNG));
    read_png(png_data, file);

    check_crc(png_data->p_IHDR);
    check_crc(png_data->p_IDAT);
    check_crc(png_data->p_IEND);

    return 0;
}
