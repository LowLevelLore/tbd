#include <string.h>

#include "utils/errors.h"

const char* WHITESPACE = " \r\n";
const char* DELIMETER = " \r\n,():";

Error lex(char* source, char** begin, char** end) {
    Error err = OK;
    if (!source || !begin || !end) {
        err.type = ERROR_ARGUMENTS;
        err.message = "Cannot LEX empty source.";
        return err;
    }
    *begin = source;
    *begin += strspn(*begin, WHITESPACE);
    *end = *begin;
    if (**end == '\0') {
        return err;
    }
    *end += strcspn(*begin, DELIMETER);
    if (*begin == *end) {
        *end += 1;
    }
    return err;
}