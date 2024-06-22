#include "../headers/codegen/registers.h"

Register *register_create(char *name) {
    if (!name) {
        log_message("A register must have a name");
    }
    Register *r = calloc(1, sizeof(Register));
    assert(r && "Could not allocate memory for new register");
    r->name = name;
    return r;
}

void register_add(Register *base, char *name) {
    while (base->next) {
        base = base->next;
    }
    base->next = register_create(name);
}

void register_free(Register *base) {
    Register *to_free = NULL;
    while (base) {
        to_free = base;
        base = base->next;
        if (to_free->name) {
            free(to_free);
        }
    }
}

RegisterDescriptor register_allocate(Register *base) {
    RegisterDescriptor rd = 0;
    while (base) {
        if (!base->in_use) {
            base->in_use = true;
            return rd;
        }
        base = base->next;
        rd++;
    }
    log_message("register_allocate() Could not allocate register !");
    exit(1);
    return -1;
}

void register_deallocate(Register *base, RegisterDescriptor rd) {
    while (base) {
        if (!rd) {
            base->in_use = false;
            return;
        }
        base = base->next;
        rd--;
    }
    log_message("register_deallocate() Could not deallocate register !");
    exit(1);
}

char *register_name(Register *base, RegisterDescriptor register_descriptor) {
    while (base) {
        if (register_descriptor <= 0) {
            return base->name;
        }
        register_descriptor--;
        base = base->next;
    }
    return "This should'nt happen !!!! <SCREAM> :( WE ARE DOOMED";
}