#include "lab_png.h"

//return 1 if true, else return 0
int is_png(U8 *buf, size_t n) {


}
int get_png_height(struct data_IHDR *buf) {
    return buf->height;
}
int get_png_width(struct data_IHDR *buf) {
    return buf->width;
}
int get_png_data_IHDR(struct data_IHDR *out, FILE *fp, long offset, int whence) {
    // fseek(fp,8, SEEK_SET);
    // char buff[DATA_IHDR_SIZE];
    //     fread(out, len, 1, fp);
    // for(int i = 0; i<len; i++)
    //     printf("%u ", buf[i]);


}
