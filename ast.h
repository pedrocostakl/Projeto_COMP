/*
**  Pedro Sousa da Costa - 2022220304
**  Marco Manuel Almeida e Silva - 2021211653
*/

#ifndef AST_H
#define AST_H

enum category_t {

    /* Raiz */
    Program = 0,

    /* Declaração de variáveis */
    VarDecl,

    /* Declaração/definição de funções */
    FuncDecl,
    FuncHeader,
    FuncParams,
    FuncBody,
    ParamDecl,

    /* Statements */
    Block,
    If,
    For,
    Return,
    Call,
    Print,
    ParseArgs,

    /* Operadores */
    Or,
    And,
    Eq,
    Ne,
    Lt,
    Gt,
    Le,
    Ge,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Not,
    Minus,
    Plus,
    Assign,

    /* Terminais */
    Int,
    Float32,
    Bool,
    String,
    Natural,
    Decimal,
    Identifier,
    StrLit,

    /* Nó intermediário que não será imprimido */
    Intermediate

};

enum type_t
{
    Undefined = 0,
    None,
    TypeInteger,
    TypeFloat32,
    TypeBool,
    TypeString
};

struct node_t {
    enum category_t category;
    enum type_t type;
    char *token;
    struct node_list_t *children;
};

struct node_list_t {
    struct node_list_t *next;
    struct node_t *node;
};

struct node_t *newnode(enum category_t category, char *token);
void addchild(struct node_t *parent, struct node_t *child);
struct node_t *getchild(struct node_t *parent, int position);
void show(struct node_t *root, int depth, int forceblock);
void clean(struct node_t *root);

void print_type(const enum type_t type);

#endif /* AST_H */