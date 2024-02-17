#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "headers/environment.h"
#include "parser.c"
#include "utils/colors.h"
#include "utils/logging.h"

int file_size(FILE *file) {
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

char *file_contents(char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        char *message = (char *)malloc(400 * sizeof(char));
        sprintf(message, "File not Found at path : %s", filepath);
        log_system_errors(message);
        free(message);
        exit(0);
    } else {
        int size = file_size(file);
        char *contents = (char *)malloc((size + 1) * sizeof(char));
        size_t bytes_read = fread(contents, 1, size, file);
        if (bytes_read != (size_t)size) {
            free(contents);
            return NULL;
        } else {
            contents[size] = '\0';
            return contents;
        }
    }
}

void print_usage(char **argv) {
    printf("%sUSAGE: %s <path_to_file_to_compile>%s\n", RED, argv[0],
           COLOR_RESET);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv);
        exit(0);
    } else {
        char *filename = argv[1];
        char *contents = file_contents(filename);
        // printf("%s", contents);
        Node ROOT;
        Error err = parse(contents, ROOT);
        log_error(&err);
    }
    return 0;
}
