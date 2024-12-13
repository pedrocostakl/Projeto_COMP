/*
**  Pedro Sousa da Costa - 2022220304
**  Marco Manuel Almeida e Silva - 2021211653
*/

#include "codegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantics.h"

#define FORMAT_INT             "@.format_int"
#define FORMAT_FLOAT32         "@.format_float32"
#define FORMAT_BOOL_TRUE       "@.format_bool_true"
#define FORMAT_BOOL_FALSE      "@.format_bool_false"
#define FORMAT_STRLIT          "@.format_strlit"

static int temporary;
static unsigned int label_num;
static enum category_t prev_category;
static struct node_t *global_program;

extern struct symbol_list_t *global_symbol_table;

enum label_type_t {
    LabelThen = 0,
    LabelElse,
    LabelEnd,
    LabelFor,
    LabelPrintTrue,
    LabelPrintFalse
};

static void codegen_var(struct node_t *var, struct symbol_list_t *scope, int global);
static void codegen_function(struct node_t *function);
static void codegen_parameters(struct node_t *parameters, struct symbol_list_t *scope);
static void codegen_block(struct node_t *block, struct symbol_list_t *scope);
static int codegen_statement(struct node_t *statement, struct symbol_list_t *scope);
static int codegen_expression(struct node_t *expression, struct symbol_list_t *scope);
static void codegen_string_literals(struct node_t *parent);
static void print_codegen_type(enum type_t type);
static void print_label(unsigned int num, enum label_type_t label_type);
static void print_type_zero(enum type_t type);
static void print_tab();
static unsigned int get_strlit_hash(const char *strlit);
static int is_strlit_declared(struct node_t *parent, unsigned int hash);
static int logical_strlen(const char *str);
static float decimal_value(const char *token);

void codegen_program(struct node_t *program) {
    global_program = program;
    label_num = 0;

    // declarar funções I/O
    printf("; C language I/O functions\n");
    printf("declare i32 @printf(i8*, ...)\n");
    printf("declare i32 @atoi(i8*)\n\n");

    // declarar formatos de print
    printf("; global string literals responsible for the printf formats\n");
    printf("%s = private constant [4 x i8] c\"%%d\\0A\\00\"\n", FORMAT_INT);
    printf("%s = private constant [6 x i8] c\"%%.8f\\0A\\00\"\n", FORMAT_FLOAT32);
    printf("%s = private constant [6 x i8] c\"true\\0A\\00\"\n", FORMAT_BOOL_TRUE);
    printf("%s = private constant [7 x i8] c\"false\\0A\\00\"\n", FORMAT_BOOL_FALSE);
    printf("%s = private constant [4 x i8] c\"%%s\\0A\\00\"\n", FORMAT_STRLIT);
    printf("\n");

    // declarar string literals globais
    printf("; string literals used in the program\n");
    codegen_string_literals(program);
    printf("\n");

    // declarar variáveis globais de argumentos
    printf("; global program arguments\n");
    printf("@.argv = global i8** null\n");
    printf("\n");

    int var_num = 0;
    struct symbol_list_t *symbol;

    // variáveis globais
    printf("; global program variables\n");
    symbol = global_symbol_table->next;
    while (symbol != NULL) {
        struct node_t *node = symbol->node;
        switch (node->category) {
            case VarDecl: {
                codegen_var(node, global_symbol_table, 1);
                var_num++;
            } break;
            default:
                break;
        }
        symbol = symbol->next;
    }
    if (var_num > 0) {
        printf("\n");
    }

    // definição de funções
    symbol = global_symbol_table->next;
    while (symbol != NULL) {
        struct node_t *node = symbol->node;
        switch (node->category) {
            case FuncDecl: {
                codegen_function(node);
            } break;
            default:
                break;
        }
        symbol = symbol->next;
    }

    // entry point
    printf("; entry point\n");
    struct symbol_list_t *main_symbol = search_symbol(global_symbol_table, "main");
    if (main_symbol != NULL && main_symbol->node->category == FuncDecl) {
        printf("define i32 @main(i32 %%argc, i8** %%argv) {\n"
               "  store i8** %%argv, i8*** @.argv\n"
               "  %%1 = call i32 @_main()\n"
               "  ret i32 %%1\n"
               "}\n");
    } else {
        printf("define i32 @main(i32 %%argc, i8** %%argv) {\n"
               "  ret i32 0\n"
               "}\n");
    }
}

void codegen_var(struct node_t *var, struct symbol_list_t *scope, int global) {
    struct symbol_list_t *var_symbol = search_symbol(scope, getchild(var, 1)->token);
    if (var_symbol != NULL && var_symbol->node->category == VarDecl) {
        if (global == 1) {
            printf("@%s = global ", var_symbol->identifier);
            print_codegen_type(var_symbol->type);
            printf(" ");
            print_type_zero(var_symbol->type);
        } else {
            print_tab();
            printf("%%%s = alloca ", var_symbol->identifier);
            print_codegen_type(var_symbol->type);
        }
        printf("\n");
    }
}

void codegen_function(struct node_t *function) {
    temporary = 1;
    struct node_t *header = getchild(function, 0);
    struct node_list_t *children;
    
    struct symbol_list_t *function_symbol = search_symbol(global_symbol_table, getchild(header, 0)->token);
    if (function_symbol != NULL && function_symbol->node->category == FuncDecl) {
        struct symbol_list_t *scope = function_symbol->scope;
        // imprimir o define
        printf("define ");
        print_codegen_type(function_symbol->type);
        printf(" @_%s(", function_symbol->identifier);
        // procurar o node de parameters
        struct node_t *parameters = getchild(header, 1);
        if (parameters->category != FuncParams) {
            parameters = getchild(header, 2);
        }
        // imprimir os parametros
        int num = 0;
        children = parameters->children->next;
        while (children != NULL) {
            struct node_t *param = children->node;
            struct symbol_list_t *param_symbol = search_symbol(scope, getchild(param, 1)->token);
            if (param_symbol != NULL) {
                if (num > 0) {
                    printf(",");
                }
                print_codegen_type(param_symbol->type);
                printf(" %%%s", param_symbol->identifier);
                num++;
            }
            children = children->next;
        }
        printf(") {\n");
        // declarar parametros
        codegen_parameters(parameters, scope);
        struct node_t *body = getchild(function, 1);
        // declarar variáveis locais
        struct node_list_t *children = body->children->next;
        while (children != NULL) {
            struct node_t *node = children->node;
            switch (node->category) {
                case VarDecl: {
                    codegen_var(node, scope, 0);
                } break;
                default:
                    break;
            }
            children = children->next;
        }
        // gerar código da função
        label_num = 0;
        codegen_block(body, scope);
        // adicionar o statement de return default
        print_tab();
        printf("ret ");
        print_codegen_type(function_symbol->type);
        printf(" ");
        print_type_zero(function_symbol->type);
        printf("\n}\n\n");
    }
}

void codegen_parameters(struct node_t *parameters, struct symbol_list_t *scope) {
    if (parameters->category == FuncParams) {
        struct node_list_t *children = parameters->children->next;
        while (children != NULL) {
            struct node_t *param = children->node;
            struct symbol_list_t *param_symbol = search_symbol(scope, getchild(param, 1)->token);
            if (param_symbol != NULL) {
                // alocar espaço na stack
                print_tab();
                printf("%%.param_%s = alloca ", param_symbol->identifier);
                print_codegen_type(param_symbol->type);
                printf("\n");
                // guardar o valor original do parametro
                print_tab();
                printf("store ");
                print_codegen_type(param_symbol->type);
                printf(" %%%s, ", param_symbol->identifier);
                print_codegen_type(param_symbol->type);
                printf("* %%.param_%s\n", param_symbol->identifier);
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
            unsigned int if_label_num = label_num;
            label_num++;

            struct node_list_t *children = statement->children->next;
            int tmp1 = codegen_expression(children->node, scope);

            // branch
            print_tab();
            printf("br i1 %%%d", tmp1);
            printf(", label %%");
            print_label(if_label_num, LabelThen);
            printf(", label %%");
            print_label(if_label_num, LabelElse);
            printf("\n");

            // if
            print_label(if_label_num, LabelThen);
            printf(":\n");
            children = children->next;
            codegen_statement(children->node, scope);
            if (prev_category != Return) {
                print_tab();
                printf("br label %%");
                print_label(if_label_num, LabelEnd);
                printf("\n");
            }
            prev_category = Intermediate;
            
            // else
            print_label(if_label_num, LabelElse);
            printf(":\n");
            children = children->next;
            codegen_statement(children->node, scope);
            if (prev_category != Return) {
                print_tab();
                printf("br label %%");
                print_label(if_label_num, LabelEnd);
                printf("\n");
            }

            // fim
            print_label(if_label_num, LabelEnd);
            printf(":\n");
        } break;
        case For: {
            unsigned int for_label_num = label_num;
            label_num++;

            struct node_list_t *children = statement->children->next;

            print_tab();
            printf("br label %%");
            print_label(for_label_num, LabelFor);
            printf("\n");
            print_label(for_label_num, LabelFor);
            printf(":\n");

            int tmp1 = codegen_expression(children->node, scope);

            // branch
            print_tab();
            printf("br i1 %%%d", tmp1);
            printf(", label %%");
            print_label(for_label_num, LabelThen);
            printf(", label %%");
            print_label(for_label_num, LabelEnd);
            printf("\n");

            print_label(for_label_num, LabelThen);
            printf(":\n");

            codegen_statement(children->next->node, scope);

            if (prev_category != Return) {
                print_tab();
                printf("br label %%");
                print_label(for_label_num, LabelFor);
                printf("\n");
            }

            print_label(for_label_num, LabelEnd);
            printf(":\n");
        } break;
        case Return: {
            struct node_t *expr = statement->children->next->node;
            int tmp1 = codegen_expression(expr, scope);
            print_tab();
            printf("ret ");
            print_codegen_type(expr->type);
            printf(" %%%d\n", tmp1);
        } break;
        case Print: {
            struct node_t *expr = getchild(statement, 0);
            int tmp1 = codegen_expression(expr, scope);
            switch (expr->type) {
                case TypeInteger: {
                    print_tab();
                    printf("%%%d = getelementptr [4 x i8], [4 x i8]* %s, i32 0, i32 0\n", temporary, FORMAT_INT);
                    print_tab();
                    temporary++;
                    printf("%%%d = call i32 (i8*, ...) @printf(i8* %%%d, i32 %%%d)\n", temporary, temporary - 1, tmp1);
                } break;
                case TypeFloat32: {
                    print_tab();
                    printf("%%%d = getelementptr [6 x i8], [6 x i8]* %s, i32 0, i32 0\n", temporary, FORMAT_FLOAT32);
                    print_tab();
                    temporary++;
                    printf("%%%d = call i32 (i8*, ...) @printf(i8* %%%d, double %%%d)\n", temporary, temporary - 1, tmp1);
                } break;
                case TypeBool: {
                    int print_label_num = label_num;
                    label_num++;

                    // branch
                    print_tab();
                    printf("br i1 %%%d", tmp1);
                    printf(", label %%");
                    print_label(print_label_num, LabelPrintTrue);
                    printf(", label %%");
                    print_label(print_label_num, LabelPrintFalse);
                    printf("\n");

                    // imprimir true
                    print_label(print_label_num, LabelPrintTrue);
                    printf(":\n");
                    print_tab();
                    printf("%%%d = getelementptr [6 x i8], [6 x i8]* %s, i32 0, i32 0\n", temporary, FORMAT_BOOL_TRUE);
                    print_tab();
                    temporary++;
                    printf("%%%d = call i32 (i8*, ...) @printf(i8* %%%d)\n", temporary, temporary - 1);
                    temporary++;
                    print_tab();
                    printf("br label %%");
                    print_label(print_label_num, LabelEnd);
                    printf("\n");

                    // imprimir false
                    print_label(print_label_num, LabelPrintFalse);
                    printf(":\n");
                    print_tab();
                    printf("%%%d = getelementptr [7 x i8], [7 x i8]* %s, i32 0, i32 0\n", temporary, FORMAT_BOOL_FALSE);
                    print_tab();
                    temporary++;
                    printf("%%%d = call i32 (i8*, ...) @printf(i8* %%%d)\n", temporary, temporary - 1);
                    print_tab();
                    printf("br label %%");
                    print_label(print_label_num, LabelEnd);
                    printf("\n");

                    // terminar
                    print_label(print_label_num, LabelEnd);
                    printf(":\n");
                } break;
                case TypeString: {
                    char *strlit = strdup(expr->token);
                    int len = logical_strlen(strlit);
                    memmove(strlit, strlit + 1, len - 1);
                    strlit[len -2] = '\0';
                    len -= 1; // retirar as duas " e adicionar o \00

                    print_tab();
                    printf("%%%d = getelementptr [4 x i8], [4 x i8]* %s, i32 0, i32 0\n", temporary, FORMAT_STRLIT);
                    print_tab();
                    temporary++;
                    printf("%%%d = getelementptr [%d x i8], [%d x i8]* @.%u, i32 0, i32 0\n", temporary, len, len, get_strlit_hash(strlit));
                    print_tab();
                    temporary++;
                    printf("%%%d = call i32 (i8*, ...) @printf(i8* %%%d, i8* %%%d)\n", temporary, temporary - 2, temporary - 1);
                } break;
                default:
                    break;
            }
            temporary++;
            tmp = temporary;
        } break;
        case ParseArgs: {
            struct node_t *id = getchild(statement, 0);
            struct node_t *expr = getchild(statement, 1);
            struct symbol_list_t *symbol = search_symbol(scope, id->token);
            if (symbol == NULL) {
                symbol = search_symbol(global_symbol_table, id->token);
            }
            int tmp1 = codegen_expression(expr, scope);
            print_tab();
            printf("%%%d = load i8**, i8*** @.argv\n", temporary);
            temporary++;
            print_tab();
            printf("%%%d = getelementptr i8*, i8** %%%d, i32 %%%d\n", temporary, temporary - 1, tmp1);
            temporary++;
            print_tab();
            printf("%%%d = load i8*, i8** %%%d\n", temporary, temporary - 1);
            temporary++;
            print_tab();
            printf("%%%d = call i32 @atoi(i8* %%%d)\n", temporary, temporary - 1);
            print_tab();
            switch (symbol->symbol_type) {
                case SymbolGlobalVar: {
                    printf("store i32 %%%d, i32* @%s\n", temporary, symbol->identifier);
                } break;
                case SymbolLocalVar: {
                    printf("store i32 %%%d, i32* %%%s\n", temporary, symbol->identifier);
                } break;
                case SymbolParam: {
                    printf("store i32 %%%d, i32* %%.param_%s\n", temporary, symbol->identifier);
                } break;
                default:
                    break;
            }
            temporary++;
        } break;
        case Assign: {
            struct node_t *id = getchild(statement, 0);
            struct symbol_list_t *symbol = search_symbol(scope, id->token);
            if (symbol == NULL) {
                symbol = search_symbol(global_symbol_table, id->token);
            }
            int tmp1 = codegen_expression(getchild(statement, 1), scope);
            print_tab();
            switch (symbol->symbol_type) {
                case SymbolGlobalVar: {
                    printf("store ");
                    print_codegen_type(statement->type);
                    printf(" %%%d, ", tmp1);
                    print_codegen_type(statement->type);
                    printf("* @%s\n", symbol->identifier);
                } break;
                case SymbolLocalVar: {
                    printf("store ");
                    print_codegen_type(statement->type);
                    printf(" %%%d, ", tmp1);
                    print_codegen_type(statement->type);
                    printf("* %%%s\n", symbol->identifier);
                } break;
                case SymbolParam: {
                    printf("store ");
                    print_codegen_type(statement->type);
                    printf(" %%%d, ", tmp1);
                    print_codegen_type(statement->type);
                    printf("* %%.param_%s\n", symbol->identifier);
                } break;
                default:
                    break;
            }
        } break;
        default:
            break;
    }
    if (statement->category != Block) {
        prev_category = statement->category;
    }
    return tmp;
}

int codegen_expression(struct node_t *expression, struct symbol_list_t *scope) {
    int tmp = -1;
    switch (expression->category) {
        case Natural: {
            print_tab();
            printf("%%%d = add i32 %s, 0\n", temporary, expression->token);
            tmp = temporary;
            temporary++;
        } break;
        case Decimal: {
            print_tab();
            float value = decimal_value(expression->token);
            printf("%%%d = fadd double %f, 0.0\n", temporary, value);
            tmp = temporary;
            temporary++;
        } break;
        case Identifier: {
            struct symbol_list_t *symbol = search_symbol(scope, expression->token);
            if (symbol == NULL) {
                symbol = search_symbol(global_symbol_table, expression->token);
            }
            print_tab();
            switch (symbol->symbol_type) {
                case SymbolGlobalVar: {
                    printf("%%%d = load ", temporary);
                    print_codegen_type(expression->type);
                    printf(", ");
                    print_codegen_type(expression->type);
                    printf("* @%s\n", symbol->identifier);
                } break;
                case SymbolLocalVar: {
                    printf("%%%d = load ", temporary);
                    print_codegen_type(expression->type);
                    printf(", ");
                    print_codegen_type(expression->type);
                    printf("* %%%s\n", symbol->identifier);
                } break;
                case SymbolParam: {
                    printf("%%%d = load ", temporary);
                    print_codegen_type(expression->type);
                    printf(", ");
                    print_codegen_type(expression->type);
                    printf("* %%.param_%s\n", symbol->identifier);
                } break;
                default:
                    break;
            }
            tmp = temporary;
            temporary++;
        } break;
        case StrLit: {
            // String literals são geradas no início da geração do programa
        } break;
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
            print_tab();
            printf("%%%d = call ", temporary);
            print_codegen_type(expression->type);
            printf(" @_%s(", id->token);
            children = expression->children->next->next;
            int num = 0;
            i = 0;
            while (children != NULL) {
                struct node_t *expr = children->node;
                if (num > 0) {
                    printf(",");
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
        case Or: {
            struct node_t *expr = getchild(expression, 0);
            int tmp1 = codegen_expression(expr, scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = or ", temporary);
            print_codegen_type(expr->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case And: {
            struct node_t *expr = getchild(expression, 0);
            int tmp1 = codegen_expression(expr, scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = and ", temporary);
            print_codegen_type(expr->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Eq: {
            struct node_t *expr = getchild(expression, 0);
            int tmp1 = codegen_expression(expr, scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            if (expr->type != TypeFloat32) {
                printf("icmp eq ");
            } else {
                printf("fcmp oeq ");
            }
            print_codegen_type(expr->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Ne: {
            struct node_t *expr = getchild(expression, 0);
            int tmp1 = codegen_expression(expr, scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            if (expr->type != TypeFloat32) {
                printf("icmp ne ");
            } else {
                printf("fcmp une ");
            }
            print_codegen_type(expr->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Lt: {
            struct node_t *expr = getchild(expression, 0);
            int tmp1 = codegen_expression(expr, scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            if (expr->type != TypeFloat32) {
                printf("icmp slt ");
            } else {
                printf("fcmp olt ");
            }
            print_codegen_type(expr->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Gt: {
            struct node_t *expr = getchild(expression, 0);
            int tmp1 = codegen_expression(expr, scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            if (expr->type != TypeFloat32) {
                printf("icmp sgt ");
            } else {
                printf("fcmp ogt ");
            }
            print_codegen_type(expr->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Le: {
            struct node_t *expr = getchild(expression, 0);
            int tmp1 = codegen_expression(expr, scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            if (expr->type != TypeFloat32) {
                printf("icmp sle ");
            } else {
                printf("fcmp ole ");
            }
            print_codegen_type(expr->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Ge: {
            struct node_t *expr = getchild(expression, 0);
            int tmp1 = codegen_expression(expr, scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            if (expr->type != TypeFloat32) {
                printf("icmp sge ");
            } else {
                printf("fcmp oge ");
            }
            print_codegen_type(expr->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Add: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            switch (expression->type) {
                case TypeInteger: {
                    printf("add ");
                } break;
                case TypeFloat32: {
                    printf("fadd ");
                } break;
                default:
                    printf("add ");
                    break;
            }
            print_codegen_type(expression->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Sub: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            switch (expression->type) {
                case TypeInteger: {
                    printf("sub ");
                } break;
                case TypeFloat32: {
                    printf("fsub ");
                } break;
                default:
                    break;
            }
            print_codegen_type(expression->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Mul: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            switch (expression->type) {
                case TypeInteger: {
                    printf("mul ");
                } break;
                case TypeFloat32: {
                    printf("fmul ");
                } break;
                default:
                    break;
            }
            print_codegen_type(expression->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Div: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            switch (expression->type) {
                case TypeInteger: {
                    printf("sdiv ");
                } break;
                case TypeFloat32: {
                    printf("fdiv ");
                } break;
                default:
                    break;
            }
            print_codegen_type(expression->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Mod: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            int tmp2 = codegen_expression(getchild(expression, 1), scope);
            print_tab();
            printf("%%%d = ", temporary);
            switch (expression->type) {
                case TypeInteger: {
                    printf("srem ");
                } break;
                case TypeFloat32: {
                    printf("frem ");
                } break;
                default:
                    break;
            }
            print_codegen_type(expression->type);
            printf(" %%%d, %%%d\n", tmp1, tmp2);
            tmp = temporary;
            temporary++;
        } break;
        case Not: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            print_tab();
            printf("%%%d = xor i1 %%%d, 1\n", temporary, tmp1);
            tmp = temporary;
            temporary++;
        } break;
        case Minus: {
            int tmp1 = codegen_expression(getchild(expression, 0), scope);
            print_tab();
            printf("%%%d = ", temporary);
            switch (expression->type) {
                case TypeInteger: {
                    printf("mul i32 -1, ");
                } break;
                case TypeFloat32: {
                    printf("fmul double -1.0, ");
                } break;
                default:
                    break;
            }
            printf("%%%d\n", tmp1);
            tmp = temporary;
            temporary++;
        } break;
        case Plus: {
            tmp = codegen_expression(getchild(expression, 0), scope);
        } break;
        default:
            break;
    }
    prev_category = expression->category;
    return tmp;
}

void codegen_string_literals(struct node_t *parent) {
    struct node_list_t *children = parent->children->next;
    while (children != NULL) {
        struct node_t *node = children->node;
        if (node->category == StrLit) {
            char *strlit = strdup(node->token);
            int len = logical_strlen(strlit);
            memmove(strlit, strlit + 1, len - 1);
            strlit[len -2] = '\0';
            len -= 1; // retirar as duas " e adicionar o \00
            unsigned int hash = get_strlit_hash(strlit);
            if (is_strlit_declared(global_program, hash) == 0) {
                printf("@.%u = private constant [%d x i8] c\"%s\\00\"\n", get_strlit_hash(strlit), len, strlit);
            }
            node->hash = hash;
            free(strlit);
        } else {
            codegen_string_literals(node);
        }
        children = children->next;
    }
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

void print_label(unsigned int num, enum label_type_t label_type) {
    switch (label_type) {
        case LabelThen: {
            printf("L%uthen", num);
        } break;
        case LabelElse: {
            printf("L%uelse", num);
        } break;
        case LabelEnd: {
            printf("L%uend", num);
        } break;
        case LabelFor: {
            printf("L%ufor", num);
        } break;
        case LabelPrintTrue: {
            printf("L%uprint_true", num);
        } break;
        case LabelPrintFalse: {
            printf("L%uprint_false", num);
        } break;
        default:
            break;
    }
}

void print_type_zero(enum type_t type) {
    switch (type) {
        case None:
        case TypeInteger: {
            printf("0");
        } break;
        case TypeFloat32: {
            printf("0.0");
        } break;
        default:
            break;
    }
}

void print_tab() {
    printf("  ");
}

unsigned int get_strlit_hash(const char *strlit) {
    unsigned int hash = 0;
    while (*strlit) {
        hash = hash * 31 + (unsigned char)(*strlit);
        strlit++;
    }
    return hash;
}

int is_strlit_declared(struct node_t *parent, unsigned int hash) {
    struct node_list_t *children = parent->children->next;
    while (children != NULL) {
        struct node_t *node = children->node;
        if (node->category == StrLit) {
            if (node->hash == hash) {
                return 1;
            }
        } else {
            int res = is_strlit_declared(node, hash);
            if (res == 1) return 1;
        }
        children = children->next;
    }
    return 0;
}

int logical_strlen(const char *str) {
    int len = 0;
    int escape = 0;
    while (*str) {
        if (*str == '\\' && escape == 0) {
            escape = 1;
        } else {
            escape = 0;
            len++;
        }
        str++;
    }
    return len;
}

float decimal_value(const char *token) {
    const char *ch = token;
    const char *exp_pos = NULL;
    while (*ch) {
        if (*ch == 'e' || *ch == 'E') {
            exp_pos = ch;
            break;
        }
        ch++;
    }
    if (exp_pos != NULL) {
        float base;
        int exp;
        char base_str[32];
        char exp_str[32];

        size_t base_len = exp_pos - token;
        snprintf(base_str, base_len + 1, "%s", token);
        snprintf(exp_str, sizeof(exp_str), "%s", exp_pos + 1);

        base = atof(base_str);
        exp = atoi(exp_str);

        int power = 1;
        while (exp != 0) {
            power *= 10;
            exp--;
        }
        return base * power;
    } else {
        return atof(token);
    }
}