%{
    /* C variables and definitions */
    int column = 0;
    int tok_column = 0;
    int line = 1;

    void user_action();
    void newline();

    #define YY_user_action() user_action();
%}

%X COMMENT STRING


digit   [0-9]

%%
"/*"                { BEGIN(COMMENT); user_action(); }
<COMMENT>[^*\n]+    { user_action(); }
<COMMENT>[*]+[^/\n]* { user_action(); }
<COMMENT>\n         { newline(); }
<COMMENT>"*/"       { BEGIN(INITIAL); user_action(); }

\"                 { BEGIN(STRING); user_action(); }
<STRING>\\[fnrt"\\] { 
                        user_action(); 
                     }

<STRING>\\[^fnrt"\\] { 
                        printf("Line %d, column %d: invalid escape sequence (\\%c)\n", line, column, yytext[1]); 
                        user_action(); 
                     }

<STRING>[^"\\\n]+   {  printf("\nSTRLIT %s", yytext);user_action(); }

<STRING>[\n]           { 
                        printf("Line %d, column %d: unterminated string\n", line, column); 
                        newline();
                        BEGIN(INITIAL); 
                     }
<STRING>\"         { BEGIN(INITIAL); user_action(); }

{digit}+|[0x|0X][a-f0-9|A-F]+          { user_action(); printf("NATURAL (%s)\n", yytext); }
{digit}*"."{digit}+ { user_action(); printf("DECIMAL (%s)\n", yytext); }
[\n]                { newline(); }
[ ]                 { user_action(); }
[\t]                { user_action(); }
[(]                 { user_action(); printf("\n"); }
[)]                 { user_action(); printf("\n"); }
[=]                 { user_action(); printf("=\n"); }
[,]                 { user_action(); printf(",\n"); }
[*]                 { user_action(); printf("*\n"); }
[/]                 { user_action(); printf("/\n"); }
[+]                 { user_action(); printf("+\n"); }
[-]                 { user_action(); printf("-\n"); }
int|integer         { user_action(); printf("INTEGER\n"); }
double              { user_action(); printf("DOUBLE\n"); }
if                  { user_action(); printf("IF\n"); }
then                { user_action(); printf("THEN\n"); }
else                { user_action(); printf("ELSE\n"); }
[a-z|A-Z|_][a-z0-9|A-Z|_]*      { user_action(); printf("IDENTIFIER (%s)\n", yytext); }

.                   { user_action(); printf("error... on line %d column %d\n", line, column); }

%%

int main() {
    yylex();  /* Run the lexical analysis automaton */
    return 0;
}

int yywrap() {  /* Called on EOF, return 1 to terminate */
    return 1;
}
void user_action(){
    tok_column = column;
    column+=yyleng;
}
void newline(){
    line++;
    column = 0;
}