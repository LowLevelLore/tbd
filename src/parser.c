#include "headers/parser.h"
#include "environment.c"
#include "headers/environment.h"
#include "headers/lexer.h"
#include "lexer.c"
#include "utils/errors.h"

ParsingContext *parse_context_create(void) {
    ParsingContext *ctx;

    ctx = calloc(1, sizeof(ParsingContext));
    assert(ctx && "Could not allocate memory for parsing context.");
    ctx->types = environment_create(NULL);
    if (environment_set(ctx->types, node_symbol("integer"), node_integer(0)) ==
        0) {
        printf("ERROR: Failed to set builtin type in types environment.\n");
    }
    ctx->variables = environment_create(NULL);
    return (ctx);
}

bool parse_int(Token *token, Node *node) {
    char *end;

    if (!token || !node) {
        return (false);
    } else {
        end = NULL;
        if ((token->end - token->beginning) == 1 &&
            *(token->beginning) == '0') {
            node->type = NODE_TYPE_INTEGER;
            node->value.tbd_integer = 0;
        } else if ((node->value.tbd_integer =
                        strtoll(token->beginning, &end, 10)) != 0) {
            if (end != token->end) {
                return (false);
            }
            node->type = NODE_TYPE_INTEGER;
        } else {
            return (false);
        }
    }
    return (true);
}

void print_token(Token *token) {
    printf("%.*s", (int)(token->end - token->beginning), token->beginning);
}

Error parse_expr(ParsingContext *context, char *source, char **end,
                 Node *result) {
    Token current_token;
    current_token.beginning = source;
    current_token.end = source;
    Error err = OK;

    while ((err = lex(current_token.end, &current_token)).type == ERROR_NULL) {
        *end = current_token.end;
        size_t token_length = current_token.end - current_token.beginning;
        if (token_length == 0) {
            break;
        }
        if (parse_int(&current_token, result)) {
            err = lex(current_token.end, &current_token);
            *end = current_token.end;
            if (err.type != ERROR_NULL) {
                return err;
            }

        } else {
            Node *symbol =
                node_symbol_from_buffer(current_token.beginning, token_length);

            err = lex(current_token.end, &current_token);
            *end = current_token.end;
            if (err.type != ERROR_NULL) {
                return err;
            }
            size_t token_length = current_token.end - current_token.beginning;
            if (token_length == 0) {
                break;
            }

            if (token_equals_string(&current_token, ":")) {

                err = lex(current_token.end, &current_token);
                *end = current_token.end;
                if (err.type != ERROR_NULL) {
                    return err;
                }
                size_t token_length =
                    current_token.end - current_token.beginning;
                if (token_length == 0) {
                    break;
                }
                Node *variable_binding = node_allocate();
                if (environment_get(*context->variables, symbol,
                                    variable_binding)) {

                    if (token_equals_string(&current_token, "=")) {
                        err = lex(current_token.end, &current_token);
                        *end = current_token.end;
                        if (err.type != ERROR_NULL) {
                            return err;
                        }
                        size_t token_length =
                            current_token.end - current_token.beginning;
                        if (token_length == 0) {
                            break;
                        }

                        Node *assigned_expr = node_allocate();
                        err = parse_expr(context, current_token.end,
                                         &current_token.end, assigned_expr);
                        if (err.type != ERROR_NULL) {
                            return err;
                        }
                        if (assigned_expr->type !=
                            variable_binding->children->type) {
                            ERROR_PREP(err, ERROR_TYPE,
                                       "Variable assignment expression has "
                                       "mismatched type.");
                            return err;
                        }

                        variable_binding->children->value =
                            assigned_expr->value;

                        free(assigned_expr);
                        return OK;
                    }
                    printf("ID of redefined variable: \"%s\"\n",
                           symbol->value.symbol);
                    ERROR_PREP(err, ERROR_GENERIC, "Redefinition of variable!");
                    return err;
                }

                Node *expected_type_symbol = node_symbol_from_buffer(
                    current_token.beginning, token_length);
                if (environment_get(*context->types, expected_type_symbol,
                                    result) == 0) {
                    ERROR_PREP(err, ERROR_TYPE,
                               "Invalid type within variable declaration");
                    printf("\nINVALID TYPE: \"%s\"\n",
                           expected_type_symbol->value.symbol);
                    return err;
                } else {

                    Node *var_decl = node_allocate();
                    var_decl->type = NODE_TYPE_VARIABLE_DECLARATION;

                    Node *type_node = node_allocate();
                    type_node->type = result->type;

                    node_add_child(var_decl, type_node);
                    node_add_child(var_decl, symbol);
                    err = lex(current_token.end, &current_token);
                    *end = current_token.end;
                    if (err.type != ERROR_NULL) {
                        return err;
                    }
                    size_t token_length =
                        current_token.end - current_token.beginning;
                    if (token_length == 0) {
                        break;
                    }

                    if (token_equals_string(&current_token, "=")) {
                        err = lex(current_token.end, &current_token);
                        *end = current_token.end;
                        if (err.type != ERROR_NULL) {
                            return err;
                        }
                        size_t token_length =
                            current_token.end - current_token.beginning;
                        if (token_length == 0) {
                            break;
                        }
                        Node *assigned_expr = node_allocate();
                        err = parse_expr(context, current_token.end,
                                         &current_token.end, assigned_expr);
                        if (err.type != ERROR_NULL) {
                            return err;
                        }
                        if (assigned_expr->type != type_node->type) {

                            print_node(assigned_expr, 0);
                            print_node(type_node, 0);

                            ERROR_PREP(err, ERROR_TYPE,
                                       "Variable assignment expression has "
                                       "mismatched type.");
                            return err;
                        }

                        type_node->value = assigned_expr->value;
                        free(assigned_expr);
                    }

                    *result = *var_decl;
                    free(var_decl);

                    return OK;
                }
            }

            printf("Unrecognized token: ");
            print_token(&current_token);
            putchar('\n');

            ERROR_PREP(err, ERROR_SYNTAX,
                       "Unrecognized token reached during parsing");
            return err;
        }

        printf("Intermediate node: ");
        print_node(result, 0);
        putchar('\n');
    }

    return err;
}