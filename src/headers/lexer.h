#ifndef _MZ_HEADERS_LEXER_GUARD

#define _MZ_HEADERS_LEXER_GUARD

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/errors.h"
#include "../utils/logging.h"

const char *NEWLINE = "\r\n";
const char *COMMENT_DEIMETER = "#";
const char *WHITESPACE = " \r\n";
const char *DELIMETER = " \r\n,():=+*/{}[]";

typedef struct Token {
    char *beginning;
    char *end;
} Token;

typedef struct ExpectReturnValue {
    Error err;
    char found;
    char done;
} ExpectReturnValue;

#define EXPECT(expected, expected_string, current_token, current_length, end)  \
    expected =                                                                 \
        lex_expect(expected_string, &current_token, &current_length, end);     \
    if (expected.err.type) {                                                   \
        return expected.err;                                                   \
    }                                                                          \


bool comment_at_beginning(Token);
bool token_equals_string(char *, Token *);
Error lex(char *, Token *);
Error lex_advance(Token *, size_t *, char **);
ExpectReturnValue lex_expect(char *, Token *, size_t *, char **);
void print_token(Token);

#endif
