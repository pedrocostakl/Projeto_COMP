#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "ast.h"

struct symbol_list_t {
	char *identifier;
	enum type_t type;
	struct node_t *node;
	struct symbol_list_t *next;
};

struct symbol_list_t *insert_symbol(struct symbol_list_t *table, char *identifier, enum type_t type, struct node_t *node);
struct symbol_list_t *search_symbol(struct symbol_list_t *table, char *identifier);
void show_symbol_table();
int check_program(struct node_t *program);

#endif /* SEMANTICS_H */