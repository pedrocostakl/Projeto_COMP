
%{
    /*
    ** --
    **  Marco Manuel Almeida e Silva - 2021211653
    */

    /* place here any C variables and definitions */

    #include <stdarg.h>
    #include <string.h>

    int line = 1;
    int column = 1;
    int tok_column = 1;
    int comment_line = 0;
    int comment_column = 0;
    int print_tokens = 0;  /* Global flag to print tokens if -l is passed */
    int emit_semicolon = 0; /* Used to know when to omit semicolons */

    void user_action();
    void print_token(const char *tok, ...);
    void newline();
    void reserved();

    #define YY_USER_ACTION user_action();

%}

unicode_letter    [a-zA-Z]
letter            {unicode_letter}|"_"
digit             [0-9]
octal_digit       [0-7]
hex_digit         {digit}|{unicode_letter}
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
{digit}*"."{digit}*[e|E][-|+]?{digit}+                 { if (print_tokens) printf("DECIMAL(%s)\n", yytext); emit_semicolon = 1; }
{digit}+[e|E][-|+]?{digit}+                            { if (print_tokens) printf("DECIMAL(%s)\n", yytext); emit_semicolon = 1; }

[1-9]{digit}*                                          { if (print_tokens) printf("NATURAL(%s)\n", yytext); emit_semicolon = 1; } /*naturals nao começam com 0 devido aos octais*/
[0][x|X]{hex_digit}+                                   { if (print_tokens) printf("NATURAL(%s)\n", yytext); emit_semicolon = 1; }
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
<COMMENT><<EOF>>                                       { BEGIN(INITIAL); printf("Line %d, column %d: unterminated comment\n", comment_line, comment_column); }

\"                                                     { BEGIN(STRING); }
<STRING>\"                                             { BEGIN(INITIAL); }
<STRING>\\[fnrt\\\"]                                   { ; }
<STRING>\\[^fnrt\\\"]                                  { printf("Line %d, column %d: invalid escape sequence (%s)\n", line, tok_column, yytext); }
<STRING>[^\\\n"]*                                      { if (print_tokens) printf("STRLIT(\"%s\")\n", yytext); emit_semicolon = 1; }
<STRING>\n                                             { newline(); BEGIN(INITIAL); }

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
