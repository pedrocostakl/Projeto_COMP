%{

    /*
    **  Pedro Sousa da Costa - 2022220304
    **  Marco Manuel Almeida e Silva - 2021211653
    */

    #include <stdio.h>

    extern int yylex(void);
    void yyerror(char *);
    extern char *yytext;

%}

%token IDENTIFIER
%token STRLIT
%token NATURAL
%token DECIMAL
%token EQ
%token GE
%token GT
%token LE
%token NE
%token AND
%token OR
%token PACKAGE
%token RETURN
%token ELSE
%token FOR
%token IF
%token VAR
%token INT
%token FLOAT32
%token BOOL
%token STRING
%token PRINT
%token PARSEINT
%token FUNC
%token CMDARGS
%token RESERVED

%left '+' '-'
%left '/' '*'

%%

calculator: expression_list                         { printf("\n"); }
          ;

expression_list: expression                         { printf("%d", $1); }
               | expression_list ',' expression     { printf(", %d", $3); }
               ;

expression: NATURAL                                 { $$ = $1; }
          | '(' expression ')'                      { $$ = $2; }
          | expression '+' expression               { $$ = $1 + $3; }
          | expression '-' expression               { $$ = $1 - $3; }
          | expression '*' expression               { $$ = $1 * $3; }
          | expression '/' expression               { $$ = $1 / $3; }
          ;

%%

void yyerror(char *error) {
    printf("%s '%s'\n", error, yytext);
}
