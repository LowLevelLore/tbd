#include "headers/codegen.h"

Error codegen_program_x86_64_att_asm(ParsingContext *context, Node *program,
                                     FILE *outfile) {
    Error err = OK;
    Node *expression = program->children;
    Node *temp_node = node_allocate();

    while (expression) {
        switch (expression->type) {
        case NODE_TYPE_VARIABLE_DECLARATION:
            // TODO: Handle nested scopes (stack based variables)

            temp_node = expression->children;
            print_node(temp_node, 0);
            print_node(temp_node->next_child, 4);
            environment_get(context->variables, expression->children,
                            temp_node);
            print_node(temp_node, 4);
            Node *size_node = node_allocate();
            if (environment_get(context->types, temp_node, size_node)) {
                print_node(size_node, 8);
            }
            break;

        default:
            break;
        }
        expression = expression->next_child;
    }
    return err;
}

Error codegen_program(CODEGEN_OUTPUT_FORMAT format, ParsingContext *context,
                      Node *program, char *output_path) {
    Error err = OK;
    if (!context) {
        ERROR_PREP(err, ERROR_ARGUMENTS,
                   "codegen_program(): A non-null context should be passed...");
        return err;
    }
    if (!program || !(program->type == NODE_TYPE_PROGRAM)) {
        ERROR_PREP(err, ERROR_ARGUMENTS,
                   "codegen_program(): Invalid arguments recieved...");
        return err;
    }

    FILE *output = fopen(output_path, "wb");
    if (!output) {
        ERROR_PREP(err, ERROR_GENERIC,
                   "codegen_program(): Cannot open code file...");
        return err;
    }

    switch (format) {
    case CODEGEN_OUTPUT_FORMAT_DEFAULT:
        log_message("CODEGEN_OUTPUT_FORMAT_DEFAULT not supported...");
        break;
    case CODEGEN_OUTPUT_FORMAT_x86_64_AT_T_ASM:
        return codegen_program_x86_64_att_asm(context, program, output);
        break;
    default:
        break;
    }
    return err;
}