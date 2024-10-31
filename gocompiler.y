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
    extern int line;           /* Extern line variable from lex file */
    extern int tok_column;     /* Extern token column variable from lex file */

    struct node_t *program;

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

%type<node> program type declarations var_declaration var_spec func_declaration parameters parameter
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
    $$ = newnode(Intermediate, NULL);
    addchild($$, $1);
    addchild($$, $3);
}
| var_declaration SEMICOLON
{
    $$ = $1;
}
| declarations SEMICOLON func_declaration
{
    $$ = program;
}
| func_declaration SEMICOLON
{
    $$ = program;
}
;

var_declaration
: VAR var_spec
{
    $$ = $2;
}
| VAR LPAR var_spec SEMICOLON RPAR
{
    $$ = $3;
}
;

var_spec
: IDENTIFIER type
{
    $$ = newnode(VarDecl, NULL);
    addchild($$, $2);
    addchild($$, newnode(Identifier, $1));
}
| IDENTIFIER COMMA var_spec
{
    $$ = $3;
}
;

func_declaration
: FUNC IDENTIFIER LPAR parameters RPAR type func_body
{ printf("function!\n"); }
| FUNC IDENTIFIER LPAR parameters RPAR func_body
{ printf("function!\n"); }
| FUNC IDENTIFIER LPAR RPAR type func_body
{ printf("function!\n"); }
| FUNC IDENTIFIER LPAR RPAR func_body
{ printf("function!\n"); }
;

parameters
: parameter
{}
| parameters COMMA parameter
{}
;

parameter
: IDENTIFIER type
{}
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
    $$ = newnode(Int, NULL);
}
| FLOAT32
{
    $$ = newnode(Float32, NULL);
}
| BOOL
{
    $$ = newnode(Bool, NULL);
}
| STR
{
    $$ = newnode(String, NULL);
}
;

%%

void yyerror(char *error) {
    printf("Line %d, column %d: %s: %s\n", line, tok_column, error, yytext);
}
