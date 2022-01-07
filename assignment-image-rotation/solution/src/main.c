#include "../include/utils.h"


int main(int argc, char** argv) {
    if (argc != 3) {
        error("В аргументах должны быть input.bmp и output.bmp\n");
    }

    FILE *input_file = NULL, *output_file = NULL;
    input_file = fopen(argv[1], "rb");
    output_file = fopen(argv[2], "wb");

    if (input_file == NULL) {
        error("Ошибка открытии файла\n");
    } else if (output_file == NULL) {
        error("Ошибка создании файла\n");
    }

    rotate_bmp(input_file, output_file);

    fclose(input_file);
    fclose(output_file);
    return 0;
}
