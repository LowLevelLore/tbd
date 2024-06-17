#ifndef _TBD_HEADERS_CODEGEN_GUARD
#define _TBD_HEADERS_CODEGEN_GUARD

#include "../utils/logging.h"
#include "error.h"
#include "parser.h"

#define MAX_ASM_LEN 9999999999

typedef enum CODEGEN_OUTPUT_FORMAT {
    CODEGEN_OUTPUT_FORMAT_DEFAULT = 0,
    CODEGEN_OUTPUT_FORMAT_x86_64_AT_T_ASM,
} CODEGEN_OUTPUT_FORMAT;

Error codegen_program(CODEGEN_OUTPUT_FORMAT, ParsingContext *, Node *, char *);
Error codegen_program_x86_64_att_asm(ParsingContext *, Node *, FILE *);

#endif