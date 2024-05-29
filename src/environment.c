#include "headers/environment.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

Environment *environment_create(Environment *parent)
{
  Environment *env = (Environment *)malloc(sizeof(Environment));
  assert(env && "Unable to allocate memory for environment.");
  env->parent = parent;
  env->binding = NULL;
  return env;
}

int environment_set(Environment *env, Node *id, Node *value)
{
  if (!env || !id || !value)
  {
    return 0;
  }
  Binding *binding_it = env->binding;
  while (binding_it)
  {
    if (node_compare(binding_it->id, id))
    {
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

bool environment_get(Environment env, Node *id, Node *result)
{
  Binding *binding_it = env.binding;
  while (binding_it)
  {
    if (node_compare(binding_it->id, id))
    {
      *result = *binding_it->value;
      return true;
    }
    binding_it = binding_it->next;
  }
  return false;
}

Node *node_allocate()
{
  Node *node = calloc(1, sizeof(Node));
  assert(node && "Could not allocate memory for AST node");
  return node;
}

void node_add_child(Node *parent, Node *new_child)
{
  if (!parent || !new_child)
  {
    return;
  }
  if (parent->children)
  {
    Node *child = parent->children;
    while (child->next_child)
    {
      child = child->next_child;
    }
    child->next_child = new_child;
  }
  else
  {
    parent->children = new_child;
  }
}

void free_nodes(Node *node)
{
  if (!node)
  {
    return;
  }
  else
  {
    Node *child = node->children;
    Node *next_child = NULL;
    while (child)
    {
      next_child = child->next_child;
      free_nodes(child);
      child = next_child;
    }
    free(node);
  }
}

void print_node(Node *node, size_t indent_level)
{
  if (!node)
  {
    return;
  }
  for (size_t i = 0; i < indent_level; ++i)
  {
    putchar(' ');
  }
  switch (node->type)
  {
  default:
    printf("UNKNOWN");
    break;
  case NODE_TYPE_NULL:
    printf("NONE");
    break;
  case NODE_TYPE_INTEGER:
    printf("INT:%lld", node->value.tbd_integer);
    break;
  case NODE_TYPE_SYMBOL:
    printf("SYM");
    if (node->value.symbol)
    {
      printf(":%s", node->value.symbol);
    }
    break;
  case NODE_TYPE_BINARY_OPERATOR:
    printf("BINARY OPERATOR");
    break;
  case NODE_TYPE_VARIABLE_DECLARATION:
    printf("VARIABLE DECLARATION");
    break;
  case NODE_TYPE_VARIABLE_DECLARATION_WITH_INITIALIZATION:
    printf("VARIABLE DECLARATION INITIALIZED");
    break;
  case NODE_TYPE_PROGRAM:
    printf("PROGRAM");
    break;
  }
  putchar('\n');
  Node *child = node->children;
  while (child)
  {
    print_node(child, indent_level + 4);
    child = child->next_child;
  }
}

int node_compare(Node *a, Node *b)
{
  if (!a || !b)
  {
    if (!a && !b)
    {
      return 1;
    }
    return 0;
  }

  if (a->type != b->type)
  {
    return 0;
  }
  switch (a->type)
  {
  case NODE_TYPE_NULL:
    if (nullp(*b))
    {
      return 1;
    }
    return 0;
    break;
  case NODE_TYPE_INTEGER:
    if (a->value.tbd_integer == b->value.tbd_integer)
    {
      return 1;
    }
    return 0;
    break;
  case NODE_TYPE_FLOAT:
    return a->value.tbd_double == b->value.tbd_double;
    break;
  case NODE_TYPE_STRING:
    printf("TODO: Handle NODE_TYPE_STRING comparison");
    break;
  case NODE_TYPE_SYMBOL:
    if (a->value.symbol && b->value.symbol)
    {
      if (strcmp(a->value.symbol, b->value.symbol) == 0)
      {
        return 1;
      }
      return 0;
    }
    else if (!a->value.symbol && !b->value.symbol)
    {
      return 1;
    }
    return 0;
    break;
  case NODE_TYPE_BINARY_OPERATOR:
    printf("TODO: node_compare() BINARY OPERATOR\n");
    break;
  case NODE_TYPE_VARIABLE_DECLARATION:
    printf("TODO: node_compare() VARIABLE DECLARATION\n");
    break;
  case NODE_TYPE_VARIABLE_DECLARATION_WITH_INITIALIZATION:
    printf("TODO: node_compare() VARIABLE DECLARATION INITIALIZED\n");
    break;
  case NODE_TYPE_PROGRAM:
    printf("TODO: Compare two programs.\n");
    break;
  }
  return 0;
}

Node *node_symbol_from_buffer(char *buffer, size_t length)
{
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

Node *node_integer(long long value)
{
  Node *integer = node_allocate();
  integer->type = NODE_TYPE_INTEGER;
  integer->value.tbd_integer = value;
  integer->children = NULL;
  integer->next_child = NULL;
  return integer;
}

Node *node_float(double value)
{
  Node *integer = node_allocate();
  integer->type = NODE_TYPE_FLOAT;
  integer->value.tbd_double = value;
  integer->children = NULL;
  integer->next_child = NULL;
  return integer;
}

Node *node_symbol(char *symbol_string)
{
  Node *symbol = node_allocate();
  symbol->type = NODE_TYPE_SYMBOL;
  symbol->value.symbol = strdup(symbol_string);
  return symbol;
}
