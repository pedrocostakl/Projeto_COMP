
%{
    /*
    **  Pedro Sousa da Costa - 2022220304
    **  Marco Manuel Almeida e Silva - 2021211653
    */

    #include <stdarg.h>
    #include <string.h>

    #include "y.tab.h"
    
    #define BUF_SIZE 8192

    int line = 1;
    int column = 1;
    int tok_column = 1;
    int string_initial_column = 1; /*Auxiliary variable to store the column where the string starts*/
    int comment_line = 0;
    int comment_column = 0; /*Auxiliary variable to store the column where the comment starts*/
    int print_tokens = 0;  /* Global flag to print tokens if -l is passed */
    int semicolon = 0; /* Used to know when to emit semicolons */
    int invalid_esc_chr = 0; /*Flag to determine if the string should be printed*/
    char temp_string [BUF_SIZE] ;

    void run_lex();
    void user_action();
    void print_token(const char *tok, ...);
    int emit_semicolon();
    void newline();
    void reserved();

    #define YY_USER_ACTION user_action();

%}

unicode_letter    [a-zA-Z]
letter            {unicode_letter}|"_"
digit             [0-9]
octal_digit       [0-7]
hex_digit         [0-9a-fA-F]
escape  \\z
reserved          break|case|chan|const|continue|default|defer|fallthrough|go|goto|import|interface|map|range|select|struct|switch|type|"++"|"--"

%X COMMENT STRING

%%

"//".*                                                 { /* Ignore the comment until the end of the line */ }

else                                                   { print_token("ELSE"); semicolon = 0; return ELSE; } /* Reserved keywords */
for                                                    { print_token("FOR"); semicolon = 0; return FOR; }
if                                                     { print_token("IF"); semicolon = 0; return IF; }
package                                                { print_token("PACKAGE"); semicolon = 0; return PACKAGE; }
return                                                 { print_token("RETURN"); semicolon = 1; return RETURN; }
var                                                    { print_token("VAR"); semicolon = 0; return VAR; }
func                                                   { print_token("FUNC"); semicolon = 0; return FUNC; }

"+"                                                    { print_token("PLUS"); semicolon = 0; return yytext[0]; } /* Operators and punctuation */
"-"                                                    { print_token("MINUS"); semicolon = 0; return yytext[0]; }
"*"                                                    { print_token("STAR"); semicolon = 0; return yytext[0]; }
"/"                                                    { print_token("DIV"); semicolon = 0; return yytext[0]; }
"("                                                    { print_token("LPAR"); semicolon = 0; return yytext[0]; }
")"                                                    { print_token("RPAR"); semicolon = 1; return yytext[0]; }
"="                                                    { print_token("ASSIGN"); semicolon = 0; return yytext[0]; }
","                                                    { print_token("COMMA"); semicolon = 0; return yytext[0]; }
";"                                                    { print_token("SEMICOLON"); semicolon = 0; return yytext[0]; }
"_"                                                    { print_token("BLANKID"); semicolon = 0; return yytext[0]; }
">"                                                    { print_token("GT"); semicolon = 0; return yytext[0]; }
"<"                                                    { print_token("LT"); semicolon = 0; return yytext[0]; }
"%"                                                    { print_token("MOD"); semicolon = 0; return yytext[0]; }
"!"                                                    { print_token("NOT"); semicolon = 0; return yytext[0]; }
"{"                                                    { print_token("LBRACE"); semicolon = 0; return yytext[0]; }
"}"                                                    { print_token("RBRACE"); semicolon = 1; return yytext[0]; }
"["                                                    { print_token("LSQ"); semicolon = 0; return yytext[0]; }
"]"                                                    { print_token("RSQ"); semicolon = 1; return yytext[0]; }
"=="                                                   { print_token("EQ"); semicolon = 0; return EQ; }
">="                                                   { print_token("GE"); semicolon = 0; return GE; }
"<="                                                   { print_token("LE"); semicolon = 0; return LE; }
"!="                                                   { print_token("NE"); semicolon = 0; return NE; }
"&&"                                                   { print_token("AND"); semicolon = 0; return AND; }
"||"                                                   { print_token("OR"); semicolon = 0; return OR; }

int                                                    { print_token("INT"); semicolon = 0; return INT; } /* Types */
float32                                                { print_token("FLOAT32"); semicolon = 0; return FLOAT32; }
bool                                                   { print_token("BOOL"); semicolon = 0; return BOOL; }
string                                                 { print_token("STRING"); semicolon = 0; return STR; }

fmt.Println                                            { print_token("PRINT"); semicolon = 0; return PRINT; } /* Functions */
strconv.Atoi                                           { print_token("PARSEINT"); semicolon = 0; return PARSEINT; }
os.Args                                                { print_token("CMDARGS"); semicolon = 0; return CMDARGS; }

{reserved}                                             { reserved(); return RESERVED; } /* Reserved keywords */

{letter}({letter}|{digit})*                            { print_token("IDENTIFIER(%s)", yytext); semicolon = 1; return IDENTIFIER; }

[1-9]{digit}*                                          { print_token("NATURAL(%s)", yytext); semicolon = 1; yylval = atoi(yytext); return NATURAL; } /*naturals don't start with 0, only octals do*/
0{octal_digit}*                                        { print_token("NATURAL(%s)", yytext); semicolon = 1; return NATURAL; }
0{digit}+                                              { printf("Line %d, column %d: invalid octal constant (%s)\n", line, tok_column, yytext); }
[0][xX]{hex_digit}+                                    { print_token("NATURAL(%s)", yytext); semicolon = 1; return NATURAL; }

{digit}*"."{digit}+                                    { print_token("DECIMAL(%s)", yytext); semicolon = 1; return DECIMAL; }
{digit}+"."{digit}*                                    { print_token("DECIMAL(%s)", yytext); semicolon = 1; return DECIMAL; }
{digit}*"."{digit}*[eE][-+]?{digit}+                   { print_token("DECIMAL(%s)", yytext); semicolon = 1; return DECIMAL; }
{digit}+[eE][-+]?{digit}+                              { print_token("DECIMAL(%s)", yytext); semicolon = 1; return DECIMAL; }

" "+                                                   { ; }
"\t"+                                                  { ; }
"\r"                                                   { ; }
"\n"                                                   { newline(); if (emit_semicolon()) return ';'; }

"/*"                                                   { BEGIN(COMMENT); comment_line = line; comment_column = tok_column; }
<COMMENT>.                                             { ; }
<COMMENT>"\n"                                          { newline(); }
<COMMENT>"*/"                                          { BEGIN(INITIAL); }
<COMMENT><<EOF>>                                       { user_action(); BEGIN(INITIAL); printf("Line %d, column %d: unterminated comment\n", comment_line, comment_column); semicolon = 0;}

\"                         { BEGIN(STRING); string_initial_column = tok_column; temp_string[0] = '\0'; invalid_esc_chr = 0;}

<STRING>\"                 { BEGIN(INITIAL); if (!invalid_esc_chr) { print_token("STRLIT(\"%s\")", temp_string); semicolon = 1; return STRLIT; } }
<STRING>\\\n               { BEGIN(INITIAL); printf("Line %d, column %d: invalid escape sequence (%c)\n", line, tok_column, yytext[0]); invalid_esc_chr = 1;printf("Line %d, column %d: unterminated string literal\n", line, string_initial_column); semicolon = 0; newline();}
<STRING>\n                 { BEGIN(INITIAL); printf("Line %d, column %d: unterminated string literal\n", line, string_initial_column); semicolon = 0; newline();}

<STRING>\\[fnrt\\\"]       { strcat(temp_string, yytext); }  /* Valid escape sequences: \f, \n, \r, \t, \\, \" */

<STRING>\\[^fnrt\\]        { printf("Line %d, column %d: invalid escape sequence (%s)\n", line, tok_column, yytext); invalid_esc_chr = 1; }  /* Print the error, but stay in STRING state */
<STRING>[^\\\n"]+          { strcat(temp_string, yytext); }  /* Regular characters inside the string */
<STRING>.                  { printf("Line %d, column %d: illegal character in string literal (%s)\n", line, tok_column, yytext); }  /* Handle any illegal characters */
<STRING><<EOF>>            { user_action(); BEGIN(INITIAL); printf("Line %d, column %d: unterminated string literal\n", line, string_initial_column); }

<<EOF>>                                { if (emit_semicolon()) return ';'; return 0; }  /* Handle end of file */
.                                      { printf("Line %d, column %d: illegal character (%s)\n", line, tok_column, yytext); }

%%

extern int yylex();
extern int yyparse();

int main(int argc, char **argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "-l") == 0) {
            print_tokens = 1;  /* Enable token printing if -l option is passed */
            run_lex();
            return 0;
        }
        if (strcmp(argv[1], "-t") == 0) {
            yyparse();
            return 0;
        }
    }
    run_lex();  /* Run the lexical analysis automaton */
    return 0;
}

int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}

void run_lex() {
    while (yylex() != 0);
}

void user_action() {
    tok_column = column;
    column += yyleng;
}

void print_token(const char *tok, ...) {
    if (print_tokens) {
        va_list args;
        va_start(args, tok);
        vprintf(tok, args);
        printf("\n");
        va_end(args);
    }
}
int emit_semicolon() {
    if (semicolon) {
        print_token("SEMICOLON");  /* Emit a semicolon if the last token requires it */
        semicolon = 0;     /* Reset the flag */
        return 1;
    }
    return 0;
}

void newline() {
    line++;
    column = 1;
}

void reserved(){
    print_token("RESERVED(%s)", yytext);
    semicolon = 0;
}
