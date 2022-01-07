#include <stdlib.h>
#include "../include/image.h"


bool make_image(struct image* image, uint64_t width, uint64_t height) {
    image->height = height;
    image->width = width;
    image->data = malloc(sizeof(struct pixel) * width * height);
    return image->data == NULL;
}

void destroy_image(struct image* image) {
    free(image->data);
}
