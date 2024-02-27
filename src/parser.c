#include "headers/parser.h"

#include "environment.c"
#include "headers/environment.h"
#include "headers/lexer.h"
#include "lexer.c"
#include "utils/errors.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_tokens(Token *root) {
  while (root) {
    Token *current = root;
    root = root->next;
    free(current);
  }
}

void print_tokens_ll(Token *root) {
  size_t count = 1;
  while (root) {
    if (count > 1000) {
      break;
    }
    printf("Token %zu : ", count);
    if (root->beginning && root->end) {
      printf("%.*s\n", (int)(root->end - root->beginning), root->beginning);
    }
    root = root->next;
    count++;
  }
}

bool token_equals_string(Token *token, char *string) {
  char *beg = token->beginning;
  if (!string || !token) {
    return false;
  }
  while (*string && beg < token->end) {
    if (*beg != *string) {
      return false;
    }
    beg++;
    string++;
  }
  return true;
}

void print_node_internal(Node *root) {
  if (!root) {
    return;
  } else {
    switch (root->type) {
    case NODE_TYPE_NULL: {
      printf("NULL NODE");
      break;
    }
    case NODE_TYPE_INTEGER: {
      printf("INT : %lld", root->value.tbd_integer);
      break;
    }
    case NODE_TYPE_FLOAT: {
      break;
    }
    case NODE_TYPE_STRING: {
      break;
    }
    case NODE_TYPE_PROGRAM: {
      break;
    }
    default: {
      printf("INVALID NODE TYPE");
    }
    }
  }
}

void print_node(Node *root, int indent_level) {
  if (!root) {
    return;
  } else {
    for (int i = 0; i < indent_level; i++) {
      putchar(' ');
    }
    print_node_internal(root);
    printf("\n");
    Node *child = root->children;
    while (child) {
      for (int i = 0; i < indent_level; i++) {
        putchar(' ');
      }
      print_node(root, indent_level + 4);
      child = child->next_child;
    }
  }
}

bool parse_int(Token *token, Node *node) {
  if (!token || !node) {
    return false;
  } else {
    if ((token->end - token->beginning) == 1 && *(token->beginning) == '0') {
      node->type = NODE_TYPE_INTEGER;
      node->value.tbd_integer = 0;
      return true;
    } else if ((node->value.tbd_integer =
                    strtoll(token->beginning, NULL, 10)) != 0) {
      node->type = NODE_TYPE_INTEGER;
      return true;
    } else {
      return false;
    }
  }
  return false;
}

Error parse(char *contents, Node result) {
  Error err = OK;

  Token current_token;
  current_token.next = NULL;
  current_token.beginning = contents;
  current_token.end = contents;

  Node *root = calloc(1, sizeof(Node));
  if (!root) {
    err.type = ERROR_GENERIC;
    err.message = "Cannot allocate memory for ROOT Node of AST";
    return err;
  }
  root->type = NODE_TYPE_PROGRAM;

  Node working_node;
  working_node.type = NODE_TYPE_NULL;
  working_node.next_child = NULL;
  working_node.children = NULL;
  working_node.value.tbd_integer = 0;

  while ((err = lex(current_token.end, &current_token)).type == ERROR_NULL) {
    int token_size = current_token.end - current_token.beginning;
    if (token_size == 0) {
      break;
    }
    if (parse_int(&current_token, &working_node)) {
      printf("Found Integer : ");
      print_node(&working_node, 0);
      putchar('\n');
    }
  }

  return err;
}
