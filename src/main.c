#define DEBUG_COMPILER
#include "headers/codegen/main.h"
#include "headers/environment.h"
#include "headers/parser.h"
#include "utils/colors.h"
#include "utils/errors.h"
#include "utils/logging.h"

#include "codegen/main.c"
#include "parser.c"
#include "typechecker.c"

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
    printf("%sUSAGE: %s <path_to_file_to_compile> <output_file>%s\n", RED,
           argv[0], COLOR_RESET);
}

Error parse_program(char *filepath, ParsingContext *context, Node *result) {
    Error err = OK;
    char *contents = file_contents(filepath);
    if (!contents) {
        printf("Filepath: \"%s\"\n", filepath);
        ERROR_PREP(err, ERROR_GENERIC,
                   "parse_program(): Couldn't get file contents");
        return err;
    }
    result->type = NODE_TYPE_PROGRAM;
    char *contents_it = contents;
    for (;;) {
        Node *expression = node_allocate();
        node_add_child(result, expression);
        err = parse_expr(context, contents_it, &contents_it, expression);
        if (err.type != ERROR_NULL) {
            free(contents);
            return err;
        }
        // Check for end-of-parsing case (source and end are the same).
        if (!(*contents_it)) {
            break;
        }

        // printf("Parsed expression:\n");
        // print_node(expression,0);
        // putchar('\n');
    }
    free(contents);
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage(argv);
        exit(0);
    } else {
        char *filename = argv[1];
        char *contents = file_contents(filename);
        if (contents) {
            Node *program = node_allocate();
            ParsingContext *context = parse_context_default_create();
            Error err = parse_program(argv[1], context, program);
            // print_node(program, 0);
            putchar('\n');
            if (err.type != ERROR_NULL) {
                log_error(&err);
                exit(1);
            }
            err = typecheck_program(context, program);
            if (err.type != ERROR_NULL) {
                log_error(&err);
                exit(1);
            }
            log_message("GENERATING CODE \n");
            err = codegen_program(context, program,
                                  MZ_CODEGEN_OUTPUT_FORMAT_x86_64_ASM_MSWIN,
                                  argv[2]);
            if (err.type != ERROR_NULL)
                log_error(&err);
            else {
                printf("\n");
                log_message("COMPLETED GENERATION");
            }
            free_nodes(program);
        }
    }
    return 0;
}
