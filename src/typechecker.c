#include "./headers/typechecker.h"

int expression_return_type(ParsingContext *context, Node *expression) {
    Node *result = node_allocate();
    switch (expression->type) {
    case NODE_TYPE_FUNCTION_CALL:
        while (context) {
            if (environment_get(*context->functions, expression->children,
                                result)) {
                result = result->children->next_child;
                break;
            }
            context = context->parent;
        }

        break;

    default:
        return expression->type;
    }
    return result->type;
}

Error typecheck_expression(ParsingContext *context, Node *expression) {
    Error err = OK;
    Node *vessel_0 = node_allocate();
    Node *vessel_1 = node_allocate();
    Node *vessel_2 = node_allocate();
    // int type = NODE_TYPE_NULL;
    switch (expression->type) {
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
            exprected++;
            recieved++;
            parse_get_type(context, vessel_1->children->next_child, vessel_2);
            if (vessel_2->type != vessel_0->type) {
                ERROR_PREP(
                    err, ERROR_TYPE,
                    "The %dth argument of the function call [%s] doesn't "
                    "match the function definition.",
                    exprected, expression->children->value.symbol);
                ret;
            }
            vessel_0 = vessel_0->next_child;
            vessel_1 = vessel_1->next_child;
        }

        if (vessel_0 != NULL || vessel_1 != NULL) {
            while (vessel_0 != NULL) {
                recieved++;
                vessel_0 = vessel_0->next_child;
            }
            while (vessel_1 != NULL) {
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