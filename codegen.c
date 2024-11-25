/*
**  Pedro Sousa da Costa - 2022220304
**  Marco Manuel Almeida e Silva - 2021211653
*/

#include "codegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantics.h"

extern struct symbol_list_t *global_symbol_table;

static int temporary;

static void codegen_var(struct node_t *var, struct symbol_list_t *scope);
static void codegen_function(struct node_t *function);
static void codegen_parameters(struct node_t *parameters, struct symbol_list_t *scope);
static void codegen_block(struct node_t *block, struct symbol_list_t *scope);
static int codegen_statement(struct node_t *statement, struct symbol_list_t *scope);
static int codegen_expression(struct node_t *expression, struct symbol_list_t *scope);

static void print_codegen_type(enum type_t type);

void codegen_program(struct node_t *program) {
    printf("\n");
    // funções IO pre-declaradas
    printf("declare i32 @_read(i32)\n");
    printf("declare i32 @_write(i32)\n\n");

    enum category_t category = None;

    // geração dos nodes de declaração global
    struct node_list_t *children = program->children->next;
    while (children != NULL) {
        struct node_t *node = children->node;
        if (node->category != category) {
            printf("\n");
            category = node->category;
        }
        switch (category) {
            case VarDecl: {
                codegen_var(node, global_symbol_table);
            } break;
            case FuncDecl: {
                codegen_function(node);
            } break;
            default:
                break;
        }
        children = children->next;
    }
}

void codegen_var(struct node_t *var, struct symbol_list_t *scope) {
    struct symbol_list_t *var_symbol = search_symbol(scope, getchild(var, 1)->token);
    if (var_symbol != NULL && var_symbol->node->category == VarDecl) {
        printf("declare ");
        print_codegen_type(var_symbol->type);
        printf(" @%s\n", var_symbol->identifier);
    }
}

void codegen_function(struct node_t *function) {
    temporary = 1;
    struct node_t *header = getchild(function, 0);
    
    struct symbol_list_t *function_symbol = search_symbol(global_symbol_table, getchild(header, 0)->token);
    if (function_symbol != NULL && function_symbol->node->category == FuncDecl) {
        struct symbol_list_t *scope = function_symbol->scope;
        printf("define ");
        print_codegen_type(function_symbol->type);
        printf(" @%s(", function_symbol->identifier);
        // procurar o node de parameters
        struct node_t *parameters = getchild(header, 1);
        if (parameters->category != FuncParams) {
            parameters = getchild(header, 2);
        }
        codegen_parameters(parameters, scope);
        printf(") {\n");
        codegen_block(getchild(function, 1), scope);
        printf("}\n\n");
    }
}

void codegen_parameters(struct node_t *parameters, struct symbol_list_t *scope) {
    int num = 0;
    if (parameters->category == FuncParams) {
        struct node_list_t *children = parameters->children->next;
        while (children != NULL) {
            struct node_t *param = children->node;
            struct symbol_list_t *param_symbol = search_symbol(scope, getchild(param, 1)->token);
            if (param_symbol != NULL) {
                if (num > 0) {
                    printf(", ");
                }
                print_codegen_type(param_symbol->type);
                printf(" %%%s", param_symbol->identifier);
                num++;
            }
            children = children->next;
        }
    }
}

void codegen_block(struct node_t *block, struct symbol_list_t *scope) {
    struct node_list_t *children = block->children->next;
    while (children != NULL) {
        struct node_t *node = children->node;
        switch (node->category) {
            case VarDecl:
                printf("  ");
                codegen_var(node, scope);
                break;
            case Block:
            case If:
            case For:
            case Return:
            case Print:
            case ParseArgs:
            case Assign:
                codegen_statement(node, scope);
                break;
            case Call:
            case Or:
            case And:
            case Eq:
            case Ne:
            case Lt:
            case Gt:
            case Le:
            case Ge:
            case Add:
            case Sub:
            case Mul:
            case Div:
            case Mod:
            case Not:
            case Minus:
            case Plus:
                codegen_expression(node, scope);
                break;
            default:
                break;
        }
        children = children->next;
    }
}

int codegen_statement(struct node_t *statement, struct symbol_list_t *scope) {
    int tmp = -1;
    switch (statement->category) {
        case Block: {
            codegen_block(statement, scope);
        } break;
        case If: {
            struct node_list_t *children = statement->children->next;
            int tmp1 = codegen_expression(children->node, scope);

            printf("  ");
            printf("br i1 %%%d, label L1then, label L1else\n", tmp1);

            printf("L1then:\n");
            children = children->next;
            codegen_statement(children->node, scope);
            printf("  ");
            printf("br label L1end\n");

            printf("L1else:\n");
            children = children->next;
            codegen_statement(children->node, scope);
            printf("  ");
            printf("br label L1end\n");

            printf("L1end:\n");
        } break;
        case For: {
            struct node_list_t *children = statement->children->next;
            codegen_expression(children->node, scope);
            codegen_statement(children->next->node, scope);
        } break;
        case Return: {
            struct node_t *expr = statement->children->next->node;
            int tmp1 = codegen_expression(expr, scope);
            printf("  ");
            printf("ret ");
            print_codegen_type(expr->type);
            printf(" %%%d\n", tmp1);
        } break;
        case Print:
        case ParseArgs:
            break;
        case Assign: {
            codegen_expression(getchild(statement, 1), scope);
        } break;
        default:
            break;
    }
    return tmp;
}

int codegen_expression(struct node_t *expression, struct symbol_list_t *scope) {
    int tmp = -1;
    switch (expression->category) {
        case Natural: {
            printf("  ");
            printf("%%%d = add i32 %s, 0\n", temporary, expression->token);
            tmp = temporary;
            temporary++;
        } break;
        case Decimal: {
            printf("  ");
            printf("%%%d = add double %s, 0\n", temporary, expression->token);
            tmp = temporary;
            temporary++;
        } break;
        case Identifier:
        case StrLit:
            break;
        case Call: {
            int i = 0;
            int tmps[100];
            struct node_t *id = getchild(expression, 0);
            struct node_list_t *children = expression->children->next->next;
            while (children != NULL) {
                struct node_t *expr = children->node;
                tmps[i] = codegen_expression(expr, scope);
                children = children->next;
                i++;
            }
            printf("  ");
            printf("%%%d = call ", temporary);
            print_codegen_type(expression->type);
            printf(" @%s(", id->token);
            children = expression->children->next->next;
            int num = 0;
            i = 0;
            while (children != NULL) {
                struct node_t *expr = children->node;
                if (num > 0) {
                    printf(", ");
                }
                print_codegen_type(expr->type);
                printf(" %%%d", tmps[i]);
                children = children->next;
                num++;
                i++;
            }
            printf(")\n");
            tmp = temporary;
            temporary++;
        } break;
        case Or:
        case And:
        case Eq:
            break;
        case Ne: {
            struct node_t *expr = getchild(expression, 0);
            int tmp1 = codegen_expression(expr, scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            printf("  ");
            printf("%%%d = icmp ne ", temporary);
            print_codegen_type(expr->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Lt:
        case Gt:
        case Le:
        case Ge:
            break;
        case Add: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            printf("  ");
            printf("%%%d = add ", temporary);
            print_codegen_type(expression->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Sub: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            printf("  ");
            printf("%%%d = sub ", temporary);
            print_codegen_type(expression->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Mul: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            printf("  ");
            printf("%%%d = mul ", temporary);
            print_codegen_type(expression->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Div: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            printf("  ");
            printf("%%%d = sdiv ", temporary);
            print_codegen_type(expression->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Mod:
        case Not:
        case Minus:
        case Plus:
        default:
            break;
    }
    return tmp;
}

void print_codegen_type(enum type_t type) {
    switch (type) {
        case None: {
            printf("i32");
        } break;
        case TypeInteger: {
            printf("i32");
        } break;
        case TypeFloat32: {
            printf("double");
        } break;
        case TypeBool: {
            printf("i1");
        } break;
        case TypeString: {
            printf("i8*");
        } break;
        default:
            break;
    }
}