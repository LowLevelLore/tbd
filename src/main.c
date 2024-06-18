#include "codegen.c"
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
            bool success = environment_get_by_symbol(context->types, "integer",
                                                     integer_hopefully);
            if (!success) {
                printf("%sCannot get Node within the environment. %s\n", BRED,
                       COLOR_RESET);
            }
            free(integer_hopefully);

            Node *program = node_allocate();
            program->type = NODE_TYPE_PROGRAM;
            Node *expression = node_allocate();
            memset(expression, 0, sizeof(Node));
            char *contents_it = contents;

            for (;;) {
                Error err =
                    parse_expr(context, contents_it, &contents_it, expression);
                if (err.type != ERROR_NULL) {
                    log_error(&err);
                    break;
                }
                if (!(*contents_it)) {
                    break;
                }
                Node *child = node_allocate();
                node_copy(expression, child);
                node_add_child(program, child);
            }

            free_nodes(expression);

            print_node(program, 0);
            putchar('\n');

            log_message("GENERATING CODE \n\n");

            codegen_program(CODEGEN_OUTPUT_FORMAT_x86_64_AT_T_ASM, context,
                            program, "test.txt");

            free_nodes(program);
            free(contents);
        }
    }
    return 0;
}
