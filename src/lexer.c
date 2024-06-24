#include "headers/lexer.h"

Token *create_token() {
    Token *token = malloc(sizeof(Token));
    assert(token && "CANNOT ALLOCATE MEMORY FOR TOKEN");
    memset(token, 0, sizeof(Token));
    return token;
};

void print_token(Token t) {
    if (t.end - t.beginning < 1) {
        printf("INVALID TOKEN POINTERS");
    } else {
        printf("%.*s", (int)(t.end - t.beginning), t.beginning);
    }
}

bool comment_at_beginning(Token token) {
    const char *comment_it = COMMENT_DEIMETER;
    while (*comment_it) {
        if (*(token.beginning) == *comment_it) {
            return true;
        }
        comment_it++;
    }
    return false;
}

bool token_equals_string(char *string, Token *token) {
    char *beg = token->beginning;
    if (!string || !token) {
        return false;
    }
    while (*string && beg < token->end) {
        if (*beg != *string) {
            return false;
        }
        beg++;
        string++;
    }
    return true;
}

Error lex_advance(Token *token, size_t *token_length, char **end) {
    if (!token || !token_length || !end) {
        Error err;
        ERROR_PREP(err, ERROR_ARGUMENTS,
                   "lex_advance(): pointer arguments must not be NULL!");
        return err;
    }
    Error err = lex(token->end, token);
    *end = token->end;
    if (err.type != ERROR_NULL) {
        return err;
    }
    *token_length = token->end - token->beginning;
    return err;
}

ExpectReturnValue lex_expect(char *expected, Token *current,
                             size_t *current_length, char **end) {
    ExpectReturnValue out;
    out.done = 0;
    out.found = 0;
    out.err = OK;
    if (!expected || !current || !current_length || !end) {
        ERROR_PREP(out.err, ERROR_ARGUMENTS,
                   "lex_expect() must not be passed NULL pointers!");
        return out;
    }
    Token current_copy = *current;
    size_t current_length_copy = *current_length;
    char *end_value = *end;

    out.err = lex_advance(&current_copy, &current_length_copy, &end_value);
    if (out.err.type != ERROR_NULL) {
        return out;
    }
    if (current_length_copy == 0) {
        out.done = 1;
        return out;
    }

    if (token_equals_string(expected, &current_copy)) {
        out.found = 1;
        *end = end_value;
        *current = current_copy;
        *current_length = current_length_copy;
        return out;
    }

    return out;
}

Error lex(char *source, Token *token) {
    Error err = OK;
    if (!source || !token) {
        ERROR_PREP(err, ERROR_ARGUMENTS, "Can not lex empty source.");
        return err;
    }
    token->beginning = source;
    token->beginning += strspn(token->beginning, WHITESPACE);
    token->end = token->beginning;
    if (*(token->end) == '\0') {
        return err;
    }
    while (comment_at_beginning(*token)) {
        token->beginning = strpbrk(token->beginning, "\n");
        if (!token->beginning) {
            token->end = token->beginning;
            return err;
        }
        token->beginning += strspn(token->beginning, WHITESPACE);
        token->end = token->beginning;
    }
    if (*(token->end) == '\0') {
        return err;
    }
    token->end += strcspn(token->beginning, DELIMETER);
    if (token->end == token->beginning) {
        token->end += 1;
    }
    return err;
}
