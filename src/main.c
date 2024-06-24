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
    printf("\nUSAGE: %s [FLAGS] [OPTIONS] <path to file to compile>\n",
           argv[0]);
    printf("Flags:\n"
           "   `-h`, `--help`    :: Show this help and usage information.\n"
           "   `--formats`       :: List acceptable output formats.\n"
           "   `-v`, `--verbose` :: Print out more information.\n");
    printf(
        "Options:\n"
        "    `-o`, `--output`  :: Set the output filepath to the one given.\n"
        "    `-f`, `--format`  :: Set the output format to the one given.\n"
        "Anything other arguments are treated as input filepaths (source "
        "code).\n");
}

int input_filepath_index = -1;
int output_filepath_index = -1;
enum CodegenOutputFormat output_format = MZ_CODEGEN_OUTPUT_FORMAT_DEFAULT;
int verbosity = 1;

void print_acceptable_formats() {
    printf("Acceptable formats include:\n"
           " -> default\n"
           " -> x86_64-mswin\n");
}

int handle_command_line_arguments(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        char *argument = argv[i];

        // printf("argument %d: \"%s\"\n", i, argument);

        if (strcmp(argument, "-h") == 0 || strcmp(argument, "--help") == 0) {
            print_usage(argv);
            exit(0);
        } else if (strcmp(argument, "--formats") == 0) {
            print_acceptable_formats();
            exit(0);
        } else if (strcmp(argument, "-v") == 0 ||
                   strcmp(argument, "--verbose") == 0) {
            verbosity = 1;
        } else if (strcmp(argument, "-o") == 0 ||
                   strcmp(argument, "--output") == 0) {
            i++;
            if (i >= argc) {
                printf("ERROR: Expected filepath after output command line "
                       "argument\n");
                return 1;
            }
            // FIXME: This very well may be a valid filepath. We may want to
            //        check that it isn't a valid filepath with fopen or
            //        something.
            if (*argv[i] == '-') {
                printf("ERROR: Expected filepath after output command line "
                       "argument\n"
                       "Instead, got what looks like another command line "
                       "argument.\n"
                       " -> \"%s\"\n",
                       argv[i]);
                return 1;
            }
            output_filepath_index = i;
        } else if (strcmp(argument, "-f") == 0 ||
                   strcmp(argument, "--format") == 0) {
            i++;
            if (i >= argc) {
                printf("ERROR: Expected format after format command line "
                       "argument\n");
                return 1;
            }
            if (*argv[i] == '-') {
                printf("ERROR: Expected format after format command line "
                       "argument\n"
                       "Instead, got what looks like another command line "
                       "argument.\n"
                       " -> \"%s\"\n",
                       argv[i]);
                return 1;
            }
            if (strcmp(argv[i], "default") == 0) {
                output_format = MZ_CODEGEN_OUTPUT_FORMAT_DEFAULT;
            } else if (strcmp(argv[i], "x86_64-mswin") == 0) {
                output_format = MZ_CODEGEN_OUTPUT_FORMAT_x86_64_ASM_MSWIN;
            } else {
                printf("ERROR: Expected format after format command line "
                       "argument\n"
                       "Instead, got an unrecognized format: \"%s\".\n",
                       argv[i]);
                print_acceptable_formats();
                return 1;
            }
        } else {
            if (input_filepath_index != -1) {
                printf("ERROR: Only a single input filepath is used, but "
                       "multiple were given.\n"
                       "Using the latest one.\n");
            }
            input_filepath_index = i;
        }
    }
    return 0;
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
    }
    free(contents);
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv);
        return 0;
    }

    int status = handle_command_line_arguments(argc, argv);
    if (status) {
        return status;
    }
    if (input_filepath_index == -1) {
        printf("Input file path was not provided.");
        print_usage(argv);
        return 1;
    }

    Node *program = node_allocate();
    ParsingContext *context = parse_context_default_create();
    Error err = parse_program(argv[input_filepath_index], context, program);

    if (verbosity == 1) {
        printf("----- Abstract Syntax Tree\n");
        print_node(program, 0);
        printf("\n-----\n");
    } else if (verbosity == 2) {
        printf("----- Abstract Syntax Tree\n");
        print_node(program, 0);
        printf("\n\n----- Parsing Context\n");
        parsing_context_print(context, 0);
        printf("-----\n");
    }

    if (err.type) {
        log_error(&err);
        return 1;
    }

    err = typecheck_program(context, program);
    if (err.type) {
        log_error(&err);
        return 2;
    }

    char *output_filepath =
        output_filepath_index == -1 ? "code.S" : argv[output_filepath_index];
    err = codegen_program(context, program, MZ_CODEGEN_OUTPUT_FORMAT_DEFAULT,
                          output_filepath);
    if (err.type) {
        log_error(&err);
        return 3;
    }

    printf("\nGenerated code at output filepath \"%s\"\n", output_filepath);

    free_nodes(program);
    return 0;
}
