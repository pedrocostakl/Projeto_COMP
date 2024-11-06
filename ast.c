#include "stdlib.h"
#include "stdio.h"
#include "ast.h"

/*
**  Pedro Sousa da Costa - 2022220304
**  Marco Manuel Almeida e Silva - 2021211653
*/

void print_category(const enum category_t category);

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

int numchildren(struct node_t *root) {
    int num = 0;
    struct node_list_t *children = root->children;
    while (children->next != NULL) {
        if (children->next->node->category == Intermediate) {
            num += numchildren(children->next->node);
        } else {
            num++;
        }
        children = children->next;
    }
    return num;
}

void show(struct node_t *root, int depth, int force) {
    // controlo do print do node e do seu token
    switch (root->category) {
        case Intermediate:
            break;
        case Block:
            if (force == 0 && numchildren(root) < 2) {
                break;
            }
        default:
            for (int i = 0; i < depth; i++) {
                printf("..");
            }
            depth++;
            print_category(root->category);
            if (root->token != NULL) {
                if (root->category != StrLit) {
                    printf("(%s)", root->token); // print do valor do token
                } else {
                    printf("(\"%s\")", root->token); // print de strlit com \"
                }
            }
            printf("\n");
            break;
    }
    // forçar ou não o próximo Block
    if (root->category == If || root->category == For) {
        force = 1;
    } else {
        force = 0;
    }
    // iterar children
    struct node_list_t *children = root->children;
    while (children->next != NULL) {
        show(children->next->node, depth, force);
        children = children->next;
    }
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
