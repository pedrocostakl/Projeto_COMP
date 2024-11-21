/*
**  Pedro Sousa da Costa - 2022220304
**  Marco Manuel Almeida e Silva - 2021211653
*/

#include <stdlib.h>
#include <stdio.h>
#include "ast.h"

int show_ast_type = 0;

static int numchildren(struct node_t *root);
static void print_category(const enum category_t category);

struct node_t *newnode(enum category_t category, char *token) {
    struct node_t *new = malloc(sizeof(struct node_t));
    new->category = category;
    new->type = None;
    new->token = token;
    new->children = malloc(sizeof(struct node_list_t));
    new->children->next = NULL;
    new->children->node = NULL;
    return new;
}

void addchild(struct node_t *parent, struct node_t *child) {
    if (child == NULL) return;
    struct node_list_t *children = parent->children;
    while (children->next != NULL) {
        children = children->next;
    }
    if (child->category != Intermediate) {
        struct node_list_t *new = malloc(sizeof(struct node_list_t));
        new->node = child;
        new->next = NULL;
        children->next = new;
    } else {
        children->next = child->children->next;
        free(child);
        free(child->children);
    }
    
}

struct node_t *getchild(struct node_t *parent, int position) {
    struct node_list_t *child = parent->children->next;
    while (child != NULL) {
        if (position == 0) {
            return child->node;
        }
        position--;
        child = child->next;
    }
    return NULL;
}

void show(struct node_t *root, int depth, int forceblock) {
    // controlo do print do node e do seu token
    switch (root->category) {
        case Intermediate:
            break;
        case Block:
            if (forceblock == 0 && numchildren(root) < 2) {
                break; // não continua para o print caso não seja válido
            }
            // se o bloco for válido, continuar para o print
        default:
            for (int i = 0; i < depth; i++) {
                printf("..");
            }
            print_category(root->category);
            if (root->token != NULL) {
                printf("(%s)", root->token);
            }
            if (root->type > None) {
                printf(" - ");
                print_type(root->type);
            }
            printf("\n");
            depth++;
            break;
    }
    // forçar ou não o próximo Block
    if (root->category == If || root->category == For) {
        forceblock = 1;
    } else {
        forceblock = 0;
    }
    // iterar children
    struct node_list_t *children = root->children;
    while (children->next != NULL) {
        show(children->next->node, depth, forceblock);
        children = children->next;
    }
}

void clean(struct node_t *root) {
    if (root == NULL) return;
    struct node_list_t *children = root->children;
    while (children != NULL) {
        struct node_t *node = children->node;
        if (node != NULL) {
            clean(node);
        }
        struct node_list_t *current = children;
        children = children->next;
        free(current);
    }
    if (root->token != NULL) free(root->token);
    free(root);
}

int numchildren(struct node_t *root) {
    int num = 0;
    if (root == NULL) return num;
    struct node_list_t *children = root->children;
    while (children->next != NULL) {
        if (children->next->node->category == Intermediate) {
            num += numchildren(children->next->node);
        } else {
            if (children->next->node->category == Block) {
                // contar um block como child apenas se não estiver vazio
                if (numchildren(children->next->node) > 0) {
                    num++;
                }
            } else {
                num++;
            }
        }
        children = children->next;
    }
    return num;
}

void print_category(const enum category_t category) {
    switch (category) {
        case Program:
            printf("Program");
            return;
        case VarDecl:
            printf("VarDecl");
            return;
        case FuncDecl:
            printf("FuncDecl");
            break;
        case FuncHeader:
            printf("FuncHeader");
            break;
        case FuncParams:
            printf("FuncParams");
            break;
        case FuncBody:
            printf("FuncBody");
            break;
        case ParamDecl:
            printf("ParamDecl");
            break;
        case Block:
            printf("Block");
            break;
        case If:
            printf("If");
            break;
        case For:
            printf("For");
            break;
        case Return:
            printf("Return");
            break;
        case Call:
            printf("Call");
            break;
        case Print:
            printf("Print");
            break;
        case ParseArgs:
            printf("ParseArgs");
            break;
        case Or:
            printf("Or");
            break;
        case And:
            printf("And");
            break;
        case Eq:
            printf("Eq");
            break;
        case Ne:
            printf("Ne");
            break;
        case Lt:
            printf("Lt");
            break;
        case Gt:
            printf("Gt");
            break;
        case Le:
            printf("Le");
            break;
        case Ge:
            printf("Ge");
            break;
        case Add:
            printf("Add");
            break;
        case Sub:
            printf("Sub");
            break;
        case Mul:
            printf("Mul");
            break;
        case Div:
            printf("Div");
            break;
        case Mod:
            printf("Mod");
            break;
        case Not:
            printf("Not");
            break;
        case Minus:
            printf("Minus");
            break;
        case Plus:
            printf("Plus");
            break;
        case Assign:
            printf("Assign");
            break;
        case Int:
            printf("Int");
            break;
        case Float32:
            printf("Float32");
            break;
        case Bool:
            printf("Bool");
            break;
        case String:
            printf("String");
            break;
        case Natural:
            printf("Natural");
            break;
        case Decimal:
            printf("Decimal");
            break;
        case Identifier:
            printf("Identifier");
            break;
        case StrLit:
            printf("StrLit");
            break;
        default:
            break;
    }
}

void print_type(const enum type_t type) {
    switch (type) {
        case None:
            printf("none");
            break;
        case TypeInteger:
            printf("int");
            break;
        case TypeFloat32:
            printf("float32");
            break;
        case TypeBool:
            printf("bool");
            break;
        case TypeString:
            printf("string");
            break;
        default:
            break;
    }
}
