#include "headers/lexer.h"

Token *create_token()
{
    Token *token = malloc(sizeof(Token));
    assert(token && "CANNOT ALLOCATE MEMORY FOR TOKEN");
    memset(token, 0, sizeof(Token));
    return token;
};

bool token_equals_string(Token *token, char *string)
{
    char *beg = token->beginning;
    if (!string || !token)
    {
        return false;
    }
    while (*string && beg < token->end)
    {
        if (*beg != *string)
        {
            return false;
        }
        beg++;
        string++;
    }
    return true;
}

Error lex(char *source, Token *token)
{
    Error err = OK;
    if (!source || !token)
    {
        err.type = ERROR_ARGUMENTS;
        err.message = "Cannot LEX empty source.";
        return err;
    }
    token->beginning = source;
    token->beginning += strspn(token->beginning, WHITESPACE);
    token->end = token->beginning;
    if (*(token->end) == '\0')
    {
        return err;
    }
    token->end += strcspn(token->beginning, DELIMETER);
    if (token->beginning == token->end)
    {
        token->end += 1;
    }
    return err;
}