#include "../include/bmp_utils.h"

static uint64_t get_image_padding(uint64_t width) {
    return width % 4;
}

/* функции чтения */
static enum read_status read_header(FILE *file_in, struct bmp_header *header) {
    size_t count = 1;
    size_t result = fread(header, sizeof(struct bmp_header), count, file_in);
    if (result != count) {
        return READ_INVALID_HEADER;
    }
    return READ_OK;
}

static enum read_status read_pixels_bmp(FILE *file, struct image *img) {
    size_t count = 1;
    size_t size = (img->width) * sizeof(struct pixel);
    size_t padding = get_image_padding(img->width);
    for (size_t i = 0; i < img->height; i++) {
        size_t result_d = fread(img->data + i * (img->width), size, count, file);
        size_t result_p = fseek(file, padding, SEEK_CUR);
        if (result_d != count || result_p != 0) {
            return READ_FAIL;
        }
    }
    return READ_OK;
}

/* функции записи */
static struct bmp_header create_header_bmp(struct image const *img) {
    size_t padding = get_image_padding(img->width);
    struct bmp_header header = {0};
    header.bfType = 19778;
    header.bfReserved = 0;
    header.bOffBits = sizeof(struct bmp_header);
    header.biSize = 40;
    header.biSizeImage = img->height * (img->width * sizeof(struct pixel) + padding);
    header.bfileSize = header.bOffBits + header.biSizeImage;
    header.biWidth = img->width;
    header.biHeight = img->height;
    header.biPlanes = 1;
    header.biBitCount = 24;
    header.biCompression = 0;
    header.biXPelsPerMeter = 0;
    header.biYPelsPerMeter = 0;
    header.biClrUsed = 0;
    header.biClrImportant = 0;
    return header;
}

static enum write_status write_pixels_bmp(FILE *out, struct image const *img) {
    size_t count = 1;
    uint32_t padding_val = 0;
    uint64_t padding = get_image_padding(img->width);
    for (size_t i = 0; i < img->height; ++i) {
        size_t result_d = fwrite(img->data + i * img->width, img->width * sizeof(struct pixel), count, out);
        size_t result_p = fwrite(&padding_val, padding, count, out);
        if (result_d != count || result_p != count) {
            return WRITE_ERROR;
        }
    }
    return WRITE_OK;
}

enum read_status from_bmp(FILE *input_file, struct image *img) {
    if (!(input_file || img)) {
        return READ_NOTHING;
    }
    struct bmp_header header = {0};
    if (read_header(input_file, &header)) {
        return READ_FAIL;
    }
    fseek(input_file, header.bOffBits, SEEK_SET);
    if (make_image(img, header.biWidth, header.biHeight)) {
        return READ_FAIL;
    }
    if(read_pixels_bmp(input_file, img)) {
        return READ_FAIL;
    }
    return READ_OK;
}

enum write_status to_bmp(FILE *output_file, struct image const *img) {
    size_t count = 1;
    struct bmp_header header = create_header_bmp(img);
    if (fwrite(&header, sizeof(struct bmp_header), count, output_file) != 1) {
        return WRITE_ERROR;
    }
    fseek(output_file, header.bOffBits, SEEK_SET);
    if (write_pixels_bmp(output_file, img)) {
        return WRITE_ERROR;
    }
    return WRITE_OK;
}










