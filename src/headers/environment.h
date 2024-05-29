#ifndef _TBD_HEADERS_ENVIRONMENT_GUARD

#define _TBD_HEADERS_ENVIRONMENT_GUARD

#define nullp(node) ((node).type == NODE_TYPE_NULL)
#define integerp(node) ((node).type == NODE_TYPE_INTEGER)
#define symbolp(node) ((node).type == NODE_TYPE_SYMBOL)

#include <stdbool.h>
typedef enum NodeType
{
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

typedef union NodeValue
{
  long long int tbd_integer;
  double tbd_double;
  char *symbol;
} NodeValue;

typedef struct Node
{
  NodeType type;
  NodeValue value;
  struct Node *children;
  struct Node *next_child;
} Node;

typedef struct Binding
{
  Node *id;
  Node *value;
  struct Binding *next;
} Binding;

typedef struct Environment
{
  struct Environment *parent;
  Binding *binding;
} Environment;

void free_nodes(Node *);

Environment *environment_create(Environment *);
int environment_set(Environment *, Node *, Node *);
bool environment_get(Environment, Node *, Node *);
Node *node_allocate();
void node_add_child(Node *, Node *);
void free_nodes(Node *);
void print_node(Node *, size_t);
int node_compare(Node *, Node *);
Node *node_symbol_from_buffer(char *, size_t);
Node *node_integer(long long);
Node *node_float(double);
Node *node_symbol(char *);

#endif
