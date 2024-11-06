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

void addChildBeginning(struct node_t *parent, struct node_t *child) {
    

    struct node_list_t *new = malloc(sizeof(struct node_list_t));
    new->node = child;
    new->next = parent->children;
    parent->children = new;
}

void show(struct node_t *root, int depth) {
    //if (root->category != Intermediate) {
        for (int i = 0; i < depth; i++) {
        printf("..");
        }
        print_category(root->category);
        if (root->token != NULL) {
            printf("(%s)", root->token);
        }
        printf("\n");
        depth += 1;
    //}
    struct node_list_t *children = root->children;
    while (children->next != NULL) {
        if(children->next->node != NULL){
            show(children->next->node, depth);
        }
         children = children->next;
    }
}

int removeIntermediates(struct node_t *root,struct node_list_t *previous, struct node_list_t *next, struct node_t * previous_node){
    int removed = 0;
    //printf("REMOVING(%s)", root->token);
    if (root->category == Intermediate) {
        /*removed = 1;
        if(previous == NULL){
            previous = root->children;
            previous_node->children = previous;
        }
        else{
            previous->next = root->children;
            previous = previous->next;
        }
        while(root->children != NULL){
           root->children = root->children->next;
          
        }
        if(previous != NULL){
            previous->next = next;
        }
        else{
            previous_node->children = next;
        }*/
       
    }
    struct node_list_t *children = root->children;
    struct node_list_t * next_previous = NULL;
    while (children->next != NULL) {
        int was_removed = 0;
        if(children->next->node != NULL){
             //was_removed = removeIntermediates(children->next->node, next_previous, children->next->next, root);
        }
        //next_previous = children;
        if(was_removed == 0){
            children = children->next;
        }
        
    }
    return removed;
}

void remove_intermediate_nodes(struct node_t *node) {
    if (node == NULL) return;  // Check if the node itself is NULL

    struct node_list_t *prev = NULL;
    struct node_list_t *current = node->children;

    while (current != NULL) {
        if (current->node && current->node->category == Intermediate) {
            // Save the children of the Intermediate node
            struct node_list_t *intermediate_children = current->node->children;
            
            if (prev == NULL) {
                // If the Intermediate node is the first child
                node->children = intermediate_children;
            } else {
                // Link the previous node to the children of Intermediate
                prev->next = intermediate_children;
            }

            // Link the last child of Intermediate's children to current->next
            struct node_list_t *last_child = intermediate_children;
            while (last_child && last_child->next != NULL) {
                last_child = last_child->next;
            }
            if (last_child) {
                last_child->next = current->next;
            }

            // Free the Intermediate node and move to the next
            struct node_list_t *to_free = current;
            current = current->next;
            free(to_free->node);  // Free the Intermediate node
            free(to_free);        // Free the list node
        } else {
            // Move to the next child
            prev = current;
            current = current->next;
        }
    }

    // Recursively apply this function to each child node
    struct node_list_t *child = node->children;
    while (child != NULL) {
        remove_intermediate_nodes(child->node);
        child = child->next;
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
