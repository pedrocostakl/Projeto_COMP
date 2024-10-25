#include "stdlib.h"
#include "stdio.h"
#include "ast.h"

/*
**  Pedro Sousa da Costa - 2022220304
**  Marco Manuel Almeida e Silva - 2021211653
*/

void print_category(enum category_t category);

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

void show(struct node_t *root, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("..");
    }
    print_category(root->category);
    printf("\n");
    struct node_list_t *children = root->children;
    while (children->next != NULL) {
        show(children->next->node, depth + 1);
        children = children->next;
    }
}

void print_category(enum category_t category) {
    switch (category) {
        case Program:
        printf("Program");
        return;
        case VarDecl:
        printf("VarDecl");
        return;
    }
}
