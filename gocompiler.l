%{
    /* place here any C variables and definitions */

    int line = 1;
    int column = 1;
    int tok_column = 1;

    void user_action();
    void newline();
    void reserved();

    #define YY_USER_ACTION user_action();

%}

digit   [0-9]
letter  [a-zA-Z] | "_"
escape  \\z

%X COMMENT STRING

%%


else                                { printf("ELSE\n"); }
for                                 { printf("FOR\n"); }
if                                  { printf("IF\n"); }
package                             { printf("PACKAGE\n"); }
return                              { printf("RETURN\n"); }
var                                 { printf("VAR\n"); }
func                                { printf("FUNC\n"); }


"+"                                 { printf("PLUS\n"); }
"-"                                 { printf("MINUS\n"); }
"*"                                 { printf("START\n"); }
"/"                                 { printf("DIV\n"); }
"("                                 { printf("LPAR\n"); }
")"                                 { printf("RPAR\n"); }
"="                                 { printf("ASSIGN\n"); }
","                                 { printf("COMMA\n"); }
";"                                 { printf("SEMICOLON\n"); }
"_"                                 { printf("BLANKID\n"); }
">"                                 { printf("GT\n"); }
"<"                                 { printf("LT\n"); }
"%"                                 { printf("MOD\n); }
"!"                                 { printf("NOT\n"); }
"{"                                 { printf("LBRACE\n"); }
"}"                                 { printf("RBRACE\n"); }
"["                                 { printf("LSQ\n"); }
"]"                                 { printf("RSQ\n"); }
==                                  { printf("EQUAL\n"); }
>=                                  { printf("GE\n"); }
<=                                  { printf("LE\n); }
!=                                  { printf("NE\n); }
&&                                  { printf("AND\n); }
||                                  { printf(OR\n); }


int                                 { printf("INT\n"); }
float32                             { printf("FLOAT32\n"); }
bool                                { printf("BOOL\n"); }
string                              { printf("STRING\n"); }


fmt.Println                         { printf("PRINT\n"); }
strconv.Atoi                        { printf("PARSEINT\n"); }
os.Args                             { printf("ARGS\n"); }


break                               { reserved(); }
case                                { reserved(); }
chan                                { reserved(); }
const                               { reserved(); }
continue                            { reserved(); }
default                             { reserved(); }
defer                               { reserved(); }
fallthrough                         { reserved(); }
go                                  { reserved(); }
goto                                { reserved(); }
import                              { reserved(); }
interface                           { reserved(); }
map                                 { reserved(); }
range                               { reserved(); }
select                              { reserved(); }
struct                              { reserved(); }
switch                              { reserved(); }
type                                { reserved(); }
++                                  { reserved(); }
--                                  { reserved(); }

{digit}+                            { printf("NATURAL(%s)\n", yytext); }
{digit}*"."{digit}+                 { printf("DECIMAL(%s)\n", yytext); }
{letter}+({letter}|{digit})*        { printf("IDENTIFIER(%s)\n", yytext); }

" "+                                { ; }
"\t"+                               { ; }
"\r"                                { ; }
"\n"                                { newline(); }

"/*"                                { BEGIN(COMMENT); }
<COMMENT>.                          { ; }
<COMMENT>"\n"                       { newline(); }
<COMMENT>"*/"                       { BEGIN(INITIAL); }

\"                                  { BEGIN(STRING); }
<STRING>\"                          { BEGIN(INITIAL); }
<STRING>\\[fnrt\\\"]                { ; }
<STRING>\\[^fnrt\\\"]               { printf("Line %d, column %d: invalid escape sequence (%s)\n", line, tok_column, yytext); }
<STRING>[^\\\n]*                    { printf("STRLIT(%s)\n", yytext); }
<STRING>\n                          { newline(); BEGIN(INITIAL); }

.                                   { printf("Line %d, column %d: unrecognized character (%s)\n", line, tok_column, yytext); }

%%

extern int yylex();

int main() {
    yylex();    /* run the lexical analysis automaton */
    return 0;
}

int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}

void user_action() {
    tok_column = column;
    column += yyleng;
}

void newline() {
    line++;
    column = 1;
}

void reserved() {
    printf("RESERVED\n");
}