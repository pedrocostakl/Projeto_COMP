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
    if (root->token) {
        printf("(%s)", root->token);
    }
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
