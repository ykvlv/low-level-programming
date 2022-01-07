#include <stdlib.h>
#include "../include/image.h"

struct image rotate(struct image *img) {
    struct image output_img;
    uint64_t width = img->width, height = img->height;
    output_img.width = height;
    output_img.height = width;
    struct pixel *data_pixels = malloc(sizeof(struct pixel) * height * width);
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            data_pixels[(height) * (j + 1) - i - 1] = img->data[(i * width) + j];
        }
    }
    output_img.data = data_pixels;
    return output_img;
}
