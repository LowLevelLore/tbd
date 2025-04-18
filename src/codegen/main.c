#include "../headers/codegen/main.h"
#include "registers.c"

static const int VERBOSITY = 1;

//======================MZ_CODEGEN_UTILS BEGINS======================
void write_bytes(char *code, FILE *outfile) {
    if (!outfile || !code) {
        return;
    }
    if (fwrite(code, 1, strlen(code), outfile) != strlen(code)) {
        Error err;
        ERROR_PREP(err, ERROR_GENERIC,
                   "write_bytes(): Cannot write '%s' bytes to asm file.", code);
        log_error(&err);
        exit(0);
    }
}
void write_line(char *code, FILE *outfile) {
    if (!outfile || !code) {
        return;
    }
    write_bytes(code, outfile);
    write_bytes("\n", outfile);
}
void write_integer(long long integer, FILE *outfile) {
    if (!outfile) {
        return;
    }
    if (sprintf(CG_INT_BUFFER, "%lld", integer) < 0) {
        Error err;
        ERROR_PREP(err, ERROR_GENERIC,
                   "write_integer(): Cannot integer %lld to buffer.", integer);
        log_error(&err);
        exit(0);
    }
    if (fwrite(CG_INT_BUFFER, 1, strlen(CG_INT_BUFFER), outfile) !=
        strlen(CG_INT_BUFFER)) {
        Error err;
        ERROR_PREP(err, ERROR_GENERIC,
                   "write_line(): Cannot write '%s' bytes to asm file.",
                   CG_INT_BUFFER);
        log_error(&err);
        exit(0);
    }
}

CodegenContext *codegen_context_create(CodegenContext *parent) {
    CodegenContext *cg_ctx = calloc(1, sizeof(CodegenContext));
    cg_ctx->parent = parent;
    cg_ctx->locals = environment_create(NULL);
    cg_ctx->locals_offset = -32;
    return cg_ctx;
}

char *label_generate() {
    char *label = CG_LABEL_BUFFER + label_index;
    label_index += snprintf(label, CG_LABEL_BUFFER_LENGTH - label_index,
                            ".L%lld", label_count);
    label_index++;
    if (label_index >= CG_LABEL_BUFFER_LENGTH) {
        label_index = 0;
        return label_generate();
    }
    label_count++;
    return label;
}

void define_binary_operator(ParsingContext *context, char *operator,
                            int precedence, char *return_type, char *lhs_type,
                            char *rhs_type) {
    Node *bin_op = node_allocate();
    bin_op->type = NODE_TYPE_BINARY_OPERATOR;
    bin_op->value.symbol = operator;
    node_add_child(bin_op, node_integer(precedence));
    node_add_child(bin_op, node_symbol(return_type));
    node_add_child(bin_op, node_symbol(lhs_type));
    node_add_child(bin_op, node_symbol(rhs_type));

    while (context->parent) {
        context = context->parent;
    }

    int status = environment_set(context->binary_operators,
                                 node_symbol(operator), bin_op);

    if (status == 0) {
        Error err;
        ERROR_PREP(
            err, ERROR_GENERIC,
            "Cannot set binary operator '%s' in the environment.", operator);
        log_error(&err);
        exit(0);
    }
    return;
}
//======================MZ_CODEGEN_UTILS ENDS========================

//======================MZ_CODEGEN_COMMON BEGINS=====================

//======================MZ_CODEGEN_COMMON ENDS=======================

//======================MZ_CODEGEN_OUTPUT_FORMAT_x86_64_ASM_MSWIN
// BEGINS======================

char *symbol_to_addr(CodegenContext *cg_context, Node *symbol) {
    char *symbol_str = CG_SYMBOL_BUFFER + symbol_index;
    // For global variables
    if (!cg_context->parent) {
        symbol_index +=
            snprintf(symbol_str, CG_SYMBOL_BUFFER_LENGTH - symbol_index,
                     "%s(%%rip)", symbol->value.symbol);
    } else {
        Node *stack_offset = node_allocate();
        if (!environment_get(*cg_context->locals, symbol, stack_offset)) {
            log_system_errors("INTERNAL COMPILER ERROR :( !");
            return NULL;
        }
        symbol_index +=
            snprintf(symbol_str, CG_SYMBOL_BUFFER_LENGTH - symbol_index,
                     "%lld(%%rbp)", stack_offset->value.MZ_integer);
        free(stack_offset);
    }
    // TODO: Local Variables
    symbol_index++;
    if (symbol_index >= CG_SYMBOL_BUFFER_LENGTH) {
        symbol_index = 0;
        return symbol_to_addr(cg_context, symbol);
    }
    return symbol_str;
}

Error codegen_function_x86_64_att_mswin(ParsingContext *context,
                                        ParsingContext **next_child,
                                        CodegenContext *cg_context, Register *r,
                                        char *name, Node *function,
                                        FILE *outfile);

Error codegen_expression_x86_64_mswin(ParsingContext *context,
                                      ParsingContext *next_child,
                                      CodegenContext *cg_context, Register *r,
                                      Node *expression, FILE *outfile) {
    Error err = OK;
    ParsingContext *og_context = context;
    Node *vessel_0 = node_allocate();
    Node *vessel_1 = node_allocate();
    Node *vessel_2 = node_allocate();
    char *buffer = (char *)malloc(256 * sizeof(char));
    switch (expression->type) {
    case NODE_TYPE_IF:
        if (VERBOSITY) {
            LINE(";#; IF");
        }
        err =
            codegen_expression_x86_64_mswin(og_context, next_child, cg_context,
                                            r, expression->children, outfile);
        ret;
        char *otherwise_label = label_generate();
        char *after_otherwise_label = label_generate();
        char *condition_register_name =
            register_name(r, expression->children->result_register);
        LINE("test %s, %s", condition_register_name, condition_register_name);
        LINE("jz %s", otherwise_label);
        register_deallocate(r, expression->children->result_register);

        vessel_0 = expression->children->next_child->children;
        Node *last_expr = NULL;
        while (vessel_0) {
            err = codegen_expression_x86_64_mswin(
                context, next_child, cg_context, r, vessel_0, outfile);
            ret;
            if (last_expr) {
                register_deallocate(r, last_expr->result_register);
            }
            last_expr = vessel_0;
            vessel_0 = vessel_0->next_child;
        }
        register_deallocate(r, last_expr->result_register);
        expression->result_register = last_expr->result_register;
        LINE("jmp %s", after_otherwise_label);
        LINE("%s:", otherwise_label);

        if (expression->children->next_child->next_child) {
            last_expr = NULL;
            vessel_0 = expression->children->next_child->next_child->children;
            while (vessel_0) {
                err = codegen_expression_x86_64_mswin(
                    context, next_child, cg_context, r, vessel_0, outfile);
                ret;
                if (last_expr) {
                    register_deallocate(r, last_expr->result_register);
                }
                last_expr = vessel_0;
                vessel_0 = vessel_0->next_child;
            }
            register_deallocate(r, last_expr->result_register);
        }

        LINE("%s:", after_otherwise_label);

        break;
    case NODE_TYPE_FUNCTION_CALL:
        if (VERBOSITY) {
            LINE(";#; FUNCTION CALL : `%s`",
                 expression->children->value.symbol);
        }
        // TODO: Push the arguments in reverse order
        LINE("push %%rax");
        long long offset = 0;
        vessel_0 = expression->children->next_child->children;
        while (vessel_0) {
            err = codegen_expression_x86_64_mswin(
                context, next_child, cg_context, r, vessel_0, outfile);
            LINE("push %s", register_name(r, vessel_0->result_register));
            register_deallocate(r, vessel_0->result_register);
            if (vessel_0->type == NODE_TYPE_INTEGER) {
                offset += 8;
            } else if (vessel_0->type == NODE_TYPE_VARIABLE_ACCESS) {
                while (context) {
                    if (environment_get(*context->variables, vessel_0->children,
                                        vessel_1)) {
                        break;
                    }
                    context = context->parent;
                }
                parse_get_type(context, vessel_1, vessel_2);
                offset += vessel_2->children->value.MZ_integer;
            } else {
                offset += 8;
            }
            vessel_0 = vessel_0->next_child;
        }

        LINE("call MZ_fn_%s", expression->children->value.symbol);
        // Assumes all arguments integers
        LINE("add $%lld, %%rsp", offset);
        expression->result_register = register_allocate(r);
        if (strcmp(register_name(r, expression->result_register), "%rax") !=
            0) {
            LINE("mov %%rax, %s",
                 register_name(r, expression->result_register));
            LINE("pop %%rax");
        } else {
            LINE("add $8, %%rsp");
        }
        break;
    case NODE_TYPE_INTEGER:
        if (VERBOSITY) {
            LINE(";#; INTEGER : `%lld`", expression->value.MZ_integer);
        }
        expression->result_register = register_allocate(r);
        LINE("mov $%lld, %s", expression->value.MZ_integer,
             register_name(r, expression->result_register));
        break;
    case NODE_TYPE_BINARY_OPERATOR:
        if (VERBOSITY) {
            LINE(";#; BINARY OPERATOR : `%s`", expression->value.symbol);
        }
        Node *result = node_allocate();
        while (context->parent) {
            context = context->parent;
        }
        environment_get(*context->binary_operators,
                        node_symbol(expression->value.symbol), result);

        // LHS
        err =
            codegen_expression_x86_64_mswin(og_context, next_child, cg_context,
                                            r, expression->children, outfile);
        ret;
        // RHS
        err = codegen_expression_x86_64_mswin(
            og_context, next_child, cg_context, r,
            expression->children->next_child, outfile);
        ret;
        if (strcmp(expression->value.symbol, "+") == 0) {

            expression->result_register =
                expression->children->next_child->result_register;

            LINE("add %s, %s",
                 register_name(r, expression->children->result_register),
                 register_name(
                     r, expression->children->next_child->result_register));

            register_deallocate(r, expression->children->result_register);

        } else if (strcmp(expression->value.symbol, "-") == 0) {

            expression->result_register = expression->children->result_register;

            LINE("sub %s, %s",
                 register_name(
                     r, expression->children->next_child->result_register),
                 register_name(r, expression->children->result_register));

            register_deallocate(
                r, expression->children->next_child->result_register);
        } else if (strcmp(expression->value.symbol, "*") == 0) {

            expression->result_register =
                expression->children->next_child->result_register;

            LINE("imul %s, %s",
                 register_name(r, expression->children->result_register),
                 register_name(
                     r, expression->children->next_child->result_register));

            register_deallocate(r, expression->children->result_register);
        } else if (strcmp(expression->value.symbol, "/") == 0) {
        } else if (strcmp(expression->value.symbol, "=") == 0) {
            expression->result_register = register_allocate(r);
            RegisterDescriptor true_val = register_allocate(r);
            LINE("mov $0, %s", register_name(r, expression->result_register));
            LINE("cmp %s, %s",
                 register_name(r, expression->children->result_register),
                 register_name(
                     r, expression->children->next_child->result_register));
            LINE("mov $1, %s", register_name(r, true_val));
            LINE("cmove %s, %s", register_name(r, true_val),
                 register_name(r, expression->result_register));

            register_deallocate(r, true_val);
            register_deallocate(r, expression->children->result_register);
            register_deallocate(
                r, expression->children->next_child->result_register);
        } else if (strcmp(expression->value.symbol, "<") == 0) {
            expression->result_register = register_allocate(r);
            RegisterDescriptor true_val = register_allocate(r);
            LINE("mov $0, %s", register_name(r, expression->result_register));
            LINE("cmp %s, %s",
                 register_name(
                     r, expression->children->next_child->result_register),
                 register_name(r, expression->children->result_register));
            LINE("mov $1, %s", register_name(r, true_val));
            LINE("cmovl %s, %s", register_name(r, true_val),
                 register_name(r, expression->result_register));

            register_deallocate(r, true_val);
            register_deallocate(r, expression->children->result_register);
            register_deallocate(
                r, expression->children->next_child->result_register);
        } else if (strcmp(expression->value.symbol, ">") == 0) {
            expression->result_register = register_allocate(r);
            RegisterDescriptor true_val = register_allocate(r);
            LINE("mov $0, %s", register_name(r, expression->result_register));
            LINE("cmp %s, %s",
                 register_name(
                     r, expression->children->next_child->result_register),
                 register_name(r, expression->children->result_register));
            LINE("mov $1, %s", register_name(r, true_val));
            LINE("cmovg %s, %s", register_name(r, true_val),
                 register_name(r, expression->result_register));

            register_deallocate(r, true_val);
            register_deallocate(r, expression->children->result_register);
            register_deallocate(
                r, expression->children->next_child->result_register);
        }

        else {
            ERROR_PREP(err, ERROR_GENERIC, "Unknown Binary operator : '%s'",
                       expression->value.symbol);
        }

        free(result);
        break;
    case NODE_TYPE_FUNCTION_DECLARATION:
        if (!cg_context->parent) {
            break;
        }
        buffer = label_generate();
        err = codegen_function_x86_64_att_mswin(
            context, &next_child, cg_context, r, buffer, expression, outfile);
        ret;
        break;
    case NODE_TYPE_DEBUG_PRINT_INTEGER:
        if (VERBOSITY) {
            LINE(";#; DEBUG PRINT INTEGER");
        }
        err = codegen_expression_x86_64_mswin(context, next_child, cg_context,
                                              r, expression->children, outfile);
        LINE("mov %s, %%rdx",
             register_name(r, expression->children->result_register));
        LINE("mov %%rax, %%rbx");
        LINE("lea fmt(%%rip), %%rcx");
        LINE("call printf");
        LINE("mov %%rax, %%rbx");

        break;
    case NODE_TYPE_VARIABLE_DECLARATION:
        if (VERBOSITY) {
            LINE(";#; VARIABLE DECLARATION : `%s`",
                 expression->children->value.symbol);
        }
        if (!cg_context->parent) {
            int rd = register_allocate(r);
            LINE("lea %s, %s", symbol_to_addr(cg_context, expression->children),
                 register_name(r, rd));
            register_deallocate(r, rd);
            break;
        }
        // Making space for integer
        ParsingContext *found_context = context;
        while (found_context) {
            if (environment_get(*found_context->variables, expression->children,
                                vessel_0)) {
                break;

                found_context = found_context->parent;
            }
        }
        parse_get_type(found_context, vessel_0, vessel_1);
        LINE("sub $%lld, %%rsp", vessel_1->children->value.MZ_integer);
        environment_set(cg_context->locals, expression->children,
                        node_integer(cg_context->locals_offset));
        cg_context->locals_offset -= vessel_1->children->value.MZ_integer;

        break;
    case NODE_TYPE_VARIABLE_ACCESS:
        if (VERBOSITY) {
            LINE(";#; VARIABLE ACCESS : `%s`",
                 expression->children->value.symbol);
        }
        if (!context->parent) {
            expression->result_register = register_allocate(r);
            LINE("mov %s(%%rip), %s", expression->children->value.symbol,
                 register_name(r, expression->result_register));
        } else {

            if (!environment_get(*cg_context->locals, expression->children,
                                 vessel_0)) {
                ERROR_PREP(err, ERROR_GENERIC, "Internal Compiler Error");
                break;
            }

            expression->result_register = register_allocate(r);
            LINE("mov %lld(%%rbp), %s", vessel_0->value.MZ_integer,
                 register_name(r, expression->result_register));
        }
        break;
    case NODE_TYPE_VARIABLE_REASSIGNMENT:
        if (VERBOSITY) {
            LINE(";#; VARIABLE REASSIGNMENT : `%s`",
                 expression->children->value.symbol);
        }
        if (expression->children->next_child->type == NODE_TYPE_INTEGER) {
            BYTES("movq $");
            write_integer(expression->children->next_child->value.MZ_integer,
                          outfile);
            LINE(", %s", symbol_to_addr(cg_context, expression->children));
        } else {
            err = codegen_expression_x86_64_mswin(
                context, next_child, cg_context, r,
                expression->children->next_child, outfile);
            ret;
            char *result_register = register_name(
                r, expression->children->next_child->result_register);
            LINE("mov %s, %s", result_register,
                 symbol_to_addr(cg_context, expression->children));
            register_deallocate(
                r, expression->children->next_child->result_register);
        }

        break;

    default:
        break;
    }
    free(buffer);
    free(vessel_0);
    free(vessel_1);
    free(vessel_2);
    return err;
}

void codegen_header_x86_64_att_mswin(FILE *outfile) {
    char *lines[5] = {";#; ==== ALIGN HEADER ====", "push %rbp",
                      "mov %rsp, %rbp", "sub $32, %rsp", ""};
    for (int i = 0; i < 5; i++) {
        write_line(lines[i], outfile);
    }
}

void codegen_footer_x86_64_att_mswin(FILE *outfile, bool exit_code) {
    if (!exit_code) {
        char *lines[3] = {"pop %rbp", "ret", ";#; ==== ALIGN FOOTER ===="};
        for (int i = 0; i < 3; i++) {
            write_line(lines[i], outfile);
        }
    } else {
        char *lines[6] = {
            "",         "mov $0, %rax", "add $32, %rsp",
            "pop %rbp", "ret",          ";#; ==== ALIGN FOOTER ===="};
        for (int i = 0; i < 6; i++) {
            write_line(lines[i], outfile);
        }
    }
}

Error codegen_function_x86_64_att_mswin(ParsingContext *context,
                                        ParsingContext **next_child,
                                        CodegenContext *cg_context, Register *r,
                                        char *name, Node *function,
                                        FILE *outfile) {
    Error err = OK;
    cg_context = codegen_context_create(cg_context);

    size_t param_count = 1;
    Node *parameter = function->children->children;
    while (parameter) {
        Node *result = node_allocate();
        param_count++;
        // TODO: Try to pass args to register
        environment_get(*context->types, parameter->children->next_child,
                        result);
        environment_set(
            cg_context->locals, parameter->children,
            node_integer(param_count * result->children->value.MZ_integer));
        parameter = parameter->next_child;
    }

    LINE("jmp MZ_fn_after%s", name);
    LINE("MZ_fn_%s:", name);

    codegen_header_x86_64_att_mswin(outfile);

    LINE("push %%rbx");
    LINE("push %%rsi");
    LINE("push %%rdi");

    ParsingContext *ctx = next_child ? *next_child : context;
    Node *expression = function->children->next_child->next_child->children;
    while (expression) {
        err = codegen_expression_x86_64_mswin(ctx, *next_child, cg_context, r,
                                              expression, outfile);
        ret;
        if (!expression->next_child) {
            char *name = register_name(r, expression->result_register);
            if (strcmp(name, "%rax")) {
                LINE("mov %s, %%rax\n", name);
            }
        }
        expression = expression->next_child;
    }
    if (next_child)
        *next_child = (*next_child)->next_child;
    LINE(" ");
    LINE("pop %%rdi");
    LINE("pop %%rsi");
    LINE("pop %%rbx");
    LINE("add $%lld, %%rsp", -cg_context->locals_offset);
    codegen_footer_x86_64_att_mswin(outfile, false);

    LINE("MZ_fn_after%s:", name);
    cg_context = cg_context->parent;

    return err;
}

Error codegen_program_x86_64_mswin(ParsingContext *context,
                                   CodegenContext *cg_context, Node *program,
                                   FILE *outfile) {
    Error err = OK;
    Register *r = register_create("%rax");
    register_add(r, "%r10");
    register_add(r, "%r11");
    register_add(r, "%rbx");
    register_add(r, "%rdi");
    register_add(r, "%rsi");

    LINE(";#; This assembly was generated for x86_64 Windows by the MZ "
         "Compiler");

    LINE(".data");

    write_line("\n;#; -------------------------Global Variables "
               "START-------------------------\n",
               outfile);
    // log_message("HERE");

#ifdef DEBUG_COMPILER
    LINE("fmt: .asciz \"%%lld\\n\"");
#endif // DEBUG

    Binding *var_it = context->variables->binding;

    while (var_it) {
        Node *var_id = var_it->id;
        Node *type = var_it->value;
        Node *type_info = node_allocate();
        environment_get(*context->types, type, type_info);
        LINE("%s: .space %lld", var_id->value.symbol,
             type_info->children->value.MZ_integer);
        var_it = var_it->next;
    }

    write_line("\n;#; -------------------------Global Variables "
               "END-------------------------\n",
               outfile);

    LINE(".global _start");

    LINE(".text");
    write_line("\n;#; -------------------------Global Functions "
               "START-------------------------\n",
               outfile);

    Binding *func_it = context->functions->binding;
    ParsingContext *next_child = context->children;

    while (func_it) {
        Node *func_id = func_it->id;
        Node *function = func_it->value;
        func_it = func_it->next;

        char *name = func_id->value.symbol;
        err = codegen_function_x86_64_att_mswin(
            context, &next_child, cg_context, r, name, function, outfile);
        ret;
    }

    write_line("\n;#; -------------------------Global Functions "
               "END-------------------------\n",
               outfile);

    LINE("_start:");
    codegen_header_x86_64_att_mswin(outfile);
    Node *expression = program->children;
    while (expression) {
        codegen_expression_x86_64_mswin(context, next_child, cg_context, r,
                                        expression, outfile);
        register_deallocate(r, expression->result_register);
        expression = expression->next_child;
    }

    codegen_footer_x86_64_att_mswin(outfile, true);
    register_free(r);
    return err;
}

//======================MZ_CODEGEN_OUTPUT_FORMAT_x86_64_ASM_MSWIN
// ENDS======================

Error codegen_program(ParsingContext *context, Node *program,
                      CodegenOutputFormat format, char *filepath) {
    Error err = OK;
    FILE *outfile = fopen(filepath, "wb");

    CodegenContext *cg_context = codegen_context_create(NULL);

    if (!outfile) {
        ERROR_PREP(err, ERROR_GENERIC, "Cannot open file '%s' for codegen! ",
                   filepath);
        return err;
    }

    switch (format) {
    case MZ_CODEGEN_OUTPUT_FORMAT_x86_64_ASM_MSWIN:
        err =
            codegen_program_x86_64_mswin(context, cg_context, program, outfile);
        ret;
        break;

    default:
        err =
            codegen_program_x86_64_mswin(context, cg_context, program, outfile);
        ret;
        break;
    }

    fclose(outfile);
    return err;
}