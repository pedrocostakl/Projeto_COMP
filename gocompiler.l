
%{
    /*
    **  Pedro Sousa da Costa - 2022220304
    **  Marco Manuel Almeida e Silva - 2021211653
    */

    /* place here any C variables and definitions */

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
    int emit_semicolon = 0; /* Used to know when to omit semicolons */
    int invalid_esc_chr = 0; /*Flag to determine if the string should be printed*/
    char temp_string [BUF_SIZE] ;

    void user_action();
    void print_token(const char *tok, ...);
    void emit_semicolon_if_needed();
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

else                                                   { print_token("ELSE"); emit_semicolon = 0; } /* Reserved keywords */
for                                                    { print_token("FOR"); emit_semicolon = 0; }
if                                                     { print_token("IF"); emit_semicolon = 0; }
package                                                { print_token("PACKAGE"); emit_semicolon = 0; }
return                                                 { print_token("RETURN"); emit_semicolon = 1; }
var                                                    { print_token("VAR"); emit_semicolon = 0; }
func                                                   { print_token("FUNC"); emit_semicolon = 0; }

"+"                                                    { print_token("PLUS"); emit_semicolon = 0; } /* Operators and punctuation */
"-"                                                    { print_token("MINUS"); emit_semicolon = 0; }
"*"                                                    { print_token("STAR"); emit_semicolon = 0; }
"/"                                                    { print_token("DIV"); emit_semicolon = 0; }
"("                                                    { print_token("LPAR"); emit_semicolon = 0; }
")"                                                    { print_token("RPAR"); emit_semicolon = 1; }
"="                                                    { print_token("ASSIGN"); emit_semicolon = 0; }
","                                                    { print_token("COMMA"); emit_semicolon = 0; }
";"                                                    { print_token("SEMICOLON"); emit_semicolon = 0; }
"_"                                                    { print_token("BLANKID"); emit_semicolon = 0; }
">"                                                    { print_token("GT"); emit_semicolon = 0; }
"<"                                                    { print_token("LT"); emit_semicolon = 0; }
"%"                                                    { print_token("MOD"); emit_semicolon = 0; }
"!"                                                    { print_token("NOT"); emit_semicolon = 0; }
"{"                                                    { print_token("LBRACE"); emit_semicolon = 0; }
"}"                                                    { print_token("RBRACE"); emit_semicolon = 1; }
"["                                                    { print_token("LSQ"); emit_semicolon = 0; }
"]"                                                    { print_token("RSQ"); emit_semicolon = 1; }
"=="                                                   { print_token("EQ"); emit_semicolon = 0; }
">="                                                   { print_token("GE"); emit_semicolon = 0; }
"<="                                                   { print_token("LE"); emit_semicolon = 0; }
"!="                                                   { print_token("NE"); emit_semicolon = 0; }
"&&"                                                   { print_token("AND"); emit_semicolon = 0; }
"||"                                                   { print_token("OR"); emit_semicolon = 0; }

int                                                    { print_token("INT"); emit_semicolon = 0; } /* Types */
float32                                                { print_token("FLOAT32"); emit_semicolon = 0; }
bool                                                   { print_token("BOOL"); emit_semicolon = 0; }
string                                                 { print_token("STRING"); emit_semicolon = 0; }

fmt.Println                                            { print_token("PRINT"); emit_semicolon = 0; } /* Functions */
strconv.Atoi                                           { print_token("PARSEINT"); emit_semicolon = 0; }
os.Args                                                { print_token("CMDARGS"); emit_semicolon = 0; }

{reserved}                                             { reserved(); } /* Reserved keywords */

{letter}({letter}|{digit})*                            { print_token("IDENTIFIER(%s)", yytext); emit_semicolon = 1; }

[1-9]{digit}*                                          { print_token("NATURAL(%s)", yytext); emit_semicolon = 1; } /*naturals don't start with 0, only octals do*/
0{octal_digit}*                                        { print_token("NATURAL(%s)", yytext); emit_semicolon = 1; }
0{digit}+                                              { printf("Line %d, column %d: invalid octal constant (%s)\n", line, tok_column, yytext); }
[0][xX]{hex_digit}+                                    { print_token("NATURAL(%s)", yytext); emit_semicolon = 1; }

{digit}*"."{digit}+                                    { print_token("DECIMAL(%s)\n", yytext); emit_semicolon = 1; }
{digit}+"."{digit}*                                    { print_token("DECIMAL(%s)\n", yytext); emit_semicolon = 1; }
{digit}*"."{digit}*[eE][-+]?{digit}+                   { print_token("DECIMAL(%s)\n", yytext); emit_semicolon = 1; }
{digit}+[eE][-+]?{digit}+                              { print_token("DECIMAL(%s)\n", yytext); emit_semicolon = 1; }


" "+                                                   { ; }
"\t"+                                                  { ; }
"\r"                                                   { ; }
"\n"                                                   { newline(); emit_semicolon_if_needed(); }

"/*"                                                   { BEGIN(COMMENT); comment_line = line; comment_column = tok_column; }
<COMMENT>.                                             { ; }
<COMMENT>"\n"                                          { newline(); }
<COMMENT>"*/"                                          { BEGIN(INITIAL); }
<COMMENT><<EOF>>                                       { user_action(); BEGIN(INITIAL); printf("Line %d, column %d: unterminated comment\n", comment_line, comment_column); emit_semicolon = 0;}

\"                         { BEGIN(STRING); string_initial_column = tok_column; temp_string[0] = '\0'; invalid_esc_chr = 0;}

<STRING>\"                 { BEGIN(INITIAL); if (print_tokens) if (!invalid_esc_chr) printf("STRLIT(\"%s\")\n", temp_string); if(!invalid_esc_chr) emit_semicolon = 1; }
<STRING>\\\n               { BEGIN(INITIAL); printf("Line %d, column %d: invalid escape sequence (%c)\n", line, tok_column, yytext[0]); invalid_esc_chr = 1;printf("Line %d, column %d: unterminated string literal\n", line, string_initial_column); emit_semicolon = 0; newline();}
<STRING>\n                 { BEGIN(INITIAL); printf("Line %d, column %d: unterminated string literal\n", line, string_initial_column); emit_semicolon = 0; newline();}

<STRING>\\[fnrt\\\"]       { strcat(temp_string, yytext); }  /* Valid escape sequences: \f, \n, \r, \t, \\, \" */

<STRING>\\[^fnrt\\]        { printf("Line %d, column %d: invalid escape sequence (%s)\n", line, tok_column, yytext); invalid_esc_chr = 1; }  /* Print the error, but stay in STRING state */
<STRING>[^\\\n"]+          { strcat(temp_string, yytext); }  /* Regular characters inside the string */
<STRING>.                  { printf("Line %d, column %d: illegal character in string literal (%s)\n", line, tok_column, yytext); }  /* Handle any illegal characters */
<STRING><<EOF>>            { user_action(); BEGIN(INITIAL); printf("Line %d, column %d: unterminated string literal\n", line, string_initial_column); }

<<EOF>>                                { emit_semicolon_if_needed(); return 0; }  /* Handle end of file */
.                                      { printf("Line %d, column %d: illegal character (%s)\n", line, tok_column, yytext); }

%%

extern int yylex();

int main(int argc, char **argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "-l") == 0) {
            print_tokens = 1;  /* Enable token printing if -l option is passed */
            yylex();
            return 0;
        }
        if (strcmp(argv[1], "-t") == 0) {
            yyparse();
            return 0;
        }
    }
    yylex();  /* Run the lexical analysis automaton */
    return 0;
}

int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
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
void emit_semicolon_if_needed() {
    if (emit_semicolon) {
        print_token("SEMICOLON");  /* Emit a semicolon if the last token requires it */
        emit_semicolon = 0;     /* Reset the flag */
    }
}

void newline() {
    line++;
    column = 1;
}

void reserved(){
    print_token("RESERVED(%s)", yytext);
    emit_semicolon = 0;
}
