#pragma once

/*
**  Pedro Sousa da Costa - 2022220304
**  Marco Manuel Almeida e Silva - 2021211653
*/

enum category {

    /* Raiz */
    Program,

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
    Call,

    /* Terminais */
    Int,
    Float32,
    Bool,
    String,
    Natural,
    Decimal,
    Identifier,
    StrLit,

} category_t;

struct node_t {
    enum category_t category;
    char *token;
    struct node_list_t *children;
};

struct node_list_t {
    struct node_list *next;
    struct node_t *node;
};

struct node_t *newnode(enum category_t category, char *token);
void addchild(struct node_t *parent, struct node_t *child);
