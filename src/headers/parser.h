#ifndef _MZ_HEADERS_PARSER_GUARD

#define _MZ_HEADERS_PARSER_GUARD

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/errors.h"
#include "environment.h"
#include "lexer.h"

typedef struct ParsingContext {
    Environment *types;
    Environment *variables;
} ParsingContext;

ParsingContext *parse_context_create();
bool parse_int(Token *, Node *);
void free_tokens(Token *);
Error parse_expr(ParsingContext *, char *, char **, Node *);

#endif
