#ifndef _MZ_HEADERS_ENVIRONMENT_GUARD

#define _MZ_HEADERS_ENVIRONMENT_GUARD

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define nullp(node) ((node).type == NODE_TYPE_NULL)
#define integerp(node) ((node).type == NODE_TYPE_INTEGER)
#define symbolp(node) ((node).type == NODE_TYPE_SYMBOL)

#include <stdbool.h>
typedef enum NodeType {
    NODE_TYPE_NULL,
    NODE_TYPE_PROGRAM,
    NODE_TYPE_SYMBOL,
    NODE_TYPE_BINARY_OPERATOR,

    NODE_TYPE_VARIABLE_DECLARATION,
    NODE_TYPE_VARIABLE_REASSIGNMENT,
    NODE_TYPE_VARIABLE_DECLARATION_WITH_INITIALIZATION,
    NODE_TYPE_INTEGER,
    NODE_TYPE_FLOAT,
    NODE_TYPE_STRING,

    /*
        NODE_TYPE_FUNCTION_DECLARATION
            1. PARAM_LIST
                a. PARAM 1
                b. PARAM 2
            2. RETURN_TYPE
            3. EXPRESSION/PROGRRAM
    */
    NODE_TYPE_FUNCTION_DECLARATION,
    NODE_TYPE_FUNCTION_PARAMS_LIST,
    NODE_TYPE_FUNCTION_PARAM,
    NODE_TYPE_FUNCTION_RETURN_TYPE,
} NodeType;

typedef union NodeValue {
    long long int MZ_integer;
    double MZ_double;
    char *symbol;
} NodeValue;

typedef struct Node {
    int type;
    NodeValue value;
    struct Node *children;
    struct Node *next_child;
} Node;

typedef struct Binding {
    Node *id;
    Node *value;
    struct Binding *next;
} Binding;

typedef struct Environment {
    struct Environment *parent;
    Binding *binding;
} Environment;

void free_nodes(Node *);

Environment *environment_create(Environment *);
int environment_set(Environment*, Node *, Node *);
bool environment_get(Environment, Node *, Node *);
bool environment_get_by_symbol(Environment , char *, Node *);
Node *node_allocate();
void node_add_child(Node *, Node *);
void free_nodes(Node *);
void print_node(Node *, size_t);
int node_compare(Node *, Node *);
Node *node_symbol_from_buffer(char *, size_t);
Node *node_integer(long long);
Node *node_float(double);
Node *node_symbol(char *);
bool node_copy(Node *source, Node *destination);

#endif
