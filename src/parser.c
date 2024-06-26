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

ParsingStack *parse_stack_create(ParsingStack *parent) {
    ParsingStack *stack = calloc(1, sizeof(ParsingStack));
    assert(stack && "Could'nt allocate memory for ParsingStack");
    stack->operator= NULL;
    stack->result = NULL;
    stack->parent = parent;
    return stack;
}

ParsingContext *parse_context_default_create() {
    ParsingContext *ctx = parse_context_create(NULL);
    assert(ctx && "Could not allocate memory for parsing context.");
    Error err = define_type(ctx->types, NODE_TYPE_INTEGER,
                            node_symbol("integer"), sizeof(long long));
    err = define_type(ctx->types, NODE_TYPE_FUNCTION_DECLARATION,
                      node_symbol("fn"), sizeof(long long));
    define_binary_operator(ctx, "=", 5, "integer", "integer", "integer");
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

Error parse_binary_infix_operator(ParsingContext *context, ParsingStack *stack,
                                  int *found, Token *current, size_t *length,
                                  char **end, long long *working_precedence,
                                  Node *result, Node **working_result) {
    Error err = OK;
    // Look ahead for a binary infix operator.
    *found = 0;
    Token current_copy = *current;
    size_t length_copy = *length;
    char *end_copy = *end;
    err = lex_advance(&current_copy, &length_copy, &end_copy);
    if (err.type != ERROR_NULL) {
        return err;
    }
    Node *operator_symbol =
        node_symbol_from_buffer(current_copy.beginning, length_copy);
    Node *operator_value = node_allocate();
    ParsingContext *global = context;
    while (global->parent) {
        global = global->parent;
    }
    if (environment_get(*global->binary_operators, operator_symbol,
                        operator_value)) {
        *current = current_copy;
        *length = length_copy;
        *end = end_copy;
        long long precedence = operator_value->children->value.MZ_integer;

        // printf("Got op. %s with precedence %lld (working %lld)\n",
        //        operator_symbol->value.symbol,
        //        precedence, working_precedence);
        // printf("working precedence: %lld\n", working_precedence);

        // TODO: Handle grouped expressions through parentheses using precedence
        // stack.

        Node *result_pointer =
            precedence <= *working_precedence ? result : *working_result;
        if (precedence <= *working_precedence) {
            if (stack) {
                result_pointer = stack->result;
            } else {
                result_pointer = result;
            }
        } else {
            result_pointer = *working_result;
        }

        Node *result_copy = node_allocate();
        node_copy(result_pointer, result_copy);
        result_pointer->type = NODE_TYPE_BINARY_OPERATOR;
        result_pointer->value.symbol = operator_symbol->value.symbol;
        result_pointer->children = result_copy;
        result_pointer->next_child = NULL;

        Node *rhs = node_allocate();
        node_add_child(result_pointer, rhs);
        *working_result = rhs;

        *working_precedence = precedence;

        *found = 1;
    }

    free(operator_symbol);
    free(operator_value);
    return OK;
}

enum StackOperatorReturnValue {
    STACK_HANDLED_INVALID = 0,
    STACK_HANDLED_BREAK = 1,
    STACK_HANDLED_PARSE = 2,
    STACK_HANDLED_CHECK = 3
};

/**
 * @retval 1 Break (stack was NULL most likely)
 * @retval 2 Continue Parsing (working_result was updated, possibly stack as
 * well)
 * @retval 3 Continue Checking (stack was updated, may need to handle it as
 * well)
 */
Error handle_stack_operator(int *status, ParsingContext **context,
                            ParsingStack **stack, Node **working_result,
                            Node *result, long long *working_precedence,
                            Token *current, size_t *length, char **end) {
    if (!(*stack)) {
        *status = STACK_HANDLED_BREAK;
        return OK;
    }

    Error err = OK;
    ExpectReturnValue expected;
    memset(&expected, 0, sizeof(ExpectReturnValue));

    Node *operator=(*stack)->operator;
    if (!operator|| operator->type != NODE_TYPE_SYMBOL) {
        ERROR_PREP(err, ERROR_TYPE,
                   "Parsing context operator must be symbol. Likely internal "
                   "error :(");
        return err;
    }

    if (strcmp(operator->value.symbol, "if-cond") == 0) {
        // TODO: Maybe eventually allow multiple expressions in an if
        // condition, or something like that.
        EXPECT(expected, "{", current, length, end);
        if (expected.found) {
            Node *if_then_body = node_allocate();
            if_then_body->type = NODE_TYPE_PROGRAM;
            (*stack)->result->next_child = if_then_body;
            Node *if_then_first_expr = node_allocate();
            node_add_child(if_then_body, if_then_first_expr);

            // Empty if-then-body handling.
            // TODO: Maybe warn?
            EXPECT(expected, "}", current, length, end);
            if (expected.found) {
                // TODO: First check for else...
                *stack = (*stack)->parent;
                *status = STACK_HANDLED_CHECK;
                return OK;
            }

            // TODO: Should new parsing context be created for scope of if body?
            // TODO: Don't leak stack->operator.
            (*stack)->operator= node_symbol("if-then-body");
            (*stack)->body = if_then_body;
            (*stack)->result = if_then_first_expr;
            *working_result = if_then_first_expr;
            *status = STACK_HANDLED_PARSE;
            return OK;
        }
        // TODO/FIXME: Ask the user how to proceed when if has no body.
        ERROR_PREP(err, ERROR_SYNTAX,
                   "`if` expression requires a \"then\" body.");
        return err;
    }

    if (strcmp(operator->value.symbol, "if-then-body") == 0) {
        // Evaluate next expression unless it's a closing brace.
        EXPECT(expected, "}", current, length, end);
        if (expected.done || expected.found) {
            // TODO: Lookahead for else then parse if-else-body.
            EXPECT(expected, "else", current, length, end);
            if (expected.found) {
                EXPECT(expected, "{", current, length, end);
                if (expected.found) {
                    Node *if_else_body = node_allocate();
                    Node *if_else_first_expr = node_allocate();
                    node_add_child(if_else_body, if_else_first_expr);

                    (*stack)->body->next_child = if_else_body;

                    // TODO: Don't leak stack operator!
                    (*stack)->operator= node_symbol("if-else-body");
                    (*stack)->body = if_else_body;
                    (*stack)->result = if_else_first_expr;
                    *working_result = if_else_first_expr;
                    *status = STACK_HANDLED_PARSE;
                    return OK;
                }
                ERROR_PREP(err, ERROR_SYNTAX,
                           "`else` must be followed by body.");
                return err;
            }

            *stack = (*stack)->parent;
            *status = STACK_HANDLED_CHECK;
            return OK;
        }
        Node *next_expr = node_allocate();
        (*stack)->result->next_child = next_expr;
        (*stack)->result = next_expr;
        *working_result = next_expr;
        *status = STACK_HANDLED_PARSE;
        return OK;
    }

    if (strcmp(operator->value.symbol, "if-else-body") == 0) {
        // Evaluate next expression unless it's a closing brace.
        EXPECT(expected, "}", current, length, end);
        if (expected.done || expected.found) {
            *stack = (*stack)->parent;
            *status = STACK_HANDLED_CHECK;
            return OK;
        }
        Node *next_expr = node_allocate();
        node_add_child((*stack)->result, next_expr);
        *working_result = next_expr;
        *status = STACK_HANDLED_PARSE;
        return OK;
    }

    if (strcmp(operator->value.symbol, "lambda") == 0) {
        // Evaluate next expression unless it's a closing brace.
        EXPECT(expected, "}", current, length, end);
        if (expected.done || expected.found) {
            EXPECT(expected, "]", current, length, end);
            if (expected.done || expected.found) {
                *context = (*context)->parent;
                *stack = (*stack)->parent;
                *status = STACK_HANDLED_CHECK;
                return OK;
            } else {
                ERROR_PREP(err, ERROR_SYNTAX,
                           "Expected closing square bracket for following "
                           "lambda body definition");
                return err;
            }
        }

        (*stack)->result->next_child = node_allocate();
        *working_result = (*stack)->result->next_child;
        (*stack)->result = *working_result;
        *status = STACK_HANDLED_PARSE;
        return OK;
    }
    if (strcmp(operator->value.symbol, "fn") == 0) {
        // Evaluate next expression unless it's a closing brace.
        EXPECT(expected, "}", current, length, end);
        if (expected.done || expected.found) {
            *context = (*context)->parent;
            *stack = (*stack)->parent;
            if (!(*stack)) {
                *status = STACK_HANDLED_BREAK;
            } else {
                *status = STACK_HANDLED_CHECK;
            }
            return OK;
        }

        (*stack)->result->next_child = node_allocate();
        *working_result = (*stack)->result->next_child;
        (*stack)->result = *working_result;
        *status = STACK_HANDLED_PARSE;
        return OK;
    }
    if (strcmp(operator->value.symbol, "fncall") == 0) {
        EXPECT(expected, ")", current, length, end);
        if (expected.done || expected.found) {

            *stack = (*stack)->parent;

            int found = 0;
            err = parse_binary_infix_operator(*context, *stack, &found, current,
                                              length, end, working_precedence,
                                              result, working_result);
            if (found) {
                *status = STACK_HANDLED_PARSE;
            } else {
                *status = STACK_HANDLED_CHECK;
            }
            return OK;
        }

        // FIXME?: Should comma be optional?
        EXPECT(expected, ",", current, length, end);
        if (expected.done || !expected.found) {
            print_token(*current);
            ERROR_PREP(err, ERROR_SYNTAX,
                       "Parameter list expected closing parenthesis or comma "
                       "for another parameter");
            return err;
        }
        (*stack)->result->next_child = node_allocate();
        *working_result = (*stack)->result->next_child;
        (*stack)->result = *working_result;
        *status = STACK_HANDLED_PARSE;
        return OK;
    }
    *status = STACK_HANDLED_INVALID;
    ERROR_PREP(err, ERROR_GENERIC,
               "Internal error: Could not handle stack operator!");
    return err;
}

Error parse_expr(ParsingContext *context, char *source, char **end,
                 Node *result) {
    ParsingStack *stack = NULL;
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

        } else {

            Node *symbol =
                node_symbol_from_buffer(current_token.beginning, token_length);

#ifdef DEBUG_COMPILER
            if (strcmp(symbol->value.symbol, "pint") == 0) {
                working_result->type = NODE_TYPE_DEBUG_PRINT_INTEGER;
                Node *next = node_allocate();
                node_add_child(working_result, next);
                working_result = next;
                continue;
            }

#endif // DEBUG

            if (strcmp("if", symbol->value.symbol) == 0) {
                Node *if_conditional = working_result;
                if_conditional->type = NODE_TYPE_IF;
                // Node *cond_node = node_allocate();
                // cond_node->type = NODE_TYPE_CONDITION;
                Node *condition_expression = node_allocate();
                // node_add_child(cond_node, condition_expression);
                node_add_child(if_conditional, condition_expression);
                working_result = condition_expression;

                stack = parse_stack_create(stack);
                stack->operator= node_symbol("if-cond");
                stack->result = working_result;
                continue;
            }

            // Parse lambda
            if (strcmp("[", symbol->value.symbol) == 0) {

                Node *lambda = working_result;
                lambda->type = NODE_TYPE_FUNCTION_DECLARATION;

                // Return type
                lex_advance(&current_token, &token_length, end);
                Node *return_type = node_allocate();
                return_type->type = NODE_TYPE_FUNCTION_RETURN_TYPE;
                Node *function_return_type = node_symbol_from_buffer(
                    current_token.beginning, token_length);
                node_add_child(return_type, function_return_type);
                // TODO: Ensure function return type is a valid type.

                // Parameter list
                EXPECT(expected, "(", &current_token, &token_length, end);
                if (!expected.found || expected.done) {
                    ERROR_PREP(
                        err, ERROR_SYNTAX,
                        "Parameter list required within lambda definition");
                    return err;
                }

                Node *parameter_list = node_allocate();
                parameter_list->type = NODE_TYPE_FUNCTION_PARAMS_LIST;

                // FIXME?: Should we possibly create a parser stack and evaluate
                // the next expression, then ensure return value is var. decl.
                // in stack handling below?
                for (;;) {
                    EXPECT(expected, ")", &current_token, &token_length, end);
                    if (expected.found) {
                        break;
                    }
                    if (expected.done) {
                        ERROR_PREP(
                            err, ERROR_SYNTAX,
                            "Expected closing parenthesis for parameter list");
                        return err;
                    }

                    err = lex_advance(&current_token, &token_length, end);
                    if (err.type) {
                        return err;
                    }
                    Node *parameter_name = node_symbol_from_buffer(
                        current_token.beginning, token_length);

                    EXPECT(expected, ":", &current_token, &token_length, end);
                    if (expected.done || !expected.found) {
                        ERROR_PREP(
                            err, ERROR_SYNTAX,
                            "Parameter declaration requires a type annotation");
                        return err;
                    }

                    lex_advance(&current_token, &token_length, end);
                    Node *parameter_type = node_symbol_from_buffer(
                        current_token.beginning, token_length);

                    Node *parameter = node_allocate();
                    parameter_type->type = NODE_TYPE_FUNCTION_PARAM;
                    node_add_child(parameter, parameter_name);
                    node_add_child(parameter, parameter_type);

                    node_add_child(parameter_list, parameter);

                    EXPECT(expected, ",", &current_token, &token_length, end);
                    if (expected.found) {
                        continue;
                    }

                    EXPECT(expected, ")", &current_token, &token_length, end);
                    if (!expected.found) {
                        ERROR_PREP(err, ERROR_SYNTAX,
                                   "Expected closing parenthesis following "
                                   "parameter list");
                        return err;
                    }
                    break;
                }

                // TODO/FIXME: Do I need to bind unnamed function in
                // environment?
                // environment_set(context->functions, function_name,
                // working_result);

                // Parse function body.
                EXPECT(expected, "{", &current_token, &token_length, end);
                if (expected.done || !expected.found) {
                    ERROR_PREP(err, ERROR_SYNTAX,
                               "Function definition requires body following "
                               "return type");
                    return err;
                }

                context = parse_context_create(context);
                Node *param_it = parameter_list->children;
                while (param_it) {
                    environment_set(context->variables, param_it->children,
                                    param_it->children->next_child);
                    param_it = param_it->next_child;
                }

                Node *function_body = node_allocate();
                function_body->type = NODE_TYPE_PROGRAM;
                Node *function_first_expression = node_allocate();
                node_add_child(function_body, function_first_expression);
                working_result = function_first_expression;

                node_add_child(lambda, parameter_list);
                node_add_child(lambda, return_type);
                node_add_child(lambda, function_body);

                stack = parse_stack_create(stack);
                stack->operator= node_symbol("lambda");
                stack->result = working_result;

                continue;
            }

            // TODO: Parse strings and other literal types.

            // TODO: Check for unary prefix operators.

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

                EXPECT(expected, "(", &current_token, &token_length, end);
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

                // FIXME?: Should we possibly create a parser stack and evaluate
                // the next expression, then ensure return value is var. decl.
                // in stack handling below?
                for (;;) {
                    EXPECT(expected, ")", &current_token, &token_length, end);
                    if (expected.found) {
                        break;
                    }
                    if (expected.done) {
                        ERROR_PREP(
                            err, ERROR_SYNTAX,
                            "Expected closing parenthesis for parameter list");
                        return err;
                    }

                    err = lex_advance(&current_token, &token_length, end);
                    if (err.type) {
                        return err;
                    }
                    Node *parameter_name = node_symbol_from_buffer(
                        current_token.beginning, token_length);

                    EXPECT(expected, ":", &current_token, &token_length, end);
                    if (expected.done || !expected.found) {
                        ERROR_PREP(
                            err, ERROR_SYNTAX,
                            "Parameter declaration requires a type annotation");
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

                    EXPECT(expected, ",", &current_token, &token_length, end);
                    if (expected.found) {
                        continue;
                    }

                    EXPECT(expected, ")", &current_token, &token_length, end);
                    if (!expected.found) {
                        ERROR_PREP(err, ERROR_SYNTAX,
                                   "Expected closing parenthesis following "
                                   "parameter list");
                        return err;
                    }
                    break;
                }

                // Parse return type.
                EXPECT(expected, ":", &current_token, &token_length, end);
                // TODO/FIXME: Should we allow implicit return type?
                if (expected.done || !expected.found) {
                    ERROR_PREP(err, ERROR_SYNTAX,
                               "Function definition requires return type "
                               "annotation following parameter list");
                    return err;
                }

                lex_advance(&current_token, &token_length, end);
                Node *return_type = node_allocate();
                return_type->type = NODE_TYPE_FUNCTION_RETURN_TYPE;
                Node *function_return_type = node_symbol_from_buffer(
                    current_token.beginning, token_length);
                node_add_child(return_type, function_return_type);
                node_add_child(working_result, return_type);

                // Bind function to function name in functions environment.
                environment_set(context->functions, function_name,
                                working_result);

                // Parse function body.
                EXPECT(expected, "{", &current_token, &token_length, end);
                if (expected.done || !expected.found) {
                    ERROR_PREP(err, ERROR_SYNTAX,
                               "Function definition requires body following "
                               "return type");
                    return err;
                }

                context = parse_context_create(context);
                Node *param_it = working_result->children->children;
                while (param_it) {
                    environment_set(context->variables, param_it->children,
                                    param_it->children->next_child);
                    param_it = param_it->next_child;
                }

                Node *function_body = node_allocate();
                function_body->type = NODE_TYPE_PROGRAM;
                Node *function_first_expression = node_allocate();
                node_add_child(function_body, function_first_expression);
                node_add_child(working_result, function_body);
                working_result = function_first_expression;

                stack = parse_stack_create(stack);
                stack->operator= node_symbol("fn");
                stack->result = working_result;

                continue;
            }

            // Check if valid symbol for variable environment, then
            // attempt to pattern match variable access, assignment,
            // declaration, or declaration with initialization.

            EXPECT(expected, ":", &current_token, &token_length, end);
            if (!expected.done && expected.found) {

                // Re-assignment of existing variable (look for =)
                EXPECT(expected, "=", &current_token, &token_length, end);
                if (expected.found) {
                    Node *variable_binding = node_allocate();
                    if (!environment_get(*context->variables, symbol,
                                         variable_binding)) {
                        // TODO: Add source location or something to the error.
                        // TODO: Create new error type.
                        printf("ID of undeclared variable: \"%s\"\n",
                               symbol->value.symbol);
                        ERROR_PREP(err, ERROR_GENERIC,
                                   "Reassignment of a variable that has not "
                                   "been declared!");
                        return err;
                    }
                    free(variable_binding);

                    working_result->type = NODE_TYPE_VARIABLE_REASSIGNMENT;
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
                err = parse_get_type(context, type_symbol, type_value);
                if (err.type) {
                    free(type_value);
                    return err;
                }
                if (nonep(*type_value)) {
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

                // `symbol` is now owned by working_result, a var. decl.
                node_add_child(working_result, symbol);

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

                EXPECT(expected, "=", &current_token, &token_length, end);
                if (expected.found) {

                    working_result->next_child = node_allocate();

                    Node **local_result = &result;
                    if (stack) {
                        local_result = &stack->result;
                    }

                    Node *reassign = node_allocate();
                    reassign->type = NODE_TYPE_VARIABLE_REASSIGNMENT;
                    Node *value_expression = node_allocate();
                    node_add_child(reassign, node_symbol(symbol->value.symbol));
                    node_add_child(reassign, value_expression);

                    (*local_result)->next_child = reassign;
                    *local_result = reassign;

                    working_result = value_expression;
                    continue;
                }

            } else {
                // Symbol is not `fn` and it is not followed by an assignment
                // operator `:`.

                // Check if it's a function call (lookahead for symbol)
                EXPECT(expected, "(", &current_token, &token_length, end);
                if (!expected.done && expected.found) {
                    working_result->type = NODE_TYPE_FUNCTION_CALL;
                    node_add_child(working_result, symbol);
                    Node *argument_list = node_allocate();
                    argument_list->type = NODE_TYPE_FUNCTION_ARGS_LIST;
                    Node *first_argument = node_allocate();
                    node_add_child(argument_list, first_argument);
                    node_add_child(working_result, argument_list);
                    working_result = first_argument;
                    // Create a parsing stack with function call operator,
                    // and then start parsing function argument expressions.
                    stack = parse_stack_create(stack);
                    stack->operator= node_symbol("fncall");
                    stack->result = working_result;
                    continue;
                }

                ParsingContext *context_it = context;
                Node *variable = node_allocate();
                while (context_it) {
                    if (environment_get(*context_it->variables, symbol,
                                        variable)) {
                        break;
                    }
                    context_it = context_it->parent;
                }

                if (!context_it) {
                    printf("Symbol: ");
                    print_node_internal(symbol, 0, BCYN, false);
                    ERROR_PREP(err, ERROR_SYNTAX, "Unknown symbol");
                    return err;
                }

                // print_token(current_token);
                // putchar('\n');

                // Variable access node
                Node *access = node_allocate();
                access->type = NODE_TYPE_VARIABLE_ACCESS;

                node_add_child(access, symbol);
                *working_result = *access;

                free(variable);
                free(access);
            }
        }

        int found = 0;
        err = parse_binary_infix_operator(
            context, stack, &found, &current_token, &token_length, end,
            &working_precedence, result, &working_result);
        if (found) {
            continue;
        }

        // If no more parser stack, return with current result.
        // Otherwise, handle parser stack operator.
        if (!stack) {
            break;
        }

        // Return Actions:
        // 1 :: Break (stack was NULL most likely)
        // 2 :: Continue Parsing (working_result was updated, possibly stack as
        // well) 3 :: Continue Checking (stack was updated, may need to handle
        // it as well)

        int status = 1;
        do {
            err = handle_stack_operator(
                &status, &context, &stack, &working_result, result,
                &working_precedence, &current_token, &token_length, end);
            if (err.type) {
                return err;
            }
        } while (status == STACK_HANDLED_CHECK);

        if (status == STACK_HANDLED_BREAK) {
            break;
        }
        if (status == STACK_HANDLED_PARSE) {
            continue;
        }

        printf("status: %d\n", status);
        ERROR_PREP(
            err, ERROR_GENERIC,
            "Internal Compiler Error :(. Unhandled parse stack operator.");
        return err;
    }

    return err;
}
