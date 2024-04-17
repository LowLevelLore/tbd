#include "headers/environment.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

Environment *environment_create(Environment *parent) {
  Environment *env = (Environment *)malloc(sizeof(Environment));
  assert(env && "Unable to allocate memory for environment.");
  env->parent = parent;
  env->binding = NULL;
  return env;
}

void environment_set(Environment *env, Node *id, Node *value) {
  Binding *binding = (Binding *)malloc(sizeof(Binding));
  assert(binding && "Could not allocte memory for new binding.");
  binding->value = value;
  binding->next = env->binding;
  binding->id = id;
  env->binding = binding;
}

bool node_compare(Node *a, Node *b) {
  if (!a || !b) {
    if (!a && !b) {
      return true;
    } else {
      return false;
    }
  } else {
    if (a->type != b->type) {
      return false;
    } else {
      switch (a->type) {
      case NODE_TYPE_NULL: {
        return true;
        break;
      }
      case NODE_TYPE_FLOAT: {
        printf("TODO : Comparing floats\n");
        break;
      }
      case NODE_TYPE_STRING: {
        printf("TODO : Comparing strings\n");
        break;
      }
      case NODE_TYPE_INTEGER: {
        return a->value.tbd_integer == b->value.tbd_integer;
        break;
      }
      case NODE_TYPE_PROGRAM: {
        printf("TODO : Comparing programs\n");
        break;
      }
      }
    }
  }
  return false;
}

Node *environment_get(Environment *env, Node *id) {
  Node value;
  Binding *binding_it = env->binding;
  while (binding_it) {
    if (node_compare(binding_it->id, id)) {
      return binding_it->value;
    }
    binding_it = binding_it->next;
  }
  return NULL;
}
