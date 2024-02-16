#include <stdbool.h>

#include "lexer.c"
#include "utils/errors.h"

Error parse(char *contents) {
    char *beginning = contents;
    char *ending = contents;
    Error err = OK;
    while ((err = lex(ending, &beginning, &ending)).type == ERROR_NULL) {
        if ((ending - beginning == 0)) {
            break;
        }
        printf("LEXED : %.*s\n", (int)(ending - beginning), beginning);
    }

    return err;
}