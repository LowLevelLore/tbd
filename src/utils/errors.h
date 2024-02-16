#ifndef _TBD_UTILS_ERRORS_GUARD
#define _TBD_UTILS_ERRORS_GUARD
#include <stdio.h>

typedef enum ErrorType {
    ERROR_NULL = 0,
    ERROR_TYPE,
    ERROR_GENERIC,
    ERROR_SYNTAX,
    ERROR_TODO,
    ERROR_ARGUMENTS,
} ErrorType;

typedef struct Error {
    ErrorType type;
    char *message;
} Error;

Error OK = {ERROR_NULL, NULL};
#endif