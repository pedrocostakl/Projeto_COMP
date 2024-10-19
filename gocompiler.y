%{

    /*
    **  Pedro Sousa da Costa - 2022220304
    **  Marco Manuel Almeida e Silva - 2021211653
    */

    #include <stdio.h>

    extern int yylex(void);
    void yyerror(char *);

    extern char *yytext;
    extern int line;
    extern int tok_column;

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
;

declarations
: VAR IDENTIFIER type SEMICOLON declarations
{ printf("var!\n"); }
| VAR IDENTIFIER type SEMICOLON
{ printf("var!\n"); }
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

%%

void yyerror(char *error) {
    printf("Line %d, column %d: %s: %s\n", line, tok_column, error, yytext);
}
