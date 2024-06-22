#include "headers/parser.h"
#include "environment.c"
#include "headers/environment.h"
#include "headers/lexer.h"
#include "lexer.c"
#include "utils/errors.h"

void parsing_context_print(ParsingContext *top, size_t indent_level) {
    // TODO: Print current context
    if (!top)
        return;
    printf("\n");
    int temp = indent_level;
    while (temp--) {
        putchar(' ');
    }
    temp = indent_level;

    if (top->parent == NULL) {
        printf("%s[BINARY_OPERATORS]:\n%s", BBLU, COLOR_RESET);
        print_environment(top->binary_operators, indent_level + 2);
        while (temp--) {
            putchar(' ');
        }
    }

    printf("%s[TYPES]:\n%s", BBLU, COLOR_RESET);
    temp = indent_level;
    print_environment(top->types, indent_level + 2);
    while (temp--) {
        putchar(' ');
    }
    printf("%s[VARIABLES]:\n%s", BBLU, COLOR_RESET);
    temp = indent_level;
    print_environment(top->variables, indent_level + 2);
    while (temp--) {
        putchar(' ');
    }
    printf("%s[FUNCTIONS]:\n%s", BBLU, COLOR_RESET);
    temp = indent_level;
    print_environment(top->functions, indent_level + 2);
    ParsingContext *itr = top->children;
    while (itr) {
        parsing_context_print(itr, indent_level + 4);
        itr = itr->next_child;
    }
}

void parsing_context_add_child(ParsingContext *parent, ParsingContext *child) {
    if (!parent) {
        return;
    }
    if (parent->children) {
        parent = parent->children;
        while (parent->next_child) {
            parent = parent->next_child;
        }
        parent->next_child = child;
    } else {
        parent->children = child;
    }
}

ParsingContext *parse_context_default_create() {
    ParsingContext *ctx = parse_context_create(NULL);
    assert(ctx && "Could not allocate memory for parsing context.");
    Error err = define_type(ctx->types, NODE_TYPE_INTEGER,
                            node_symbol("integer"), sizeof(long long));
    err = define_type(ctx->types, NODE_TYPE_FUNCTION_DECLARATION,
                      node_symbol("fn"), sizeof(long long));
    define_binary_operator(ctx, "+", 5, "integer", "integer", "integer");
    define_binary_operator(ctx, "-", 5, "integer", "integer", "integer");
    define_binary_operator(ctx, "*", 10, "integer", "integer", "integer");
    define_binary_operator(ctx, "/", 10, "integer", "integer", "integer");
    if ((err.type != ERROR_NULL)) {
        printf("ERROR: Failed to set builtin type in types environment.\n");
    }
    ctx->variables = environment_create(NULL);
    return (ctx);
}

ParsingContext *parse_context_create(ParsingContext *parent) {
    ParsingContext *ctx = calloc(1, sizeof(ParsingContext));
    assert(ctx && "Could not allocate memory for parsing context.");
    parsing_context_add_child(parent, ctx);
    ctx->parent = parent;
    ctx->operator= NULL;
    ctx->result = NULL;
    ctx->types = environment_create(NULL);
    ctx->variables = environment_create(NULL);
    ctx->functions = environment_create(NULL);
    ctx->binary_operators = environment_create(NULL);
    ctx->children = NULL;
    ctx->next_child = NULL;
    return ctx;
}

Error parse_get_type(ParsingContext *context, Node *id, Node *result) {
    Error err;
    while (context) {
        if (environment_get(*context->types, id, result)) {
            return OK;
        }
        context = context->parent;
    }
    result->type = NODE_TYPE_NULL;
    ERROR_PREP(err, ERROR_GENERIC,
               "Type not found in context or any of its parent.");
    return err;
}

bool parse_integer(Token *token, Node *node) {
    char *end;
    if (!token || !node) {
        return (false);
    } else {
        end = NULL;
        if ((token->end - token->beginning) == 1 &&
            *(token->beginning) == '0') {
            node->type = NODE_TYPE_INTEGER;
            node->value.MZ_integer = 0;
        } else if ((node->value.MZ_integer =
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
    long long working_precedence = 0;

    while ((err = lex_advance(&current_token, &token_length, end)).type ==
           ERROR_NULL) {
        // printf("lexed: "); print_token(current_token); putchar('\n');
        if (token_length == 0) {
            return OK;
        }

        if (parse_integer(&current_token, working_result)) {

            // TODO: Look ahead for binary ops that include integers.
            // It would be cool to use an operator environment to look up
            // operators instead of hard-coding them. This would eventually
            // allow for user-defined operators, or stuff like that!

        } else {

            Node *symbol =
                node_symbol_from_buffer(current_token.beginning, token_length);

#ifdef DEBUG_COMPILER
            if (strcmp(symbol->value.symbol, "pint") == 0) {
                working_result->type = NODE_TYPE_DEBUG_PRINT_INTEGER;
                lex_advance(&current_token, &token_length, end);
                Node *temp_symbol = node_allocate();
                temp_symbol = node_symbol_from_buffer(current_token.beginning,
                                                      token_length);
                node_add_child(working_result, temp_symbol);
                return OK;
            }

#endif // DEBUG

            if (strcmp("[", symbol->value.symbol) == 0) {
                // Return Type
                // Param List
                working_result->type = NODE_TYPE_FUNCTION_DECLARATION;
                lex_advance(&current_token, &token_length, end);
                Node *function_return_type = node_symbol_from_buffer(
                    current_token.beginning, token_length);
                Node *function_return = node_allocate();
                function_return->type = NODE_TYPE_FUNCTION_RETURN_TYPE;
                node_add_child(function_return, function_return_type);

                EXPECT(expected, "(", current_token, token_length, end);
                if (!expected.found || expected.done) {
                    ERROR_PREP(err, ERROR_SYNTAX,
                               "Expected opening parenthesis for parameter "
                               "list after function name");
                    return err;
                }

                Node *parameter_list = node_allocate();
                parameter_list->type = NODE_TYPE_FUNCTION_PARAMS_LIST;
                node_add_child(working_result, parameter_list);
                node_add_child(working_result, function_return);
                // FIXME?: Should we possibly create a parser stack and
                // evaluate the next expression, then ensure return value is
                // var. decl. in stack handling below?

                // Node *narg = node_allocate();
                // narg->type = NODE_TYPE_INITIAL_ARG;
                // node_add_child(parameter_list, narg);

                for (;;) {
                    EXPECT(expected, ")", current_token, token_length, end);
                    if (expected.found) {
                        break;
                    }
                    if (expected.done) {
                        ERROR_PREP(err, ERROR_SYNTAX,
                                   "Expected closing parenthesis for "
                                   "parameter list");
                        return err;
                    }

                    err = lex_advance(&current_token, &token_length, end);
                    if (err.type) {
                        return err;
                    }
                    Node *parameter_name = node_symbol_from_buffer(
                        current_token.beginning, token_length);

                    EXPECT(expected, ":", current_token, token_length, end);
                    if (expected.done || !expected.found) {
                        ERROR_PREP(err, ERROR_SYNTAX,
                                   "Parameter declaration requires a type "
                                   "annotation");
                        return err;
                    }

                    lex_advance(&current_token, &token_length, end);
                    Node *parameter_type = node_symbol_from_buffer(
                        current_token.beginning, token_length);

                    Node *parameter = node_allocate();
                    parameter->type = NODE_TYPE_FUNCTION_PARAM;
                    node_add_child(parameter, parameter_name);
                    node_add_child(parameter, parameter_type);

                    node_add_child(parameter_list, parameter);

                    EXPECT(expected, ",", current_token, token_length, end);
                    if (expected.found) {
                        continue;
                    }

                    EXPECT(expected, ")", current_token, token_length, end);
                    if (!expected.found) {
                        ERROR_PREP(err, ERROR_SYNTAX,
                                   "Expected closing parenthesis following "
                                   "parameter list");
                        return err;
                    }
                    break;
                }

                if (!parameter_list->children) {
                    Node *none_param = node_allocate();
                    node_add_child(parameter_list, none_param);
                }
                // Body
                EXPECT(expected, "{", current_token, token_length, end);
                if (expected.done || !expected.found) {
                    ERROR_PREP(err, ERROR_SYNTAX,
                               "Function definition requires body following "
                               "return type: \"{ a + b }\"");
                    return err;
                }
                context = parse_context_create(context);
                context->operator= node_symbol("lambda");

                Node *param_it = working_result->children->children;
                if (param_it && param_it->children) {
                    environment_set(context->variables, param_it->children,
                                    param_it->children->next_child);
                }

                Node *function_body = node_allocate();
                function_body->type = NODE_TYPE_PROGRAM;
                Node *function_first_expression = node_allocate();
                node_add_child(function_body, function_first_expression);

                node_add_child(working_result, function_body);

                working_result = function_first_expression;
                context->result = working_result;
                continue;
            }

            // TODO: Parse strings and other literal types.

            // TODO: Check for unary prefix operators.

            // TODO: Check that it isn't a binary operator (we should
            // encounter left side first and peek forward, rather than
            // encounter it at top level).

            if (strcmp("fn", symbol->value.symbol) == 0) {
                // Begin function definition.
                // FUNCTION
                //   LIST of parameters
                //     PARAMETER
                //       SYMBOL:NAME
                //       SYMBOL:TYPE
                //   RETURN TYPE SYMBOL
                //   PROGRAM/LIST of expressions
                //     ...

                working_result->type = NODE_TYPE_FUNCTION_DECLARATION;

                lex_advance(&current_token, &token_length, end);
                Node *function_name = node_symbol_from_buffer(
                    current_token.beginning, token_length);

                EXPECT(expected, "(", current_token, token_length, end);
                if (!expected.found) {
                    printf("Function Name: \"%s\"\n",
                           function_name->value.symbol);
                    ERROR_PREP(err, ERROR_SYNTAX,
                               "Expected opening parenthesis for parameter "
                               "list after function name");
                    return err;
                }

                Node *parameter_list = node_allocate();
                parameter_list->type = NODE_TYPE_FUNCTION_PARAMS_LIST;
                node_add_child(working_result, parameter_list);
                // FIXME?: Should we possibly create a parser stack and
                // evaluate the next expression, then ensure return value is
                // var. decl. in stack handling below?

                // Node *narg = node_allocate();
                // narg->type = NODE_TYPE_INITIAL_ARG;
                // node_add_child(parameter_list, narg);

                for (;;) {
                    EXPECT(expected, ")", current_token, token_length, end);
                    if (expected.found) {
                        break;
                    }
                    if (expected.done) {
                        ERROR_PREP(err, ERROR_SYNTAX,
                                   "Expected closing parenthesis for "
                                   "parameter list");
                        return err;
                    }

                    err = lex_advance(&current_token, &token_length, end);
                    if (err.type) {
                        return err;
                    }
                    Node *parameter_name = node_symbol_from_buffer(
                        current_token.beginning, token_length);

                    EXPECT(expected, ":", current_token, token_length, end);
                    if (expected.done || !expected.found) {
                        ERROR_PREP(err, ERROR_SYNTAX,
                                   "Parameter declaration requires a type "
                                   "annotation");
                        return err;
                    }

                    lex_advance(&current_token, &token_length, end);
                    Node *parameter_type = node_symbol_from_buffer(
                        current_token.beginning, token_length);

                    Node *parameter = node_allocate();
                    parameter->type = NODE_TYPE_FUNCTION_PARAM;
                    node_add_child(parameter, parameter_name);
                    node_add_child(parameter, parameter_type);

                    node_add_child(parameter_list, parameter);

                    EXPECT(expected, ",", current_token, token_length, end);
                    if (expected.found) {
                        continue;
                    }

                    EXPECT(expected, ")", current_token, token_length, end);
                    if (!expected.found) {
                        ERROR_PREP(err, ERROR_SYNTAX,
                                   "Expected closing parenthesis following "
                                   "parameter list");
                        return err;
                    }
                    break;
                }

                if (!parameter_list->children) {
                    Node *none_param = node_allocate();
                    node_add_child(parameter_list, none_param);
                }

                // Parse return type.
                EXPECT(expected, ":", current_token, token_length, end);
                // TODO/FIXME: Should we allow implicit return type?
                if (expected.done || !expected.found) {
                    ERROR_PREP(err, ERROR_SYNTAX,
                               "Function definition requires return type "
                               "annotation following parameter list");
                    return err;
                }

                lex_advance(&current_token, &token_length, end);
                Node *function_return_type = node_symbol_from_buffer(
                    current_token.beginning, token_length);
                Node *function_return = node_allocate();
                function_return->type = NODE_TYPE_FUNCTION_RETURN_TYPE;
                node_add_child(function_return, function_return_type);
                node_add_child(working_result, function_return);

                // Bind function to function name in functions environment.
                environment_set(context->functions, function_name,
                                working_result);

                // Parse function body.
                EXPECT(expected, "{", current_token, token_length, end);
                if (expected.done || !expected.found) {
                    ERROR_PREP(err, ERROR_SYNTAX,
                               "Function definition requires body following "
                               "return type: \"{ a + b }\"");
                    return err;
                }
                context = parse_context_create(context);
                context->operator= node_symbol("fn");

                Node *param_it = working_result->children->children;
                if (param_it && param_it->children) {
                    environment_set(context->variables, param_it->children,
                                    param_it->children->next_child);
                }

                Node *function_body = node_allocate();
                function_body->type = NODE_TYPE_PROGRAM;
                Node *function_first_expression = node_allocate();
                node_add_child(function_body, function_first_expression);
                node_add_child(working_result, function_body);
                working_result = function_first_expression;
                context->result = working_result;
                continue;
            }

            // TODO: Check if valid symbol for variable environment,
            // then attempt to pattern match variable access,
            // assignment, declaration, or declaration with
            // initialization.

            EXPECT(expected, ":", current_token, token_length, end);
            if (expected.found) {

                // Re-assignment of existing variable (look for =)
                EXPECT(expected, "=", current_token, token_length, end);
                if (expected.found) {
                    Node *variable_binding = node_allocate();
                    if (!environment_get(*context->variables, symbol,
                                         variable_binding)) {
                        // TODO: Add source location or something to the
                        // error.
                        // TODO: Create new error type.
                        printf("ID of undeclared variable: \"%s\"\n",
                               symbol->value.symbol);
                        ERROR_PREP(err, ERROR_GENERIC,
                                   "Reassignment of a variable that has "
                                   "not been declared!");
                        return err;
                    }
                    free(variable_binding);

                    working_result->type = NODE_TYPE_VARIABLE_REASSIGNMENT;
                    // print_node(symbol, 0);
                    node_add_child(working_result, symbol);
                    Node *reassign_expr = node_allocate();
                    node_add_child(working_result, reassign_expr);

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
                Node *type_symbol = node_symbol_from_buffer(
                    current_token.beginning, token_length);
                Node *type_value = node_allocate();
                parse_get_type(context, type_symbol, type_value);
                if (type_value->type == NODE_TYPE_NULL) {
                    ERROR_PREP(err, ERROR_TYPE,
                               "Invalid type within variable declaration");
                    printf("\nINVALID TYPE: \"%s\"\n",
                           type_symbol->value.symbol);
                    return err;
                }
                free(type_value);

                Node *variable_binding = node_allocate();
                if (environment_get(*context->variables, symbol,
                                    variable_binding)) {
                    // TODO: Create new error type.
                    printf("ID of redefined variable: \"%s\"\n",
                           symbol->value.symbol);
                    ERROR_PREP(err, ERROR_GENERIC, "Redefinition of variable!");
                    return err;
                }
                // Variable binding is shell-node for environment value
                // contents.
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
                    ERROR_PREP(err, ERROR_GENERIC,
                               "Failed to define variable!");
                    return err;
                }

                EXPECT(expected, "=", current_token, token_length, end);
                if (expected.found) {
                    working_result = value_expression;
                    continue;
                }

                return OK;
            } else {
                EXPECT(expected, "(", current_token, token_length, end);
                if (expected.found) {
                    working_result->type = NODE_TYPE_FUNCTION_CALL;
                    node_add_child(working_result, symbol);

                    Node *arg_list = node_allocate();
                    arg_list->type = NODE_TYPE_FUNCTION_ARGS_LIST;
                    Node *arg = node_allocate();
                    node_add_child(arg_list, arg);
                    node_add_child(working_result, arg_list);
                    working_result = arg;

                    context = parse_context_create(context);
                    context->operator= node_symbol("fncall");
                    context->result = working_result;
                    continue;
                } else {
                    ParsingContext *copy = context;
                    bool found = false;
                    Node *variable = node_allocate();
                    while (copy->parent) {
                        if (environment_get(*copy->variables, symbol,
                                            variable)) {
                            found = true;
                            break;
                        }
                        copy = copy->parent;
                    }

                    if (environment_get(*copy->variables, symbol, variable)) {
                        found = true;
                    }

                    if (!found) {
                        ERROR_PREP(err, ERROR_SYNTAX, "Unknown token: %s",
                                   symbol->value.symbol);
                        ret;
                    }

                    working_result->type = NODE_TYPE_VARIABLE_ACCESS;
                    node_add_child(working_result, symbol);

                    // TODO: Check for variable access.
                }
            }

            // printf("Unrecognized token: ");
            // print_token(current_token);
            // putchar('\n');

            // ERROR_PREP(err, ERROR_SYNTAX,
            //            "Unrecognized token reached during parsing");
            // return err;
        }

        // Look ahead for a binary infix ? :^
        Token current_copy = current_token;
        size_t length_copy = token_length;
        char *end_copy = *end;
        err = lex_advance(&current_copy, &length_copy, &end_copy);
        ret;
        Node *operator_symbol =
            node_symbol_from_buffer(current_copy.beginning, length_copy);
        Node *operator_value = node_allocate();
        ParsingContext *global = context;
        while (global->parent) {
            global = global->parent;
        }
        if (environment_get(*global->binary_operators, operator_symbol,
                            operator_value)) {
            current_token = current_copy;
            token_length = length_copy;
            *end = end_copy;
            long long precedence = operator_value->children->value.MZ_integer;

            // printf("Got op. %s with precedence %lld (working %lld)\n",
            //        operator_symbol->value.symbol,
            //        precedence, working_precedence);
            // printf("working precedence: %lld\n", working_precedence);

            // TODO: Handle grouped expressions through parentheses using
            // precedence stack.

            Node *result_pointer =
                precedence <= working_precedence ? result : working_result;

            Node *result_copy = node_allocate();
            node_copy(result_pointer, result_copy);
            result_pointer->type = NODE_TYPE_BINARY_OPERATOR;
            result_pointer->value.symbol = operator_symbol->value.symbol;
            result_pointer->children = result_copy;
            result_pointer->next_child = NULL;

            Node *rhs = node_allocate();
            node_add_child(result_pointer, rhs);
            working_result = rhs;

            working_precedence = precedence;

            free(operator_symbol);
            free(operator_value);

            continue;
        }

        if (!context->parent) {
            break;
        }

        Node *operator= context->operator;
        if (operator->type != NODE_TYPE_SYMBOL) {
            ERROR_PREP(err, ERROR_TYPE,
                       "Parsing context operator must be symbol. Likely "
                       "internal error :(");
            return err;
        }
        if (strcmp(operator->value.symbol, "lambda") == 0) {
            // Evaluate next expression unless it's a closing brace.
            EXPECT(expected, "}", current_token, token_length, end);
            if (expected.done || expected.found) {
                EXPECT(expected, "]", current_token, token_length, end);
                if (expected.done || expected.found) {
                    if (!context->parent)
                        return OK;
                    context = context->parent;
                    if (!context->parent) {
                        context->result = result;
                    }
                } else {
                    ERROR_PREP(err, ERROR_SYNTAX,
                               "Expected ']' after lambda definition.");
                    ret;
                }
            }

            context->result->next_child = node_allocate();
            working_result = context->result->next_child;
            context->result = working_result;
            continue;
        }

        if (strcmp(operator->value.symbol, "fn") == 0) {
            // Evaluate next expression unless it's a closing brace.
            EXPECT(expected, "}", current_token, token_length, end);
            if (expected.done || expected.found) {
                if (!context->parent)
                    break;
                context = context->parent;
                if (!context->parent) {
                    context->result = result;
                }
            }

            context->result->next_child = node_allocate();
            working_result = context->result->next_child;
            context->result = working_result;
            continue;
        }
        if (strcmp(operator->value.symbol, "fncall") == 0) {
            EXPECT(expected, ")", current_token, token_length, end);
            if (expected.done || expected.found) {
                break;
            }
            EXPECT(expected, ",", current_token, token_length, end);
            if (expected.done || !expected.found) {
                ERROR_PREP(err, ERROR_SYNTAX,
                           "Parameter list expected closing parenthesis or "
                           "comma for another parameter");
                return err;
            }

            context->result->next_child = node_allocate();
            working_result = context->result->next_child;
            context->result = working_result;

            continue;
        }
    }

    return err;
}
