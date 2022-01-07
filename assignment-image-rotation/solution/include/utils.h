#include <stdbool.h>
#include <stdio.h>

_Noreturn void error(const char* msg);

bool rotate_bmp(FILE *input_file, FILE *output_file);
