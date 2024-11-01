%{

    /*
    **  Pedro Sousa da Costa - 2022220304
    **  Marco Manuel Almeida e Silva - 2021211653
    */

    #include <stdio.h>
    #include "ast.h"

    extern int yylex(void);
    void yyerror(char *);

    extern char *yytext;
    extern int line;                 /* Extern line variable from lex file */
    extern int tok_column;           /* Extern token column variable from lex file */

    struct node_t *program;
    struct node_t *type;
    struct node_t *declarations;

%}

%token SEMICOLON
%token COMMA
%token BLANKID
%token ASSIGN
%token STAR
%token DIV
%token MINUS
%token PLUS
%token EQ
%token GE
%token GT
%token LBRACE
%token LE
%token LPAR
%token LSQ
%token LT
%token MOD
%token NE
%token NOT
%token AND
%token OR
%token RBRACE
%token RPAR
%token RSQ
%token PACKAGE
%token RETURN
%token ELSE
%token FOR
%token IF
%token VAR
%token INT
%token FLOAT32
%token BOOL
%token STR
%token PRINT
%token PARSEINT
%token FUNC
%token CMDARGS
%token RESERVED

%token<lexeme> IDENTIFIER STRLIT NATURAL DECIMAL

%type<node> program type declarations var_declaration var_spec func_declaration func_header parameters parameter
%type<node> func_body vars_statements statement

%left LOW
%left '+' '-'
%left '/' '*'
%left HIGH

%union {
    char *lexeme;
    struct node_t *node;
}

%%

program
: PACKAGE IDENTIFIER SEMICOLON declarations
{
    $$ = program = newnode(Program, NULL);
    addchild(program, $4);
}
| PACKAGE IDENTIFIER SEMICOLON
{
    $$ = program = newnode(Program, NULL);
}
;

declarations
: var_declaration SEMICOLON declarations
{
    $$ = declarations;
}
| var_declaration SEMICOLON
{
    $$ = declarations;
}
| declarations SEMICOLON func_declaration
{
    $$ = declarations;
}
| func_declaration SEMICOLON
{
    $$ = declarations;
}
;

var_declaration
: VAR var_spec
{
    addchild(declarations, $2);
}
| VAR LPAR var_spec SEMICOLON RPAR
{
    addchild(declarations, $3);
}
;

var_spec
: IDENTIFIER type
{
    if (declarations == NULL) {
        declarations = newnode(Intermediate, NULL);
    }

    $$ = newnode(VarDecl, NULL);
    addchild($$, $2);
    addchild($$, newnode(Identifier, $1));
}
| IDENTIFIER COMMA var_spec
{
    if (declarations == NULL) {
        declarations = newnode(Intermediate, NULL);
    }

    $$ = newnode(VarDecl, NULL);
    addchild($$, type);
    addchild($$, newnode(Identifier, $1));
    addchild(declarations, $3);
}
;

func_declaration
: FUNC func_header func_body
{
    $$ = newnode(FuncDecl, NULL);
    addchild($$, $2);
    addchild(declarations, $$);
    //addchild($$, $3);
}
;

func_header
: IDENTIFIER LPAR parameters RPAR type
{
    $$ = newnode(FuncHeader, NULL);
    addchild($$, newnode(Identifier, $1));
    addchild($$, $5);
    addchild($$, $3);
}
| IDENTIFIER LPAR parameters RPAR
{
    $$ = newnode(FuncHeader, NULL);
    addchild($$, newnode(Identifier, $1));
    addchild($$, $3);

}
| IDENTIFIER LPAR RPAR type
{
    $$ = newnode(FuncHeader, NULL);
    addchild($$, newnode(Identifier, $1));
    addchild($$, $4);
    addchild($$, newnode(FuncParams, NULL));
}
| IDENTIFIER LPAR RPAR
{
    $$ = newnode(FuncHeader, NULL);
    addchild($$, newnode(Identifier, $1));
    addchild($$, newnode(FuncParams, NULL));
}
;

parameters
: parameter
{}
| parameters COMMA parameter
{}
;

parameter
: IDENTIFIER type
{
    $$ = newnode(ParamDecl, NULL);
    addchild($$, newnode(Identifier, $1));
    addchild($$, $2);
}
;

func_body
: LBRACE vars_statements RBRACE
{}
| LBRACE RBRACE
{}
;

vars_statements
: var_declaration SEMICOLON vars_statements
{}
| statement SEMICOLON vars_statements
{}
| var_declaration SEMICOLON
{}
| statement SEMICOLON
{}
;

statement
: IDENTIFIER ASSIGN NATURAL
{ printf("statement in function body!\n"); }
;

type
: INT
{ 
    type = newnode(Int, NULL);
    $$ = type;
}
| FLOAT32
{
    type = newnode(Float32, NULL);
    $$ = type;
}
| BOOL
{
    type = newnode(Bool, NULL);
    $$ = type;
}
| STR
{
    type = newnode(String, NULL);
    $$ = type;
}
;

%%

void yyerror(char *error) {
    printf("Line %d, column %d: %s: %s\n", line, tok_column, error, yytext);
}
