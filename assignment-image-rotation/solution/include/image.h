#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

struct pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

#pragma pack(push, 1)
struct image {
    uint64_t width;
    uint64_t height;
    struct pixel* data;
};
#pragma pack(pop)

bool make_image(struct image* image, uint64_t width, uint64_t height);
void destroy_image(struct image* image);
