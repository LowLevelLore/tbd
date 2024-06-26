#include "./headers/typechecker.h"

Error expression_return_type(ParsingContext *context, Node *expression,
                             int *type) {

    Error err = OK;
    ParsingContext *og_context = context;
    Node *result = node_allocate();
    Node *vessel = node_allocate();
    switch (expression->type) {
    case NODE_TYPE_VARIABLE_ACCESS:
        // parsing_context_print(og_context, 0);
        while (context) {
            if (environment_get(*context->variables, expression->children,
                                result)) {
                break;
            }
            context = context->parent;
        }

        if (!context) {
            printf("Variable : `%s`", expression->value.symbol);
            ERROR_PREP(err, ERROR_GENERIC,
                       "Couldnt get variable access return type");
            break;
        }
        context = og_context;
        while (context) {
            if (environment_get(*context->types, result, vessel))
                break;
            context = context->parent;
        }
        if (!context) {
            printf("Type : `%s`", result->value.symbol);
            ERROR_PREP(err, ERROR_GENERIC,
                       "Couldnt get variable accessreturn type");
            break;
        }
        result->type = vessel->type;
        break;
    case NODE_TYPE_BINARY_OPERATOR:
        err = typecheck_expression(context, expression);
        brk;
        while (context->parent) {
            context = context->parent;
        }

        environment_get_by_symbol(*context->binary_operators,
                                  expression->value.symbol, result);
        err = parse_get_type(og_context, result->children->next_child, result);
        break;
    case NODE_TYPE_FUNCTION_CALL:
        while (context) {
            if (environment_get(*context->functions, expression->children,
                                result)) {
                // result = result->children->next_child;
                break;
            }
            context = context->parent;
        }
        result = result->children->next_child->children;
        parse_get_type(og_context, result, vessel);
        result->type = vessel->type;
        break;

    default:
        result->type = expression->type;
        *type = expression->type;
        return OK;
    }
    *type = result->type;
    free(vessel);
    free(result);
    return err;
}

Error typecheck_expression(ParsingContext *context, Node *expression) {
    Error err = OK;
    ParsingContext *og_context = context;
    Node *vessel_0 = node_allocate();
    Node *vessel_1 = node_allocate();
    Node *vessel_2 = node_allocate();
    int type = NODE_TYPE_NULL;

    // Typecheck all children
    Node *it = expression->children;
    while (it) {
        it = it->next_child;
        if (it) {
            err = typecheck_expression(context, it);
            if (err.type != ERROR_NULL) {
                free(vessel_0);
                free(vessel_1);
                free(vessel_2);
                free(it);
                return err;
            }
        }
    }

    switch (expression->type) {
    case NODE_TYPE_BINARY_OPERATOR:
        // parsing_context_print(og_context, 0);
        while (context->parent) {
            context = context->parent;
        }

        environment_get_by_symbol(*context->binary_operators,
                                  expression->value.symbol, vessel_0);
        err = expression_return_type(og_context, expression->children, &type);
        brk;
        err = parse_get_type(
            og_context, vessel_0->children->next_child->next_child, vessel_1);
        brk;

        if (type != vessel_1->type) {
            printf("Expected: %d, Recieved: %d\n", vessel_1->type, type);
            ERROR_PREP(err, ERROR_TYPE, "Mismatched LHS Type");
            brk;
        }
        expression_return_type(og_context, expression->children->next_child,
                               &type);
        parse_get_type(og_context, vessel_0->children->next_child->next_child,
                       vessel_1);

        if (type != vessel_1->type) {
            printf("Exprected: %d, Recieved: %d\n", type, vessel_1->type);
            ERROR_PREP(err, ERROR_TYPE, "Mismatched RHS Type");
            brk;
        }

        break;
    case NODE_TYPE_FUNCTION_CALL:
        // TODO: Make sure that the arguments match the parameters...
        while (context) {
            if (environment_get(*context->functions, expression->children,
                                vessel_0)) {
                break;
            }
            context = context->parent;
        }
        int exprected = 0;
        int recieved = 0;
        vessel_1 = vessel_0->children->children;
        vessel_0 = expression->children->next_child->children;
        while (vessel_1 && vessel_0) {
            if (vessel_1->type == NODE_TYPE_NULL &&
                vessel_0->type != NODE_TYPE_NULL) {
                ERROR_PREP(err, ERROR_TYPE,
                           "The function  [%s] doesn't "
                           "accept any arguments.",
                           expression->children->value.symbol);
                ret;
            }
            if (vessel_1->type == NODE_TYPE_NULL &&
                vessel_0->type == NODE_TYPE_NULL) {
                return OK;
            }
            exprected++;
            recieved++;
            int v1;
            expression_return_type(context, vessel_0, &v1);
            parse_get_type(context, vessel_1->children->next_child, vessel_2);
            if (v1 != vessel_2->type) {
                ERROR_PREP(
                    err, ERROR_TYPE,
                    "The %dth argument of the function call [%s] doesn't "
                    "match the function declaration.",
                    exprected, expression->children->value.symbol);
                ret;
            }
            vessel_0 = vessel_0->next_child;
            vessel_1 = vessel_1->next_child;
        }

        if (vessel_0 != NULL || vessel_1 != NULL) {
            while (vessel_0 != NULL) {
                // print_node(vessel_0, 0);
                recieved++;
                vessel_0 = vessel_0->next_child;
            }
            while (vessel_1 != NULL) {
                // print_node(vessel_1, 0);
                exprected++;
                vessel_1 = vessel_1->next_child;
            }

            ERROR_PREP(
                err, ERROR_SYNTAX,
                "Invalid number of arguments in function call [], expected "
                "%d arguments, recieved %d arguments.",
                exprected, recieved);
            ret;
        }

        break;


    default:
        break;
    }
    free(vessel_0);
    free(vessel_1);
    free(vessel_2);
    return err;
}

Error typecheck_program(ParsingContext *context, Node *program) {
    Error err = OK;

    printf("\n");
    log_message("TYPECHECKING");
    printf("\n");

    Node *expression = program->children;
    while (expression) {
        err = typecheck_expression(context, expression);
        ret;
        expression = expression->next_child;
    }

    printf("\n");
    log_message("TYPECHECKING DONE");
    printf("\n");

    return err;
}