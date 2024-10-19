%{

    /*
    **  Pedro Sousa da Costa - 2022220304
    **  Marco Manuel Almeida e Silva - 2021211653
    */

    #include <stdio.h>

    extern int yylex(void);
    void yyerror(char *);

    extern char *yytext;
    extern int line;           /* Extern line variable from lex file */
    extern int tok_column;     /* Extern token column variable from lex file */

%}

%token IDENTIFIER
%token STRLIT
%token NATURAL
%token DECIMAL
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

%left '+' '-'
%left '/' '*'

%%

program
: PACKAGE IDENTIFIER SEMICOLON declarations
{ printf("package!\n"); }
| PACKAGE IDENTIFIER SEMICOLON
{ printf("empty program!\n"); }
;

type
: INT
{}
| FLOAT32
{}
| BOOL
{}
| STR
{}
;

declarations
: var_declaration SEMICOLON declarations
{}
| var_declaration SEMICOLON
{}
| func_declaration SEMICOLON declarations
{}
| func_declaration SEMICOLON
{}
;

var_declaration
: VAR var_spec
{}
| VAR LPAR var_spec SEMICOLON RPAR
{}
;

var_spec
: IDENTIFIER type
{ printf("var!\n"); }
| IDENTIFIER var_spec_list
{}
;

var_spec_list
: COMMA var_spec
{ printf("var in list!\n"); }
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
| parameter parameters_list
{}
;

parameters_list
: COMMA parameters
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

%%

void yyerror(char *error) {
    printf("Line %d, column %d: %s: %s\n", line, tok_column, error, yytext);
}
