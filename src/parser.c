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

ParsingContext *parse_context_create()
{
  ParsingContext *ctx = calloc(1, sizeof(ParsingContext));
  assert(ctx && "Could not allocate memory for parsing context.");
  ctx->types = environment_create(NULL);
  if (environment_set(ctx->types, node_symbol("integer"), node_integer(0)) == 0)
  {
    printf("ERROR: Failed to set builtin type in types environment.\n");
  }
  ctx->variables = environment_create(NULL);
  return ctx;
}

bool parse_int(Token *token, Node *node)
{
  if (!token || !node)
  {
    return false;
  }
  else
  {
    char *end = NULL;
    if ((token->end - token->beginning) == 1 && *(token->beginning) == '0')
    {
      node->type = NODE_TYPE_INTEGER;
      node->value.tbd_integer = 0;
    }
    else if ((node->value.tbd_integer =
                  strtoll(token->beginning, &end, 10)) != 0)
    {
      if (end != token->end)
      {
        return false;
      }
      node->type = NODE_TYPE_INTEGER;
    }
    else
    {
      return false;
    }
  }
  return true;
}

void print_token(Token *token)
{
  printf("%.*s", (int)(token->end - token->beginning), token->beginning);
}

Error parse_expr(ParsingContext *context,
                 char *source,
                 char **end,
                 Node *result)
{
  Token current_token;
  current_token.beginning = source;
  current_token.end = source;
  Error err = OK;

  while ((err = lex(current_token.end, &current_token)).type == ERROR_NULL)
  {
    *end = current_token.end;
    size_t token_length = current_token.end - current_token.beginning;
    if (token_length == 0)
    {
      break;
    }
    if (parse_int(&current_token, result))
    {
      err = lex(current_token.end, &current_token);
      if (err.type != ERROR_NULL)
      {
        return err;
      }
      *end = current_token.end;
    }
    else
    {
      Node *symbol = node_symbol_from_buffer(current_token.beginning, token_length);
      err = lex(current_token.end, &current_token);
      if (err.type != ERROR_NULL)
      {
        return err;
      }
      *end = current_token.end;
      size_t token_length = current_token.end - current_token.beginning;
      if (token_length == 0)
      {
        break;
      }

      if (token_equals_string(&current_token, ":"))
      {
        err = lex(current_token.end, &current_token);
        if (err.type != ERROR_NULL)
        {
          return err;
        }
        *end = current_token.end;
        size_t token_length = current_token.end - current_token.beginning;
        if (token_length == 0)
        {
          break;
        }

        Node *expected_type_symbol =
            node_symbol_from_buffer(current_token.beginning, token_length);
        bool status = environment_get(*context->types, expected_type_symbol, result);
        if (!status)
        {
          err.type = ERROR_TYPE;
          err.message = "Invalid type within variable declaration";
          printf("\nINVALID TYPE: \"%s\"\n", expected_type_symbol->value.symbol);
          return err;
        }
        else
        {
          Node *var_decl = node_allocate();
          var_decl->type = NODE_TYPE_VARIABLE_DECLARATION;
          Node *type_node = node_allocate();
          type_node->type = result->type;
          node_add_child(var_decl, type_node);
          node_add_child(var_decl, symbol);
          *result = *var_decl;
          free(var_decl);
          return OK;
        }
      }

      printf("Unrecognized token: ");
      print_token(&current_token);
      putchar('\n');

      return err;
    }

    printf("Intermediate node: ");
    print_node(result, 0);
    putchar('\n');
  }

  return err;
}
