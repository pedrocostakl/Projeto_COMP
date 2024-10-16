
%{
    /*
    **  Pedro Sousa da Costa - 2022220304
    **  Marco Manuel Almeida e Silva - 2021211653
    */

    /* place here any C variables and definitions */

    #include <stdarg.h>
    #include <string.h>

    int line = 1;
    int column = 1;
    int tok_column = 1;
    int string_initial_column = 1;
    int comment_line = 0;
    int comment_column = 0;
    int print_tokens = 0;  /* Global flag to print tokens if -l is passed */
    int emit_semicolon = 0; /* Used to know when to omit semicolons */
    int invalid_esc_chr = 0; /*Flag to determine if the string should be printed*/

    void user_action();
    void print_token(const char *tok, ...);
    void emit_semicolon_if_needed();
    void newline();
    void reserved();


    char  temp_string [1000] ;

    #define YY_USER_ACTION user_action();

%}

unicode_letter    [a-zA-Z]
letter            {unicode_letter}|"_"
digit             [0-9]
octal_digit       [0-7]
hex_digit         [0-9a-fA-F]  /* Hex digits: 0-9, a-f, A-F */
escape  \\z

%X COMMENT STRING

%%

"//".*                                                 { /* Ignore the comment until the end of the line */ }

else                                                   { if (print_tokens) printf("ELSE\n"); emit_semicolon = 0; } /* Reserved keywords */
for                                                    { if (print_tokens) printf("FOR\n"); emit_semicolon = 0; }
if                                                     { if (print_tokens) printf("IF\n"); emit_semicolon = 0; }
package                                                { if (print_tokens) printf("PACKAGE\n"); emit_semicolon = 0; }
return                                                 { if (print_tokens) printf("RETURN\n"); emit_semicolon = 1; }
var                                                    { if (print_tokens) printf("VAR\n"); emit_semicolon = 0; }
func                                                   { if (print_tokens) printf("FUNC\n"); emit_semicolon = 0; }

"+"                                                    { if (print_tokens) printf("PLUS\n"); emit_semicolon = 0; } /* Operators and punctuation */
"-"                                                    { if (print_tokens) printf("MINUS\n"); emit_semicolon = 0; }
"*"                                                    { if (print_tokens) printf("STAR\n"); emit_semicolon = 0; }
"/"                                                    { if (print_tokens) printf("DIV\n"); emit_semicolon = 0; }
"("                                                    { if (print_tokens) printf("LPAR\n"); emit_semicolon = 0; }
")"                                                    { if (print_tokens) printf("RPAR\n"); emit_semicolon = 1; }
"="                                                    { if (print_tokens) printf("ASSIGN\n"); emit_semicolon = 0; }
","                                                    { if (print_tokens) printf("COMMA\n"); emit_semicolon = 0; }
";"                                                    { if (print_tokens) printf("SEMICOLON\n"); emit_semicolon = 0; }
"_"                                                    { if (print_tokens) printf("BLANKID\n"); emit_semicolon = 0; }
">"                                                    { if (print_tokens) printf("GT\n"); emit_semicolon = 0; }
"<"                                                    { if (print_tokens) printf("LT\n"); emit_semicolon = 0; }
"%"                                                    { if (print_tokens) printf("MOD\n"); emit_semicolon = 0; }
"!"                                                    { if (print_tokens) printf("NOT\n"); emit_semicolon = 0; }
"{"                                                    { if (print_tokens) printf("LBRACE\n"); emit_semicolon = 0; }
"}"                                                    { if (print_tokens) printf("RBRACE\n"); emit_semicolon = 1; }
"["                                                    { if (print_tokens) printf("LSQ\n"); emit_semicolon = 0; }
"]"                                                    { if (print_tokens) printf("RSQ\n"); emit_semicolon = 1; }
"=="                                                   { if (print_tokens) printf("EQ\n"); emit_semicolon = 0; }
">="                                                   { if (print_tokens) printf("GE\n"); emit_semicolon = 0; }
"<="                                                   { if (print_tokens) printf("LE\n"); emit_semicolon = 0; }
"!="                                                   { if (print_tokens) printf("NE\n"); emit_semicolon = 0; }
"&&"                                                   { if (print_tokens) printf("AND\n"); emit_semicolon = 0; }
"||"                                                   { if (print_tokens) printf("OR\n"); emit_semicolon = 0; }

int                                                    { if (print_tokens) printf("INT\n"); emit_semicolon = 0; } /* Types */
float32                                                { if (print_tokens) printf("FLOAT32\n"); emit_semicolon = 0; }
bool                                                   { if (print_tokens) printf("BOOL\n"); emit_semicolon = 0; }
string                                                 { if (print_tokens) printf("STRING\n"); emit_semicolon = 0; }

fmt.Println                                            { if (print_tokens) printf("PRINT\n"); emit_semicolon = 0; } /* Functions */
strconv.Atoi                                           { if (print_tokens) printf("PARSEINT\n"); emit_semicolon = 0; }
os.Args                                                { if (print_tokens) printf("CMDARGS\n"); emit_semicolon = 0; }

break                                                  { reserved(); } /* Reserved keywords */
case                                                   { reserved(); }
chan                                                   { reserved(); }
const                                                  { reserved(); }
continue                                               { reserved(); }
default                                                { reserved(); }
defer                                                  { reserved(); }
fallthrough                                            { reserved(); }
go                                                     { reserved(); }
goto                                                   { reserved(); }
import                                                 { reserved(); }
interface                                              { reserved(); }
map                                                    { reserved(); }
range                                                  { reserved(); }
select                                                 { reserved(); }
struct                                                 { reserved(); }
switch                                                 { reserved(); }
type                                                   { reserved(); }
"++"                                                   { reserved(); }
"--"                                                   { reserved(); }

{digit}*"."{digit}+                                    { if (print_tokens) printf("DECIMAL(%s)\n", yytext); emit_semicolon = 1; }
{digit}+"."{digit}*                                    { if (print_tokens) printf("DECIMAL(%s)\n", yytext); emit_semicolon = 1; }
{digit}*"."{digit}*[eE][-+]?{digit}+                 { if (print_tokens) printf("DECIMAL(%s)\n", yytext); emit_semicolon = 1; }
{digit}+[eE][-+]?{digit}+                            { if (print_tokens) printf("DECIMAL(%s)\n", yytext); emit_semicolon = 1; }


[1-9]{digit}*                                          { if (print_tokens) printf("NATURAL(%s)\n", yytext); emit_semicolon = 1; } /*naturals nao começam com 0 devido aos octais*/

[0]{octal_digit}*                                      { if (print_tokens) printf("NATURAL(%s)\n", yytext); emit_semicolon = 1; }
[0]{digit}*                                            { printf("Line %d, column %d: invalid octal constant (%s)\n", line, tok_column, yytext); }                                       

{letter}({letter}|{digit})*                            { if (print_tokens) printf("IDENTIFIER(%s)\n", yytext); emit_semicolon = 1; }

" "+                                                   { ; }
"\t"+                                                  { ; }
"\r"                                                   { ; }
"\n"                                                   { newline(); }

"/*"                                                   { BEGIN(COMMENT); comment_line = line; comment_column = tok_column; }
<COMMENT>.                                             { ; }
<COMMENT>"\n"                                          { newline(); }
<COMMENT>"*/"                                          { BEGIN(INITIAL); }
<COMMENT><<EOF>>                                       { user_action(); BEGIN(INITIAL); printf("Line %d, column %d: unterminated comment\n", comment_line, comment_column); emit_semicolon = 0;}

\"                          { BEGIN(STRING); string_initial_column = tok_column; temp_string[0] = '\0'; invalid_esc_chr = 0;}

<STRING>\"                   { BEGIN(INITIAL); if (print_tokens) if (!invalid_esc_chr) printf("STRLIT(\"%s\")\n", temp_string); if(!invalid_esc_chr) emit_semicolon = 1; }

<STRING>\n                 { BEGIN(INITIAL); printf("Line %d, column %d: unterminated string literal\n", line, string_initial_column); emit_semicolon = 0; newline();}

<STRING>\\[fnrt\\\"]          { strcat(temp_string, yytext); }  /* Valid escape sequences: \f, \n, \r, \t, \\, \" */

<STRING>\\[^fnrt\\]           { printf("Line %d, column %d: invalid escape sequence (\\%c)\n", line, tok_column, yytext[1]); invalid_esc_chr = 1; }  /* Print the error, but stay in STRING state */

<STRING>[^\\\n"]+             { strcat(temp_string, yytext); }  /* Regular characters inside the string */

<STRING>.                     { printf("Line %d, column %d: illegal character in string literal (%s)\n", line, tok_column, yytext); }  /* Handle any illegal characters */
<STRING><<EOF>>              { user_action(); BEGIN(INITIAL); printf("Line %d, column %d: unterminated string literal\n", line, string_initial_column); }

<<EOF>>                                { emit_semicolon_if_needed(); return 0; }  /* Handle end of file */
.                                                      { printf("Line %d, column %d: illegal character (%s)\n", line, tok_column, yytext); }

%%

extern int yylex();

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "-l") == 0) {
        print_tokens = 1;  /* Enable token printing if -l option is passed */
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
        printf("SEMICOLON\n");  /* Emit a semicolon if the last token requires it */
        emit_semicolon = 0;     /* Reset the flag */
    }
}

void newline() {
    line++;
    column = 1;

    if (emit_semicolon) {
        print_token("SEMICOLON");
        emit_semicolon = 0;
    }
}

void reserved(){
    print_token("RESERVED(%s)", yytext);
    emit_semicolon = 0;
}
