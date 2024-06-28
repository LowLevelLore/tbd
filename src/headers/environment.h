#ifndef _MZ_HEADERS_ENVIRONMENT_GUARD

#define _MZ_HEADERS_ENVIRONMENT_GUARD

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define nonep(node) ((node).type == NODE_TYPE_NULL)
#define integerp(node) ((node).type == NODE_TYPE_INTEGER)
#define symbolp(node) ((node).type == NODE_TYPE_SYMBOL)

#include <stdbool.h>
typedef enum NodeType {
    NODE_TYPE_NULL,
    // A program contains expressions as its children
    NODE_TYPE_PROGRAM,
    // Hold a symbol
    NODE_TYPE_SYMBOL,

    NODE_TYPE_BINARY_OPERATOR,

    // To debug print an integer
    // Syntax: pint var_name
    // NODE_TYPE_DEBUG_PRINT_INTEGER
    //      --> NODE_TYPE_SYMBOL
    NODE_TYPE_DEBUG_PRINT_INTEGER,

    NODE_TYPE_VARIABLE_DECLARATION,
    NODE_TYPE_VARIABLE_REASSIGNMENT,
    NODE_TYPE_VARIABLE_ACCESS,
    NODE_TYPE_INTEGER,
    NODE_TYPE_FLOAT,
    NODE_TYPE_STRING,

    /*
        NODE_TYPE_FUNCTION_DECLARATION
            1. PARAM_LIST
                // 0. INITIAL ARG
                a. PARAM 1
                b. PARAM 2
            2. RETURN_TYPE
            3. EXPRESSION/PROGRRAM
    */
    NODE_TYPE_FUNCTION_DECLARATION,
    NODE_TYPE_FUNCTION_PARAMS_LIST,
    NODE_TYPE_FUNCTION_PARAM,
    NODE_TYPE_FUNCTION_RETURN_TYPE,
    // NODE_TYPE_INITIAL_ARG,

    /*
        NODE_TYPE_FUNCTION_CALL
            1. Function Symbol
            2. Parameter List
    */
    NODE_TYPE_IF,
    NODE_TYPE_CONDITION,
    NODE_TYPE_ELSE, // Do we really need this ?
    NODE_TYPE_FUNCTION_CALL,
    NODE_TYPE_FUNCTION_ARGS_LIST,
} NodeType;

typedef union NodeValue {
    long long int MZ_integer;
    double MZ_double;
    char *symbol;
} NodeValue;

typedef struct Node {
    int type;
    NodeValue value;
    struct Node *parent;
    struct Node *children;
    struct Node *next_child;
    int result_register;
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

void print_environment(Environment *env, size_t indent_level);
Environment *environment_create(Environment *);
int environment_set(Environment *, Node *, Node *);
bool environment_get(Environment, Node *, Node *);
bool environment_get_by_symbol(Environment, char *, Node *);
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
