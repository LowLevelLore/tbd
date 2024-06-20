#ifndef _MZ_HEADERS_PARSER_GUARD

#define _MZ_HEADERS_PARSER_GUARD

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/errors.h"
#include "codegen.h"
#include "environment.h"
#include "lexer.h"

#define MAX_ARGS_ITER 200

typedef struct ParsingContext {
    struct ParsingContext *parent;
    Node *operator;
    Environment *types;
    Environment *variables;
    Environment *functions;
    Node *result;
} ParsingContext;

ParsingContext *parse_context_create(ParsingContext *);
ParsingContext *parse_context_default_create();
bool parse_int(Token *, Node *);
Error parse_get_type(ParsingContext *context, Node *id, Node *result);
void free_tokens(Token *);
Error parse_expr(ParsingContext *, char *, char **, Node *);

#endif
