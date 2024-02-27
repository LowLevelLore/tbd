#include "headers/environment.h"
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
