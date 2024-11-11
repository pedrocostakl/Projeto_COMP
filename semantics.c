#include "semantics.h"

#include <stdio.h>
#include <string.h>

struct symbol_list_t *insert_symbol(struct symbol_list_t *table, char *identifier, enum type_t type, struct node_t *node) {
    struct symbol_list_t *new = (struct symbol_list_t*)malloc(sizeof(struct symbol_list_t));
    new->identifier = strdup(identifier);
    new->type = type;
    new->node = node;
    new->next = NULL;
    struct symbol_list_t *symbol = table;
    while (symbol != NULL) {
        if (symbol->next == NULL) {
            symbol->next = new;
            break;
        } else if (strcmp(symbol->next->identifier, identifier) == 0) {
            free(new);
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

void show_symbol_table() {
    struct symbol_list_t *symbol = global_symbol_table->next;
    while (symbol != NULL) {
        // Print here
        symbol = symbol->next;
    }
}

/**
 * Análise semântica
 */

static int semantic_errors;
static struct symbol_list_t *global_symbol_table;

static void check_var(struct symbol_list_t *scope, struct node_t *var);
static void check_function(struct symbol_list_t *scope, struct node_t *function);
static void check_parameters(struct symbol_list_t *scope, struct node_t *parameters);
static void check_function_body(struct symbol_list_t *scope, struct node_t *function_body);

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

void check_var(struct symbol_list_t *scope, struct node_t *var)
{
    
}

void check_function(struct symbol_list_t *scope, struct node_t *function)
{
    struct symbol_list_t *function_scope = (struct symbol_list_t*)malloc(sizeof(struct symbol_list_t));
    struct node_t *header = getchild(function, 0);

    // Identifier
    struct node_t *id = getchild(header, 0);
    if (insert_symbol(global_symbol_table, id->token, None, function) == NULL) {
        semantic_errors++;
    }

    // Parameters
    struct node_t *parameters = getchild(header, 1);
    if (parameters->category != FuncParams) {
        parameters = getchild(header, 2);
    }
    check_parameters(global_symbol_table, parameters);

    // Function body
    struct node_t *function_body = getchild(function, 1);
    check_function_body(function_scope, function_body);
}

static void check_parameters(struct symbol_list_t *scope, struct node_t *parameters) {

}

static void check_function_body(struct symbol_list_t *scope, struct node_t *function_body) {

}
