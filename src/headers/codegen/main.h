#ifndef _MZ_HEADERS_CODEGEN_GUARD
#define _MZ_HEADERS_CODEGEN_GUARD

#include "../../utils/errors.h"
#include "../environment.h"
#include "../parser.h"
#include "registers.h"

typedef enum CodegenOutputFormat {
    MZ_CODEGEN_OUTPUT_FORMAT_DEFAULT,
    MZ_CODEGEN_OUTPUT_FORMAT_x86_64_ASM_MSWIN
} CodegenOutputFormat;

typedef struct CodegenContext {
    struct CodegenContext *parent;
    // LOCALS
    //      --> SYMBOL(NAME) -> INTEGER(STACK OFFSET)
    Environment *locals;
} CodegenContext;

#define CG_MAX_INT_BUFFER_LENGTH 32
char CG_INT_BUFFER[CG_MAX_INT_BUFFER_LENGTH];

#define CG_LABEL_BUFFER_LENGTH 1024
size_t label_index;
size_t label_count = 0;
char CG_LABEL_BUFFER[CG_LABEL_BUFFER_LENGTH];

#define CG_SYMBOL_BUFFER_LENGTH 1024
size_t symbol_index;
size_t symbol_count = 0;
char CG_SYMBOL_BUFFER[CG_SYMBOL_BUFFER_LENGTH];

#define ret                                                                    \
    if (err.type != ERROR_NULL) {                                              \
        return err;                                                            \
    }

#define CG_WRITE_BUFFER_LENGTH 2048
char CG_WRITE_BUFFER[CG_WRITE_BUFFER_LENGTH];

void write_bytes(char *code, FILE *outfile);
void write_line(char *code, FILE *outfile);
void write_integer(long long integer, FILE *outfile);

#define BYTES(fmt, ...)                                                        \
    sprintf(CG_WRITE_BUFFER, fmt, ##__VA_ARGS__);                              \
    write_bytes(CG_WRITE_BUFFER, outfile);

#define LINE(fmt, ...)                                                         \
    sprintf(CG_WRITE_BUFFER, fmt, ##__VA_ARGS__);                              \
    write_line(CG_WRITE_BUFFER, outfile);

char *label_generate();

Error codegen_program_x86_64_mswin(ParsingContext *context,
                                   CodegenContext *cg_context, Node *program,
                                   FILE *outfile);
Error codegen_program(ParsingContext *context, Node *program,
                      CodegenOutputFormat format, char *filepath);

#endif