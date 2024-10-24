#include "stdlib.h"
#include "stdio.h"
#include "ast.h"

/*
**  Pedro Sousa da Costa - 2022220304
**  Marco Manuel Almeida e Silva - 2021211653
*/

struct node_t *newnode(enum category_t category, char *token) {
    struct node_t *new = malloc(sizeof(struct node_t));
    new->category = category;
    new->token = token;
    new->children = malloc(sizeof(struct node_list_t));
    new->children->next = NULL;
    new->children->node = NULL;
    return new;
}

void addchild(struct node_t *parent, struct node_t *child) {
    struct node_list_t *children = parent->children;
    while (children->next != NULL) {
        children = children->next;
    }
    struct node_list_t *new = malloc(sizeof(struct node_list_t));
    new->node = child;
    new->next = NULL;
    children->next = new;
}
