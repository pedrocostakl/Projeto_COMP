#include "semantics.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct symbol_list_t *insert_symbol(struct symbol_list_t *table, char *identifier, enum type_t type, struct node_t *node) {
    struct symbol_list_t *new = NULL;
    struct symbol_list_t *symbol = table;
    while (symbol != NULL) {
        if (symbol->next == NULL) {
            new = (struct symbol_list_t*)malloc(sizeof(struct symbol_list_t));
            new->identifier = strdup(identifier);
            new->type = type;
            new->node = node;
            new->next = NULL;
            symbol->next = new;
            break;
        } else if (strcmp(symbol->next->identifier, identifier) == 0) {
            return NULL;
        }
        symbol = symbol->next;
    }
    return new;
}

struct symbol_list_t *search_symbol(struct symbol_list_t *table, char *identifier) {
    struct symbol_list_t *symbol = table->next;
    while (symbol != NULL) {
        if (strcmp(symbol->identifier, identifier) == 0) {
            return symbol;
        }
        symbol = symbol->next;
    }
    return NULL;
}

struct symbol_list_t *enter_scope(struct symbol_list_t *table) {
    if (table == NULL) return NULL;
    table->scope = (struct symbol_list_t*)malloc(sizeof(struct symbol_list_t));
    return table->scope;
}

static int semantic_errors;
static struct symbol_list_t *global_symbol_table;

static void print_parameters(struct node_t *node);
static void show_function(struct symbol_list_t *symbol);

void show_symbol_table() {
    struct symbol_list_t *symbol = global_symbol_table->next;
    printf("===== Global Symbol Table =====\n");
    while (symbol != NULL) {
        switch (symbol->node->category) {
            case FuncDecl:
                printf("%s\t(", symbol->identifier);
                print_parameters(symbol->node);
                printf(")\t");
                print_type(symbol->type);
                printf("\n");
                break;
            case VarDecl:
                printf("%s\t\t", symbol->identifier);
                print_type(symbol->type);
                printf("\n");
                break;
        }
        symbol = symbol->next;
    }
    printf("\n");

    symbol = global_symbol_table->next;
    while (symbol != NULL) {
        if (symbol->node->category == FuncDecl) {
            show_function(symbol);
        }
        symbol = symbol->next;
    }
}

void print_parameters(struct node_t *node) {
    struct node_t *header = getchild(node, 0);
    struct node_t *parameters = getchild(header, 1);
    if (parameters->category != FuncParams) {
        parameters = getchild(header, 2);
    }
    struct node_list_t *children = parameters->children->next;
    while (children != NULL) {
        struct node_list_t *next = children->next;
        print_type(children->node->type);
        if (next != NULL) {
            printf(", ");
        }
        children = next;
    }
}

void show_function(struct symbol_list_t *symbol) {
    //printf("===== Function %s() Symbol Table =====\n", symbol->identifier);
    printf("===== Function %s(", symbol->identifier);
    print_parameters(symbol->node);
    printf(") Symbol Table =====\n");

    printf("return\t\t");
    print_type(symbol->type);
    printf("\n");

    struct symbol_list_t *scope_table = symbol->scope->next;
    while (scope_table != NULL) {
        printf("%s\t\t", scope_table->identifier);
        print_type(scope_table->type);
        printf("\n");
        scope_table = scope_table->next;
    }

    printf("\n");
}

/**
 * Análise semântica
 */

static void check_var(struct symbol_list_t *scope, struct node_t *var);
static void check_function(struct symbol_list_t *scope, struct node_t *function);
static void check_parameters(struct symbol_list_t *scope, struct node_t *parameters);
static void check_function_body(struct symbol_list_t *scope, struct node_t *function_body);

static enum type_t get_type(struct node_t *node);

int check_program(struct node_t *program) {
    global_symbol_table = (struct symbol_list_t*)malloc(sizeof(struct symbol_list_t));
    global_symbol_table->next = NULL;
    struct node_list_t *child = program->children->next;
    while (child != NULL) {
        struct node_t *node = child->node;
        switch (node->category) {
            case VarDecl:
                check_var(global_symbol_table, node);
                break;
            case FuncDecl:
                check_function(global_symbol_table, node);
                break;
            default:
                break;
        }
        child = child->next;
    }
    return semantic_errors;
}

void check_var(struct symbol_list_t *scope, struct node_t *var) {
    struct node_t *id = getchild(var, 1);
    enum type_t type = get_type(getchild(var, 0));
    if (insert_symbol(scope, id->token, type, var) == NULL) {
        printf("Error: identifier already declared: %s\n", id->token);
        semantic_errors++;
    }
}

void check_function(struct symbol_list_t *scope, struct node_t *function) {
    struct node_t *header = getchild(function, 0);

    // Type and parameters node
    enum type_t type = None;
    struct node_t *parameters = getchild(header, 1);
    if (parameters->category != FuncParams) {
        type = get_type(parameters);
        parameters = getchild(header, 2);
    }

    // Insert Identifier
    struct node_t *id = getchild(header, 0);
    struct symbol_list_t *new_symbol = insert_symbol(global_symbol_table, id->token, type, function);
    if (new_symbol == NULL) {
        printf("Error: identifier already declared: %s\n", id->token);
        semantic_errors++;
    }

    struct symbol_list_t *function_scope = enter_scope(new_symbol);

    // Parameters
    check_parameters(function_scope, parameters);

    // Function body
    struct node_t *function_body = getchild(function, 1);
    check_function_body(function_scope, function_body);
}

void check_parameters(struct symbol_list_t *scope, struct node_t *parameters) {
    int position = 0;
    struct node_t *param = getchild(parameters, position);
    while (param != NULL) {
        enum type_t type = get_type(getchild(param, 0));
        param->type = type;
        struct node_t *id = getchild(param, 1);
        if (insert_symbol(scope, id->token, type, param) == NULL) {
            printf("Error: identifier already declared: %s\n", id->token);
            semantic_errors++;
        }
        position++;
        param = getchild(parameters, position);
    }
}

void check_function_body(struct symbol_list_t *scope, struct node_t *function_body) {
    struct node_list_t *child = function_body->children->next;
    while (child != NULL) {
        struct node_t *node = child->node;
        switch (node->category) {
            case VarDecl:
                check_var(scope, node);
                break;
            default:
                break;
        }
        child = child->next;
    }
}

enum type_t get_type(struct node_t *node) {
    if (node == NULL) return None;
    switch (node->category) {
        case Int:
            return TypeInteger;
        case Float32:
            return TypeFloat32;
        case Bool:
            return TypeBool;
        case String:
            return TypeString;
        default:
            return node->type;
    }
}