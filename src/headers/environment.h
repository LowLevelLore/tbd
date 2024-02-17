#ifndef _TBD_HEADERS_ENVIRONMENT_GUARD

#define _TBD_HEADERS_ENVIRONMENT_GUARD

typedef enum NodeType {
    NODE_TYPE_NULL,
    NODE_TYPE_PROGRA,
    NODE_TYPE_INTEGER,
    NODE_TYPE_FLOAT,
    NODE_TYPE_STRING,
} NodeType;

typedef union NodeValue {
    long long int tbd_integer;
    double tbd_double;
} NodeValue;

typedef struct Node {
    NodeType type;
    NodeValue value;
    struct Node **children;
} Node;

typedef struct Binding {
    char *id;
    Node *value;
    struct Binding *next;
} Binding;

typedef struct Environment {
    struct Environment *parent;
    Binding *binding;
} Environment;

#endif
