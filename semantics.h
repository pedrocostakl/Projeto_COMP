#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "ast.h"

struct symbol_list_t {
	char *identifier;
	enum type_t type;
	struct node_t *node;
	struct symbol_list_t *next;
	struct symbol_list_t *scope;
};

struct symbol_list_t *insert_symbol(struct symbol_list_t *table, char *identifier, enum type_t type, struct node_t *node);
struct symbol_list_t *search_symbol(struct symbol_list_t *table, char *identifier);
struct symbol_list_t *enter_scope(struct symbol_list_t *table);
void show_symbol_table();

int check_program(struct node_t *program);

#endif /* SEMANTICS_H */