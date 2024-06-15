#include "headers/environment.h"
#include "headers/parser.h"
#include "parser.c"
#include "utils/colors.h"
#include "utils/errors.h"
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
        if (!message) {
            log_system_errors("COULD NOT ALLOCATE BUFFER FOR FILE CONTENTS");
        }
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
        if (contents) {
            ParsingContext *context = parse_context_create();

            Node *integer_hopefully = node_allocate();
            bool success = environment_get_by_symbol(*context->types, "integer",
                                                     integer_hopefully);
            if (success) {
                print_node(integer_hopefully, 0);
            } else {
                printf("Cannot get Node within the environment .\n");
            }
            free(integer_hopefully);

            Node *program = node_allocate();
            program->type = NODE_TYPE_PROGRAM;
            Node *expression = node_allocate();
            memset(expression, 0, sizeof(Node));
            char *contents_it = contents;

            while (true) {
                Error err =
                    parse_expr(context, contents_it, &contents_it, expression);
                if (!*contents_it) {
                    break;
                }
                if (err.type != ERROR_NULL) {
                    log_error(&err);
                    break;
                }
                node_add_child(program, expression);
            }

            print_node(program, 0);
            putchar('\n');

            free_nodes(program);
            free(contents);
        }
    }
    return 0;
}
