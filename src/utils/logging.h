#ifndef _TBD_UTILS_LOGGING_GUARD
#define _TBD_UTILS_LOGGING_GUARD

#include <stdio.h>

#include "colors.h"
#include "errors.h"

void log_system_errors(char *message) {
    printf("%sUNKNOWN ERROR: %s%s\n", BRED, message, COLOR_RESET);
}

void log_error(Error *err) {
    switch (err->type) {
    case ERROR_NULL:
        break;
    case ERROR_TYPE:
        printf("%sTYPE ERROR: %s%s\n", BRED, err->message, COLOR_RESET);
        break;
    case ERROR_GENERIC:
        printf("%sGENERIC ERROR: %s%s\n", BRED, err->message, COLOR_RESET);
        break;
    case ERROR_SYNTAX:
        printf("%sSYNTAX ERROR: %s%s\n", BRED, err->message, COLOR_RESET);
        break;
    case ERROR_TODO:
        printf("%sTODO ERROR: %s%s\n", BRED, err->message, COLOR_RESET);
        break;
    case ERROR_ARGUMENTS:
        printf("%sARGUMENTS ERROR: %s%s\n", BRED, err->message, COLOR_RESET);
        break;
    default:
        printf("%sUNKNOWN ERROR: %s%s\n", BRED, err->message, COLOR_RESET);
        break;
    }
}

void log_message(char *message) {
    printf("%sLOG: %s%s\n", BCYN, message, COLOR_RESET);
}



#endif
