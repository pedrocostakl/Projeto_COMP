/*
**  Pedro Sousa da Costa - 2022220304
**  Marco Manuel Almeida e Silva - 2021211653
*/

#include "semantics.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUF_SIZE 160000
int notIncorrectlyApplied = 0;
int dontCheckIncorrectType = 0; //SE TEMOS ERROS NO FOR, NÃO VERFICAMOS SE O FILHO É BOOL   
int  unaryError = 0; 
int checkErrors = 0; //Flag to check and print errors
int semantic_errors;
struct symbol_list_t *global_symbol_table;


static void show_function(struct symbol_list_t *symbol, struct node_t *node);
static int is_parameter(struct node_t *node, struct node_t *function);

struct symbol_list_t *insert_symbol(struct symbol_list_t *table, char *identifier, enum type_t type, struct node_t *node) {
    struct symbol_list_t *new = NULL;
    struct symbol_list_t *symbol = table;
    while (symbol != NULL) {
       
        if (symbol->next == NULL) {
            new = (struct symbol_list_t*)malloc(sizeof(struct symbol_list_t));
            new->identifier = strdup(identifier);
            new->type = type;
            new->node = node;
            new->next = NULL;
            new->scope = NULL;
            symbol->next = new;
            new->used = 0; // O símbolo ainda não foi usado
            break;
        }
        else if (strcmp(symbol->next->identifier, identifier) == 0) {
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
struct symbol_list_t *create_symbol_list() {
    struct symbol_list_t *new_list = malloc(sizeof(struct symbol_list_t));
    new_list->identifier = NULL;
    new_list->type = None;
    new_list->node = NULL;
    new_list->next = NULL;
    return new_list;
}
void free_symbol_list(struct symbol_list_t *list) {
    while (list != NULL) {
        struct symbol_list_t *next = list->next;
        free(list->identifier); // Free the identifier string
        free(list);             // Free the symbol node
        list = next;
    }
}
struct symbol_list_t *enter_scope(struct symbol_list_t *table) {
    if (table == NULL) return NULL;
    table->scope = (struct symbol_list_t*)malloc(sizeof(struct symbol_list_t));
    return table->scope;
}
struct symbol_list_t *search_symbol_in_list(struct symbol_list_t *list, const char *identifier) {
    while (list != NULL) {
        if (list->identifier != NULL && strcmp(list->identifier, identifier) == 0) {
            return list; // Found
        }
        list = list->next;
    }
    return NULL; // Not found
}

int semantic_errors;
struct symbol_list_t *global_symbol_table;

static void show_function(struct symbol_list_t *symbol, struct node_t *node);
static int is_parameter(struct node_t *node, struct node_t *function);

const char *get_operator_token(enum category_t category) { //função para auxiliar no print de erros que involvem operators
    switch (category) {
        case Or: return "||";
        case And: return "&&";
        case Eq: return "==";
        case Ne: return "!=";
        case Lt: return "<";
        case Gt: return ">";
        case Le: return "<=";
        case Ge: return ">=";
        case Add: return "+";
        case Sub: return "-";
        case Mul: return "*";
        case Div: return "/";
        case Mod: return "%";
        case Not: return "!";
        case Minus: return "-";
        case Plus: return "+";
        case Assign: return "=";
        case ParseArgs: return "strconv.Atoi";
        default: return "<unknown>";
    }
}
//Para detetar símbolos que não foram usados

void report_unused_symbols(struct symbol_list_t *symbol_table, int is_global) {
    if (symbol_table == NULL) {
        //fprintf(stderr, "Error: symbol_table is NULL\n");
        return;
    }

    struct symbol_list_t *symbol = symbol_table->next;
    while (symbol != NULL) {
        if (symbol->node != NULL) {
            //printf("Checking symbol: %s\n", symbol->identifier);

            if (symbol->node != NULL &&
            symbol->used == 0 &&
            symbol->node->category != FuncDecl ) {

            // Report unused symbols
            struct node_t *id_node = getchild(symbol->node, 1);
            printf("Line %d, column %d: Symbol %s declared but never used\n",
                   id_node->line, id_node->column, symbol->identifier);
                   //printf("%d\n",symbol->node->category);
        }
        } else {
            //printf("Skipping symbol with NULL node.\n");
        }

        if (symbol->scope != NULL) {
            //printf("Entering scope of symbol: %s\n", symbol->identifier);
            report_unused_symbols(symbol->scope, 0);
        }

        symbol = symbol->next;
    }
}



void show_symbol_table() {
    struct symbol_list_t *symbol = global_symbol_table->next;
    printf("===== Global Symbol Table =====\n");
    while (symbol != NULL) {
        switch (symbol->node->category) {
            case FuncDecl:{
                printf("%s\t(", symbol->identifier);
                print_parameters(symbol->node);
                printf(")\t");
                print_type(symbol->type);
                printf("\n");
                break;}
            case VarDecl:{
                printf("%s\t\t", symbol->identifier);
                print_type(symbol->type);
                printf("\n");
                break;}
            default:
                break;
        }
        symbol = symbol->next;
    }
    printf("\n");

    symbol = global_symbol_table->next;
    while (symbol != NULL) {
        if (symbol->node->category == FuncDecl) {
            show_function(symbol, symbol->node);
        }
        symbol = symbol->next;
    }
}
const char *get_type_string(enum type_t type) {
    switch (type) {
        case TypeInteger: return "int";
        case TypeFloat32: return "float32";
        case TypeBool: return "bool";
        case TypeString: return "string";
        case Undefined: return "undef";
        default: return "none";
    }
}

void print_parameters(struct node_t *node) {
    struct node_t *header = getchild(node, 0);
    struct node_t *parameters = getchild(header, 1);
    if (parameters->category != FuncParams) {
        parameters = getchild(header, 2);
    }
    struct node_list_t *children = parameters->children->next;
    while (children != NULL) {
        struct node_list_t *next = children->next;
        print_type(children->node->type);
        if (next != NULL) {
            printf(",");
        }
        children = next;
    }
}

void show_function(struct symbol_list_t *symbol, struct node_t *node) {
    //printf("===== Function %s() Symbol Table =====\n", symbol->identifier);
    printf("===== Function %s(", symbol->identifier);
    print_parameters(symbol->node);
    printf(") Symbol Table =====\n");

    printf("return\t\t");
    print_type(symbol->type);
    printf("\n");

    struct symbol_list_t *scope_table = symbol->scope->next;
    while (scope_table != NULL) {
        printf("%s\t\t", scope_table->identifier);
        print_type(scope_table->type);
        if (is_parameter(scope_table->node, node) == 1) {
            printf("\tparam\n");
        } else {
            printf("\n");
        }
        scope_table = scope_table->next;
    }

    printf("\n");
}

int is_parameter(struct node_t *node, struct node_t *function) {
    if (node == NULL || function == NULL) return 0;

    struct node_t *header = getchild(function, 0);
    if (header == NULL) return 0;

    struct node_t *parameters = getchild(header, 1);
    if (parameters->category != FuncParams || parameters == NULL) {
        parameters = getchild(header, 2);
        if (parameters == NULL) return 0;
    }

    struct node_list_t *children = parameters->children->next;
    while (children != NULL) {
        if (children->node == node) return 1;
        children = children->next;
    }
    return 0;
}

/**
 * Análise semântica
 */

static void check_var(struct symbol_list_t *scope, struct node_t *var);
static void check_function(struct symbol_list_t *scope, struct node_t *function, int checkBody);
static void check_parameters(struct symbol_list_t *scope, struct node_t *parameters);
static void check_function_body(struct symbol_list_t *scope, struct node_t *function_body,  enum type_t function_return_type);
static void check_statement(struct symbol_list_t *scope, struct node_t *parent,  enum type_t function_return_type);
static void check_expressions(struct symbol_list_t *scope, struct node_t *parent, int callFunction);//1-se é call

static enum type_t get_type(struct node_t *node);

int check_program(struct node_t *program) {
    global_symbol_table = (struct symbol_list_t*)malloc(sizeof(struct symbol_list_t));
    global_symbol_table->next = NULL;
    struct node_list_t *child = program->children->next;
    while (child != NULL) {
        struct node_t *node = child->node;
        switch (node->category) {
            case VarDecl:{
                check_var(global_symbol_table, node);
                break;
            }
            case FuncDecl:{
                check_function(global_symbol_table, node,0);
                break;
            }
            default:
                break;
        }
        child = child->next;
    }
    child = program->children->next;
    while (child != NULL) {
        struct node_t *node = child->node;
        switch (node->category) {
            case FuncDecl:{
                check_function(global_symbol_table, node,1);
                break;
            }
            default:
                break;
        }
        child = child->next;
    }

    // Report unused symbols (global and recursively for all scopes)
    //report_unused_symbols(global_symbol_table, 1);
    return semantic_errors;
}

void check_var(struct symbol_list_t *scope, struct node_t *var) {
    struct node_t *id = getchild(var, 1);
    enum type_t type = get_type(getchild(var, 0));
    if (insert_symbol(scope, id->token, type, var) == NULL) {
        printf("Line %d, column %d: Symbol %s already defined\n", id->line, id->column, id->token);
        semantic_errors++;
    }
}

void check_function(struct symbol_list_t *scope, struct node_t *function, int checkBody) {
    struct node_t *header = getchild(function, 0);
   
    

    // Type and parameters node
    enum type_t type = None;
    struct node_t *parameters = getchild(header, 1);
    if (parameters != NULL && parameters->category != FuncParams) {
        type = get_type(parameters);
        parameters = getchild(header, 2);
    }
    struct node_t *id; 
    struct symbol_list_t *new_symbol;
    id = getchild(header, 0);

    // If the function body should be skipped, only check if the function already exists
    if(checkBody == 0){
        // Insert Identifier
        new_symbol = insert_symbol(global_symbol_table, id->token, type, function);
        if (new_symbol == NULL) {
            printf("Line %d, column %d: Symbol %s already defined\n", id->line, id->column, id->token);
            semantic_errors++;
            return; // Skip further processing since the function is already defined
        }
    }
    else{
        new_symbol = search_symbol(global_symbol_table, id->token);
        if (new_symbol != NULL && new_symbol->node != function) {
            // Function already defined, skip the body
            //printf("Line %d, column %d: Skipping redefinition of function %s\n", id->line, id->column, id->token);
            return;
        }
    }
    

    struct symbol_list_t *function_scope = enter_scope(new_symbol);

    // Parameters should only be checked when processing the function body (checkBody == 1)
    if (parameters != NULL && checkBody == 1) {
        check_parameters(function_scope, parameters);
    }
    

    // Get function return type
    struct node_t *return_type_node = getchild(header, 1);
    enum type_t function_return_type = get_type(return_type_node);
    // Function body
    struct node_t *function_body = getchild(function, 1);
    if (function_body != NULL && checkBody == 1) {
        check_function_body(function_scope, function_body, function_return_type);
    }

    // Report unused symbols in the function's local scope
    if(checkBody == 1)
        report_unused_symbols(function_scope, 0);
}


void check_parameters(struct symbol_list_t *scope, struct node_t *parameters) {
    int position = 0;
    struct node_t *param = getchild(parameters, position);
    while (param != NULL) {
        enum type_t type = get_type(getchild(param, 0));
        param->type = type;
        struct node_t *id = getchild(param, 1);
        struct symbol_list_t *symbol = insert_symbol(scope, id->token, type, param);
        if (symbol == NULL) {
            printf("Line %d, column %d: Symbol %s already defined\n", id->line, id->column, id->token);
            semantic_errors++;
        } else {
            symbol->used = 1; // Mark parameter as used
        }
        position++;
        param = getchild(parameters, position);
    }
    
}

void check_function_body(struct symbol_list_t *scope, struct node_t *function_body, enum type_t function_return_type) {
    struct node_list_t *child = function_body->children->next;
    while (child != NULL) {
        struct node_t *node = child->node;
        switch (node->category) {
            case VarDecl:
                check_var(scope, node);
                break;
            case Block:
            case If: {
                struct node_t *condition = getchild(node, 0);
                struct node_t *block1 = getchild(node, 1);
                struct node_t *block2 = getchild(node, 2);
                check_expressions(scope, condition, 0);
                check_statement(scope, block1, function_return_type);
                check_statement(scope, block2, function_return_type);
                break;
            }
            case For:
            case Return:
                check_statement(scope, node, function_return_type);
                break;
            case Print:
            case ParseArgs:
            case Assign:
                check_statement(scope, node, function_return_type);
                break;
            case Call:
            case Or:
            case And:
            case Eq:
            case Ne:
            case Lt:
            case Gt:
            case Le:
            case Ge:
            case Add:
            case Sub:
            case Mul:
            case Div:
            case Mod:
            case Not:
            case Minus:
            case Plus:
                check_expressions(scope, node, 0);
                break;
            default:
                break;
        }
        child = child->next;
    }
}


void check_statement(struct symbol_list_t *scope, struct node_t *parent, enum type_t function_return_type) {
    switch (parent->category) {
        case Block: {
            struct node_list_t *children = parent->children->next;
            while (children != NULL) {
                check_statement(scope, children->node, function_return_type);
                check_expressions(scope, children->node, 0);
                children = children->next;
            }
            break;
        }
        case Return: {
            struct node_t *return_expr = getchild(parent, 0);
            /*printf("Parent of return: categorY %d type ",parent->category);
            print_type(parent->type);
            printf("/n");*/
            if (return_expr != NULL) {
                check_expressions(scope, return_expr, 0);
                if (return_expr->type != function_return_type) {
                    printf("Line %d, column %d: Incompatible type ",
                           return_expr->line, return_expr->column);
                    print_type(return_expr->type);
                    printf(" in return statement");
                    //print_type(function_return_type);
                    printf("\n");
                    semantic_errors++;
                }
            } else {
                // Handle case where return statement is empty
                if (function_return_type != None) {
                    printf("Line %d, column %d: Incompatible type void in return statement",
                           parent->line, parent->column);
                    //print_type(function_return_type);
                    printf("\n");
                    semantic_errors++;
                }
            }
            break;
        }
        case If: {
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            struct node_t *else_block = getchild(parent, 2);
            if (node2 != NULL) {
                check_expressions(scope, node1,0);
                check_statement(scope, node2,function_return_type);
                if (else_block != NULL) {
                    check_statement(scope, else_block, function_return_type);
                }
                if(node1->type != TypeBool){
                    if(dontCheckIncorrectType == 0 && notIncorrectlyApplied == 0){
                    printf("Line %d, column %d: Incompatible type ",  node1->line, node1->column);
                    print_type(node1->type);
                    printf(" in if statement");//FAZER FUNCAO PARA BUSCAR CATEGORY
                    printf("\n");
                     parent->type = Undefined;
                    semantic_errors++;
                    }
                    parent->type = Undefined;
                    semantic_errors++;
                }
               
            } else {
                check_statement(scope, node1, function_return_type);
            }
        break;
        }
        case For: {
            struct node_t *node1 = getchild(parent, 0);
                struct node_t *node2 = getchild(parent, 1);
                if (node2 != NULL) {
                    check_expressions(scope, node1,0);
                    check_statement(scope, node2, function_return_type);
                    if(node1->type != TypeBool){
                        if(dontCheckIncorrectType == 0 && notIncorrectlyApplied == 0 && node1->errorOccurred == 0){
                            printf("Line %d, column %d: Incompatible type ",node1->line, node1->column);
                            print_type(node1->type);
                            printf(" in for statement");
                            printf("\n");
                            parent->type = Undefined;
                            semantic_errors++;
                        }
                        parent->type = Undefined;
                        semantic_errors++;
                    }
                } else {
                    check_statement(scope, node1, function_return_type);
                }
            break;
        }
        case Print: {
                struct node_t *node = getchild(parent, 0);
                check_expressions(scope, node,0);
                if(node->type == Undefined){ //só imprime expr válidas
                    printf("Line %d, column %d: Incompatible type ",node->line, node->column);
                    print_type(node->type);
                    printf(" in fmt.Println statement");
                    printf("\n");
                    parent->type = Undefined;
                    semantic_errors++;
                }
            } break;
        case ParseArgs: {
                struct node_t *node1 = getchild(parent, 0);
                struct node_t *node2 = getchild(parent, 1);
                check_expressions(scope, node1,0);
                check_expressions(scope, node2,0);
                if (node1->type == node2->type && node1->type == TypeInteger) {
                    parent->type = node1->type;
                } else {
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    parent->type = Undefined;
                }
            } break;
        case Assign: {
                /*print_type(parent->type);
                printf("\n");*/
                struct node_t *node1 = getchild(parent, 0);
                struct node_t *node2 = getchild(parent, 1);
                check_expressions(scope, node1,0);
                check_expressions(scope, node2,0);
                // Skip type checking if a prior error occurred in the assignment expression
                if (node1->errorOccurred || node2->errorOccurred) {
                    parent->errorOccurred = 1;
                    parent->type = node1->type;
                    break;
                }

                if ((node1->type == node2->type)) {
                    parent->type = node1->type;
                    if(node1->type == Undefined){
                        printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                        parent->line, parent->column, get_operator_token(parent->category));
                        print_type(node1->type);
                        printf(", ");
                        print_type(node2->type);
                        printf("\n");
                        parent->type = node1->type;
                        semantic_errors++;
                    }
                } else {
                    //if(node1->errorOperationOccurred)
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    parent->type = node1->type;
                    semantic_errors++;
                }
            } break;
        default:
            break;
    }
}

void check_expressions(struct symbol_list_t *scope, struct node_t *parent, int callFunction) {
    notIncorrectlyApplied = 0; //to prevent printing in case of h(!i) BOOLEANS
    dontCheckIncorrectType = 0;
    unaryError = 0; //If a unaryboprator is incorrectly applied, we don't print the cannot find symbol message.
    //printf("Checking: %s\n", parent->token);
    switch (parent->category) {
        case Call: {
            struct node_list_t *children = parent->children->next;

            // The first child is the function identifier
            struct node_t *function_id = children->node;
            struct symbol_list_t *symbol = search_symbol(global_symbol_table, function_id->token);
            
            // Prepare the error message
            char error_message[BUF_SIZE] = {0};
            snprintf(error_message, sizeof(error_message), "Line %d, column %d: Cannot find symbol %s(",
                    function_id->line, function_id->column, function_id->token);

            // Collect argument types
            struct node_list_t *arg_list = children->next;
            int first_argument = 1; // Formatting flag
            while (arg_list != NULL) {
                struct node_t *arg = arg_list->node;
                check_expressions(scope, arg, 0); // Validate the argument

                if (!first_argument) {
                    strncat(error_message, ",", sizeof(error_message) - strlen(error_message) - 1);
                } else {
                    first_argument = 0;
                }
                strncat(error_message, get_type_string(arg->type), sizeof(error_message) - strlen(error_message) - 1);

                arg_list = arg_list->next;
            }
            strncat(error_message, ")", sizeof(error_message) - strlen(error_message) - 1);

            // Check if the function exists
            if ((symbol == NULL || symbol->node->category != FuncDecl) && notIncorrectlyApplied == 0) {
                /*printf("Line: %d Column:%d notIncorrectlyApplied = %d\n", function_id->line, function_id->column,notIncorrectlyApplied);*/
                printf("%s\n", error_message);
                parent->type = Undefined;
                function_id->type = Undefined;
                parent->errorOperationOccurred = 1;
                semantic_errors++;
                return;
            }

            // Match argument types with function parameters
            struct node_t *func_header = getchild(symbol->node, 0);
            struct node_t *func_params = getchild(func_header, 1);
            if (func_params->category != FuncParams) {
                func_params = getchild(func_header, 2);
            }

            struct node_list_t *param_list = func_params->children->next;
            arg_list = children->next; // Reset argument list
            int mismatch = 0;

            while (arg_list != NULL || param_list != NULL) {
                struct node_t *arg = arg_list ? arg_list->node : NULL;
                struct node_t *param = param_list ? param_list->node : NULL;

                if (arg == NULL || param == NULL) {
                    mismatch = 1;
                    break; // Argument count mismatch
                }

                check_expressions(scope, arg, 0);
                if (arg->type != param->type) {
                    mismatch = 1;
                }

                arg_list = arg_list->next;
                param_list = param_list->next;
            }

            if (mismatch && notIncorrectlyApplied == 0) {
                //printf("Line: %d Column:%d notIncorrectlyApplied = %d\n", function_id->line, function_id->column,notIncorrectlyApplied);
                printf("%s\n", error_message);
                parent->type = Undefined;
                function_id->type = Undefined;
                semantic_errors++;
                parent->errorOperationOccurred = 1;
            } else {
                parent->type = symbol->type;
            }

            // Mark the function as used
            symbol->used = 1;
            break;
        }

        case Natural: {
            parent->type = TypeInteger;
            break;}
        case Decimal: {
            parent->type = TypeFloat32;
            break;
        }
        case Identifier: {
            struct symbol_list_t *symbol = search_symbol(scope, parent->token);
            if (symbol == NULL) {
                symbol = search_symbol(global_symbol_table, parent->token);   
            }
            if (symbol != NULL) {
                if (symbol->node->category == FuncDecl && callFunction == 0) {
                    // If it's a function being used improperly
                    printf("Line %d, column %d: Cannot find symbol %s\n", parent->line, parent->column, parent->token);
                    semantic_errors++;
                    parent->type = Undefined; // Mark the type as Undefined for consistency
                } else {
                    parent->type = symbol->type;
                    symbol->used = 1; // Símbolo foi usado
                }
            } else {
                // Erro: Undefined type
                if(callFunction == 1){
                    printf("Line %d, column %d: Cannot find symbol %s()\n", parent->line, parent->column, parent->token);
                }
                else{
                     printf("Line %d, column %d: Cannot find symbol %s\n", parent->line, parent->column, parent->token);
                }
                semantic_errors++;
                parent->type = Undefined;
            }
            break;
        }
        case StrLit: {
            parent->type = TypeString;
            break;
        }
        case Or:{
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);

             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            parent->type = TypeBool;
            if (node1->type == node2->type &&
                (node1->type == TypeBool)) {
                
            } else {
                if(parent->errorOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                }
                semantic_errors++;
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
               
            }
         break;}
        case And: {
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);

             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            parent->type = TypeBool;
            if (node1->type == node2->type &&
                (node1->type == TypeBool)) {
               
            } else {
                if(parent->errorOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    
                }
                semantic_errors++;
                parent->errorOccurred = 1;
                dontCheckIncorrectType = 1;
                notIncorrectlyApplied = 1;
                
               
            }
         break;}
        case Eq: {
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);

             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            parent->type = TypeBool;
            if (node1->type == node2->type) {
                
            } else {
                if(parent->errorOccurred){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                     semantic_errors++;
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
                }
                semantic_errors++;
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
               
            }
            break;
        }
        case Ne: {
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);

             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            parent->type = TypeBool;
            if (node1->type == node2->type) {
               
            } else {
                if(parent->errorOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    semantic_errors++;
                    
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
                }
                semantic_errors++;
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
            }
            break;}
        case Lt:{
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);

             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            parent->type = TypeBool;
            if (node1->type == node2->type &&
                (node1->type == TypeInteger || node1->type == TypeFloat32)) {
               
            } else {
                if(parent->errorOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    semantic_errors++;
                    
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
                }
                semantic_errors++;
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
            }
         break;}
        case Gt:{
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);

             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            parent->type = TypeBool;
            if (node1->type == node2->type &&
                (node1->type == TypeInteger || node1->type == TypeFloat32)) {
                
            } else {
                if(parent->errorOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    semantic_errors++;
                    
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
                }
                semantic_errors++;
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
            }
         break;}
        case Le:{
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);

             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            parent->type = TypeBool;
            if (node1->type == node2->type &&
                (node1->type == TypeInteger || node1->type == TypeFloat32)) {
                
            } else {
                if(parent->errorOccurred == 0){
                     printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    semantic_errors++;
                   
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
                }
                semantic_errors++;
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
            }
         break;}
        case Ge: {
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);

             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }

            parent->type = TypeBool;
            if (node1->type == node2->type &&
                (node1->type == TypeInteger || node1->type == TypeFloat32)) {
                
            } else {
                if(parent->errorOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    
                    semantic_errors++;
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
                }
                
                    semantic_errors++;
                    parent->errorOccurred = 1;
                    dontCheckIncorrectType = 1;
                    notIncorrectlyApplied = 1;
                
            }
         break;}
        case Add:{
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);

             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            if (node1->type == node2->type &&
                (node1->type == TypeInteger || node1->type == TypeFloat32)) {
                if(node1->type== TypeInteger){
                    parent->type = TypeInteger;
                }
                else{
                    parent->type = TypeFloat32;
                }
            } else {
                if(parent->errorOperationOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                    parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    
                    semantic_errors++;
                    parent->type = Undefined;
                    dontCheckIncorrectType = 1;
                    parent->errorOperationOccurred = 1;
                }
                
            }
            break;}
        case Sub:{
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);
            if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL &&symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            if (node1->type == node2->type &&
                (node1->type == TypeInteger || node1->type == TypeFloat32)) {
                if(node1->type== TypeInteger){
                    parent->type = TypeInteger;
                }
                else{
                    parent->type = TypeFloat32;
                }
            } else {
                 if(parent->errorOperationOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                        parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    semantic_errors++;
                    parent->type = Undefined;
                    parent->errorOperationOccurred = 1;
                    dontCheckIncorrectType = 1;
                 }
            }
            break;}
        case Mul:{
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);
             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            if (node1->type == node2->type &&
                (node1->type == TypeInteger || node1->type == TypeFloat32)) {
                if(node1->type== TypeInteger){
                    parent->type = TypeInteger;
                }
                else{
                    parent->type = TypeFloat32;
                }
            } else {
                 if(parent->errorOperationOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                        parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    semantic_errors++;
                    parent->type = Undefined;
                    parent->errorOperationOccurred = 1;
                    dontCheckIncorrectType = 1;
                 }
            }
            break;}
        case Div:{
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);
             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }
            if (node1->type == node2->type &&
                (node1->type == TypeInteger || node1->type == TypeFloat32)) {
                if(node1->type== TypeInteger){
                    parent->type = TypeInteger;
                }
                else{
                    parent->type = TypeFloat32;
                }
            } else {
                 if(parent->errorOperationOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                        parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    semantic_errors++;
                    parent->type = Undefined;
                    parent->errorOperationOccurred = 1;
                    dontCheckIncorrectType = 1;
                 }
            }
            break;}
        case Mod: {
            struct node_t *node1 = getchild(parent, 0);
            struct node_t *node2 = getchild(parent, 1);
            check_expressions(scope, node1,0);
            check_expressions(scope, node2,0);
             if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
             if(node2->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node2->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node2->type = Undefined;
                }
            }

            if (node1->type == node2->type &&
                (node1->type == TypeInteger || node1->type == TypeFloat32)) {
                parent->type = node1->type;
            } else {
                 if(parent->errorOperationOccurred == 0){
                    printf("Line %d, column %d: Operator %s cannot be applied to types ", 
                        parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf(", ");
                    print_type(node2->type);
                    printf("\n");
                    parent->type = Undefined;
                    semantic_errors++;
                    parent->errorOperationOccurred = 1;
                    dontCheckIncorrectType = 1;
                 }
            }
            break;
        }
        case Not: {
            struct node_t *node1 = getchild(parent, 0);
            check_expressions(scope, node1,0);
            if(node1->category == Identifier){
                struct symbol_list_t *symbol = search_symbol(global_symbol_table, node1->token);
                if(symbol != NULL && symbol->node->category == FuncDecl){
                    node1->type = Undefined;
                }
            }
            parent->type = TypeBool;
             if (node1->type!=TypeBool) {
                if (notIncorrectlyApplied==0 && parent->errorOccurred == 0) { // Only log the error once
                    printf("Line %d, column %d: Operator %s cannot be applied to type ", 
                        parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node1->type);
                    printf("\n");
                    semantic_errors++;
                    notIncorrectlyApplied = 1;// Flag to suppress related errors
                    
                    parent->errorOccurred = 1;
                }
                notIncorrectlyApplied = 1;// Flag to suppress related errors
                parent->errorOccurred = 1;
            }
        break;}
        case Minus:
        case Plus: {
            struct node_t *node = getchild(parent, 0);
            check_expressions(scope, node,0);
            if(node->type == TypeFloat32 || node->type == TypeInteger){
                parent->type = node->type;
            }
            else{
                printf("Line %d, column %d: Operator %s cannot be applied to type ", 
                        parent->line, parent->column, get_operator_token(parent->category));
                    print_type(node->type);
                    printf("\n");
                    parent->type = Undefined;
                    semantic_errors++;
                    parent->errorOperationOccurred = 1;
                    dontCheckIncorrectType = 1;
            }
        break;}
 
        default:
            break;
    }
}

enum type_t get_type(struct node_t *node) {
    if (node == NULL) return None;
    switch (node->category) {
        case Int:
            return TypeInteger;
        case Float32:
            return TypeFloat32;
        case Bool:
            return TypeBool;
        case String:
            return TypeString;
        default:
            return node->type;
    }
}
