#ifndef _MZ_HEADERS_CODEGEN_REGISTERS_GUARD
#define _MZ_HEADERS_CODEGEN_REGISTERS_GUARD

#include "../../utils/logging.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef int RegisterDescriptor;

typedef struct Register {
    struct Register *next;
    char *name;
    bool in_use;
} Register;

Register *register_create(char *name);
void register_add(Register *base, char *name);
void register_free(Register *base);
RegisterDescriptor register_allocate(Register *base);
void register_deallocate(Register *base, RegisterDescriptor rd);
char *register_name(Register *base, RegisterDescriptor register_descriptor);

#endif