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

%token NATURAL
%token IF
%token THEN
%token ELSE

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
