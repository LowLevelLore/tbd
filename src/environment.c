#include "headers/environment.h"
#include "utils/colors.h"
#include "utils/errors.h"
#include "utils/logging.h"

void print_node_internal(Node *node, size_t indent_level, char *color,
                         bool new_line) {
    if (!node) {
        return;
    }
    if (new_line)
        printf("\n");
    for (size_t i = 0; i < indent_level; ++i) {
        putchar(' ');
    }
    printf(color);
    switch (node->type) {
    default:
        printf("[UNKNOWN]");
        break;
    case NODE_TYPE_NULL:
        printf("[NONE]");
        break;
    case NODE_TYPE_INTEGER:
        printf("[INT]: %lld", node->value.MZ_integer);
        break;
    case NODE_TYPE_SYMBOL:
        printf("[SYM]");
        if (node->value.symbol) {
            printf(": %s", node->value.symbol);
        }
        break;
    case NODE_TYPE_BINARY_OPERATOR:
        printf("[BINARY OPERATOR %s]", node->value.symbol);
        break;
    case NODE_TYPE_VARIABLE_DECLARATION:
        printf("[VARIABLE DECLARATION]");
        break;
    case NODE_TYPE_VARIABLE_REASSIGNMENT:
        printf("[VARIABLE REASSIGNMENT]");
        break;
    case NODE_TYPE_VARIABLE_ACCESS:
        printf("[VARIABLE ACCESS] : ");
        break;
    case NODE_TYPE_PROGRAM:
        printf("[PROGRAM] : ");
        break;
    case NODE_TYPE_FUNCTION_DECLARATION:
        printf("[NODE_TYPE_FUNCTION_DECLARATION] : ");
        break;
    case NODE_TYPE_FUNCTION_PARAM:
        printf("[NODE_TYPE_FUNCTION_PARAM] : ");
        break;
    case NODE_TYPE_FUNCTION_PARAMS_LIST:
        printf("[NODE_TYPE_FUNCTION_PARAMS_LIST] : ");
        break;
    case NODE_TYPE_FUNCTION_RETURN_TYPE:
        printf("[NODE_TYPE_FUNCTION_RETURN_TYPE] : ");
        break;
    case NODE_TYPE_FUNCTION_CALL:
        printf("[NODE_TYPE_FUNCTION_CALL] : ");
        break;
    case NODE_TYPE_FUNCTION_ARGS_LIST:
        printf("[NODE_TYPE_FUNCTION_ARGS_LIST] : ");
        break;
    case NODE_TYPE_IF:
        printf("[IF STATEMENT] : ");
        break;
    case NODE_TYPE_CONDITION:
        printf("[CONDITION] : ");
        break;
    case NODE_TYPE_ELSE:
        printf("[ELSE STATEMENT] : ");
        break;
    case NODE_TYPE_DEBUG_PRINT_INTEGER:
        printf("[NODE_TYPE_DEBUG_PRINT_INTEGER] : ");
        break;
    }

    printf(COLOR_RESET);
    Node *child = node->children;
    while (child) {
        print_node(child, indent_level + 4);
        child = child->next_child;
    }
}

Environment *environment_create(Environment *parent) {
    Environment *env = (Environment *)malloc(sizeof(Environment));
    assert(env && "Unable to allocate memory for environment.");
    env->parent = parent;
    env->binding = NULL;
    return env;
}

int environment_set(Environment *env, Node *id, Node *value) {
    if (!env || !id || !value) {
        return 0;
    }
    Binding *binding_it = env->binding;
    while (binding_it) {
        if (node_compare(binding_it->id, id)) {
            binding_it->value = value;
            return 2;
        }
        binding_it = binding_it->next;
    }
    Binding *binding = malloc(sizeof(Binding));
    assert(binding && "Could not allocate new binding for environment");
    binding->id = id;
    binding->value = value;
    binding->next = env->binding;
    env->binding = binding;
    return 1;
}

bool environment_get(Environment env, Node *id, Node *result) {
    Binding *binding_it = env.binding;
    while (binding_it) {
        if (node_compare(binding_it->id, id)) {
            *result = *binding_it->value;
            return true;
        }
        binding_it = binding_it->next;
    }
    return false;
}

void print_environment(Environment *env, size_t indent_level) {
    Binding *binding_it = env->binding;
    while (binding_it) {
        print_node_internal(binding_it->id, indent_level, BYEL, false);
        printf("\n");
        print_node_internal(binding_it->value, indent_level, BGRN, false);
        printf("\n");
        binding_it = binding_it->next;
    }
}

bool environment_get_by_symbol(Environment env, char *symbol, Node *result) {
    Node *sym_node = node_symbol(symbol);
    bool status = environment_get(env, sym_node, result);
    free(sym_node);
    return status;
}

Node *node_allocate() {
    Node *node = calloc(1, sizeof(Node));
    assert(node && "Could not allocate memory for AST node");
    return node;
}

void node_add_child(Node *parent, Node *new_child) {
    if (!parent || !new_child) {
        return;
    }
    new_child->parent = parent;
    if (parent->children) {
        Node *child = parent->children;
        while (child->next_child) {
            child = child->next_child;
        }
        child->next_child = new_child;
    } else {
        parent->children = new_child;
    }
}

void free_nodes(Node *node) {
    if (!node) {
        return;
    } else {
        Node *child = node->children;
        Node *next_child = NULL;
        while (child) {
            next_child = child->next_child;
            free_nodes(child);
            child = next_child;
        }
        free(node);
    }
}

void print_node(Node *node, size_t indent_level) {
    print_node_internal(node, indent_level, BCYN, true);
}

Error define_type(Environment *types, int type, Node *type_symbol,
                  long long byte_size) {
    assert(types && "Can not add type to NULL types environment");
    assert(type_symbol && "Can not add NULL type symbol to types environment");
    assert(byte_size >= 0 &&
           "Can not define new type with zero or negative byte size");

    Node *size_node = node_allocate();
    size_node->type = NODE_TYPE_INTEGER;
    size_node->value.MZ_integer = byte_size;

    Node *type_node = node_allocate();
    type_node->type = type;
    type_node->children = size_node;

    if (environment_set(types, type_symbol, type_node) == 1) {
        return OK;
    }
    // TYPE REDEFINITION ERROR
    printf("%sType that was redefined: \"%s\"\n%s", BRED,
           type_symbol->value.symbol, COLOR_RESET);
    Error err;
    ERROR_PREP(err, ERROR_TYPE, "Redefinition of type!");
    return err;
}

Node *node_none() {
    Node *none = node_allocate();
    none->type = NODE_TYPE_NULL;
    return none;
}

int node_compare(Node *a, Node *b) {
    if (!a || !b) {
        if (!a && !b) {
            return 1;
        }
        return 0;
    }

    if (a->type != b->type) {
        return 0;
    }
    switch (a->type) {
    case NODE_TYPE_NULL:
        if (nonep(*b)) {
            return 1;
        }
        return 0;
        break;
    case NODE_TYPE_INTEGER:
        if (a->value.MZ_integer == b->value.MZ_integer) {
            return 1;
        }
        return 0;
        break;
    case NODE_TYPE_FLOAT:
        return a->value.MZ_double == b->value.MZ_double;
        break;
    case NODE_TYPE_STRING:
        printf("TODO: Handle NODE_TYPE_STRING comparison");
        break;
    case NODE_TYPE_SYMBOL:
        if (a->value.symbol && b->value.symbol) {
            if (strcmp(a->value.symbol, b->value.symbol) == 0) {
                return 1;
            }
            return 0;
        } else if (!a->value.symbol && !b->value.symbol) {
            return 1;
        }
        return 0;
        break;
    case NODE_TYPE_BINARY_OPERATOR:
        printf("TODO: node_compare() BINARY OPERATOR\n");
        break;

    case NODE_TYPE_VARIABLE_REASSIGNMENT:
        printf("TODO: node_compare() NODE TYPE VARIABLE REASSIGNMENT\n");
        break;
    case NODE_TYPE_VARIABLE_DECLARATION:
        printf("TODO: node_compare() VARIABLE DECLARATION\n");
        break;

    case NODE_TYPE_DEBUG_PRINT_INTEGER:
        printf("TODO: node_compare() VARIABLE DECLARATION\n");
        break;

    case NODE_TYPE_PROGRAM:
        printf("TODO: Compare two programs.\n");
        break;
    }
    return 0;
}

Node *node_symbol_from_buffer(char *buffer, size_t length) {
    assert(buffer && "Can not create AST symbol node from NULL buffer");
    char *symbol_string = malloc(length + 1);
    assert(symbol_string && "Could not allocate memory for symbol string");
    memcpy(symbol_string, buffer, length);
    symbol_string[length] = '\0';
    Node *symbol = node_allocate();
    symbol->type = NODE_TYPE_SYMBOL;
    symbol->value.symbol = symbol_string;
    return symbol;
}

Node *node_integer(long long value) {
    Node *integer = node_allocate();
    integer->type = NODE_TYPE_INTEGER;
    integer->value.MZ_integer = value;
    integer->children = NULL;
    integer->next_child = NULL;
    return integer;
}

Node *node_float(double value) {
    Node *integer = node_allocate();
    integer->type = NODE_TYPE_FLOAT;
    integer->value.MZ_double = value;
    integer->children = NULL;
    integer->next_child = NULL;
    return integer;
}

Node *node_symbol(char *symbol_string) {
    Node *symbol = node_allocate();
    symbol->type = NODE_TYPE_SYMBOL;
    symbol->value.symbol = strdup(symbol_string);
    return symbol;
}

bool node_copy(Node *source, Node *destination) {
    if (!source || !destination) {
        return false;
    } else {
        memcpy(destination, source, sizeof(Node));
        destination->type = source->type;
        switch (source->type) {
        case NODE_TYPE_SYMBOL:
            strcpy(destination->value.symbol, source->value.symbol);
            assert(destination->value.symbol &&
                   "node_copy(): Could not allocate memory for new symbol.");

            break;

        default:
            destination->value = source->value;
            break;
        }

        Node *child = source->children;
        Node *child_it = NULL;
        while (child) {
            Node *new_child = node_allocate();
            if (child_it) {
                child_it->next_child = new_child;
            } else {
                destination->children = new_child;
                child_it = new_child;
            }
            node_copy(child, new_child);
            child = child->next_child;
        }

        return true;
    }
}
