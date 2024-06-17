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
    Error err = node_add_type(ctx->types, NODE_TYPE_INTEGER,
                              node_symbol("inteeger"), sizeof(long long));
    if ((err.type != ERROR_NULL) ||
        (environment_set(ctx->types, node_symbol("integer"), node_integer(0)) ==
         0)) {
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

void print_token(Token t) {
    if (t.end - t.beginning < 1) {
        printf("INVALID TOKEN POINTERS");
    } else {
        printf("%.*s", (int)(t.end - t.beginning), t.beginning);
    }
}

Error parse_expr(ParsingContext *context, char *source, char **end,
                 Node *result) {
    ExpectReturnValue expected;
    size_t token_length = 0;
    Token current_token;
    current_token.beginning = source;
    current_token.end = source;
    Error err = OK;

    Node *working_result = result;

    while ((err = lex_advance(&current_token, &token_length, end)).type ==
           ERROR_NULL) {
        // printf("lexed: "); print_token(current_token); putchar('\n');
        if (token_length == 0) {
            return OK;
        }

        if (parse_int(&current_token, working_result)) {

            // TODO: Look ahead for binary ops that include integers.
            // It would be cool to use an operator environment to look up
            // operators instead of hard-coding them. This would eventually
            // allow for user-defined operators, or stuff like that!

            return OK;
        }

        // TODO: Parse strings and other literal types.

        // TODO: Check for unary prefix operators.

        // TODO: Check that it isn't a binary operator (we should encounter left
        // side first and peek forward, rather than encounter it at top level).

        Node *symbol =
            node_symbol_from_buffer(current_token.beginning, token_length);

        // TODO: Check if valid symbol for variable environment, then
        // attempt to pattern match variable access, assignment,
        // declaration, or declaration with initialization.

        EXPECT(expected, ":", current_token, token_length, end);
        if (expected.found) {

            // Re-assignment of existing variable (look for =)
            EXPECT(expected, "=", current_token, token_length, end);
            if (expected.found) {

                Node *variable_binding = node_allocate();
                if (!environment_get(*context->variables, symbol,
                                     variable_binding)) {
                    // TODO: Add source location or something to the error.
                    // TODO: Create new error type.
                    printf("ID of undeclared variable: \"%s\"\n",
                           symbol->value.symbol);
                    ERROR_PREP(err, ERROR_GENERIC,
                               "Reassignment of a variable that has not been "
                               "declared!");
                    return err;
                }

                // At this point, we have a guaranteed valid reassignment
                // expression, unless errors occur when parsing the actual value
                // expression.

                Node *var_reassign = node_allocate();
                var_reassign->type = NODE_TYPE_VARIABLE_REASSIGNMENT;

                Node *reassign_expr = node_allocate();

                node_add_child(var_reassign, symbol);
                node_add_child(var_reassign, reassign_expr);

                *working_result = *var_reassign;
                free(var_reassign);

                working_result = reassign_expr;
                continue;
            }

            err = lex_advance(&current_token, &token_length, end);
            if (err.type != ERROR_NULL) {
                return err;
            }
            if (token_length == 0) {
                break;
            }
            Node *type_symbol =
                node_symbol_from_buffer(current_token.beginning, token_length);
            if (environment_get(*context->types, type_symbol, working_result) ==
                0) {
                ERROR_PREP(err, ERROR_TYPE,
                           "Invalid type within variable declaration");
                printf("\nINVALID TYPE: \"%s\"\n", type_symbol->value.symbol);
                return err;
            }

            Node *variable_binding = node_allocate();
            if (environment_get(*context->variables, symbol,
                                variable_binding)) {
                // TODO: Create new error type.
                printf("ID of redefined variable: \"%s\"\n",
                       symbol->value.symbol);
                ERROR_PREP(err, ERROR_GENERIC, "Redefinition of variable!");
                return err;
            }
            // Variable binding is shell-node for environment value contents.
            free(variable_binding);

            working_result->type = NODE_TYPE_VARIABLE_DECLARATION;

            Node *value_expression = node_none();

            // `symbol` is now owned by working_result, a var. decl.
            node_add_child(working_result, symbol);
            node_add_child(working_result, value_expression);

            // Context variables environment gains new binding.
            Node *symbol_for_env = node_allocate();
            node_copy(symbol, symbol_for_env);
            int status = environment_set(context->variables, symbol_for_env,
                                         type_symbol);
            if (status != 1) {
                printf("Variable: \"%s\", status: %d\n",
                       symbol_for_env->value.symbol, status);
                ERROR_PREP(err, ERROR_GENERIC, "Failed to define variable!");
                return err;
            }

            EXPECT(expected, "=", current_token, token_length, end);
            if (expected.found) {
                working_result = value_expression;
                continue;
            }

            return OK;
        }

        printf("Unrecognized token: ");
        print_token(current_token);
        putchar('\n');

        ERROR_PREP(err, ERROR_SYNTAX,
                   "Unrecognized token reached during parsing");
        return err;
    }

    return err;
}