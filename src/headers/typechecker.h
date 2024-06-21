#ifndef _MZ_HEADERS_TYPECHECKER_GUARD

#define _MZ_HEADERS_TYPECHECKER_GUARD

#include "../utils/errors.h"
#include "environment.h"
#include "parser.h"

Error typecheck_expression(ParsingContext *context, Node *expression);
Error typecheck_program(ParsingContext *context, Node *program);

#endif