#ifndef _TBD_HEADERS_PARSER_GUARD

#define _TBD_HEADERS_PARSER_GUARD

#include <stdbool.h>

#include "../utils/errors.h"
#include "environment.h"
#include "lexer.h"

bool parse_int(Token *, Node *);
void free_tokens(Token *);
Error parse(char *, Node);

#endif
