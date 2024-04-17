#ifndef _TBD_HEADERS_ENVIRONMENT_GUARD

#define _TBD_HEADERS_ENVIRONMENT_GUARD
#include <stdbool.h>
typedef enum NodeType {
  NODE_TYPE_NULL,
  NODE_TYPE_PROGRAM,
  NODE_TYPE_SYMBOL,
  NODE_TYPE_BINARY_OPERATOR,
  NODE_TYPE_VARIABLE_DECLARATION,
  NODE_TYPE_VARIABLE_DECLARATION_WITH_INITIALIZATION,
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
void environment_set(Environment *, Node *, Node *);
bool node_compare(Node *, Node *);
Node *environment_get(Environment *, Node *);

#endif
