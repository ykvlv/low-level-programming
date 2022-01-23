#include "../include/bmp_utils.h"
#include "../include/transform.h"

_Noreturn void error(const char* msg) {
    fprintf(stderr, "%s", msg);
    exit(1);
}

bool rotate_bmp(FILE *input_file, FILE *output_file) {
    struct image original_image;
    struct image transformed_image;

    if (from_bmp(input_file, &original_image)) {
        fclose(input_file); fclose(output_file);
        destroy_image(&original_image);
        error("Не удалось конвертировать файл из BMP.\n");
    }

    fprintf(stdout, "Конвертация из BMP прошла успешно.\n");

    transformed_image = rotate(&original_image);

    if(to_bmp(output_file,&transformed_image)) {
        fclose(input_file); fclose(output_file);
        destroy_image(&original_image); destroy_image(&transformed_image);
        error("Не удалось конвертировать выходной файл в BMP.\n");
    }

    fprintf(stdout, "Перевернутая картинка успешно сохранена.\n");

    destroy_image(&original_image);
    destroy_image(&transformed_image);
    return true;
}
