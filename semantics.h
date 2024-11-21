/*
**  Pedro Sousa da Costa - 2022220304
**  Marco Manuel Almeida e Silva - 2021211653
*/

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
void print_parameters(struct node_t *node);
void show_symbol_table();

int check_program(struct node_t *program);

#endif /* SEMANTICS_H */