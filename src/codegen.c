#include "headers/codegen.h"

Error fwrite_(char *bytestring, FILE *file) {
    Error err;
    ERROR_PREP(err, ERROR_GENERIC, "fwrite_() cannot write to the output file.")
    if (!file) {
        return err;
    }
    size_t len = strlen(bytestring);
    if (fwrite(bytestring, 1, len, file) != len) {
        return err;
    }
    return OK;
}

Error fwrite_line(char *bytestring, FILE *file) {
    Error err;
    ERROR_PREP(err, ERROR_GENERIC,
               "fwrite_line() cannot write to the output file.")
    if (!file) {
        return err;
    }
    size_t len = strlen(bytestring);
    if (fwrite(bytestring, 1, len, file) != len) {
        return err;
    }
    if (fwrite("\n", 1, 1, file) != 1) {
        return err;
    }
    return OK;
}

Error fwrite_integer(long long integer, FILE *file) {
    Error err;
    ERROR_PREP(err, ERROR_GENERIC,
               "fwrite_integer() cannot write to the output file.")
    if (!file) {
        return err;
    }
    sprintf(number, "%lld", integer);
    return fwrite_(number, file);
}

Error codegen_program_x86_64_att_asm_data_section(ParsingContext *context,
                                                  FILE *outfile) {

    Error err = OK;
    err = fwrite_line(".section .data", outfile);
    ret;
    Binding *it = context->variables->binding;
    Node *id, *type_node;
    Node *size_node = node_allocate();
    while (it) {
        id = it->id;
        type_node = it->value;
        environment_get(context->types, type_node, size_node);
        err = fwrite_(id->value.symbol, outfile);
        ret;
        err = fwrite_(": .space ", outfile);
        ret;
        err = fwrite_integer(size_node->value.MZ_integer, outfile);
        ret;
        err = fwrite_("\n", outfile);
        ret;
        it = it->next;
    }
    return err;
}

Error codegen_program_x86_64_att_asm(ParsingContext *context, Node *program,
                                     FILE *outfile) {
    Error err = OK;
    Node *expression = program->children;
    fwrite_((char *)";#; ", outfile);
    fwrite_line((char *)CODEGEN_HEADER, outfile);

    // For all global variables
    err = codegen_program_x86_64_att_asm_data_section(context, outfile);
    ret;

    char *lines[6] = {".global _start", ".section .text", "_start: ",
                      "push %rbp",      "mov %rsp, %rbp", ""};

    for (int i = 0; i < 6; i++) {
        fwrite_line(lines[i], outfile);
        ret;
    }

    while (expression) {
        switch (expression->type) {
        default:
            break;
        case NODE_TYPE_VARIABLE_REASSIGNMENT:
            // Expression -> next_child -> next_child ....
            //       --> [SYM]
            //       --> [VALUE]
            err = fwrite_("lea ", outfile);
            err = fwrite_(expression->children->value.symbol, outfile);
            err = fwrite_line("(%rip), %rax", outfile);
            // Assumes to move 8 bytes by default
            err = fwrite_("movq $", outfile);
            // Assumes Integer :(
            err = fwrite_integer(
                expression->children->next_child->value.MZ_integer, outfile);
            err = fwrite_line(", (%rax)", outfile);
            ret;
            break;
        }
        expression = expression->next_child;
    }

    /*
        TODO: Different exit procedres for linux and windows
        # Exit for linux
        movl    $0, %ebx
        movl    $1, %eax
        int     $0x80
        # Exit for Windows
        movq    $0, %rax
        ret
    */

    fwrite_line("", outfile);
    fwrite_line("pop %rbp", outfile);
    fwrite_line("movl $0, %ebx", outfile);
    fwrite_line("movl $1, %eax", outfile);
    fwrite_line("int $0x80", outfile);
    ret;

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