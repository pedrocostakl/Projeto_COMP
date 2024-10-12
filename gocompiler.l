%{
    /* place here any C variables and definitions */

    int line = 1;
    int column = 1;
    int tok_column = 1;
    int print_tokens = 0;  /* Global flag to print tokens if -l is passed */

    void user_action();
    void newline();
    void reserved();

    #define YY_USER_ACTION user_action();

%}

digit   [0-9]
letter  [a-zA-Z]|"_"
escape  \\z

%X COMMENT STRING

%%

"//".*                             { /* Ignore the comment until the end of the line */ }
else                                { if (print_tokens) printf("ELSE\n"); } /* Reserved keywords */
for                                 { if (print_tokens) printf("FOR\n"); }
if                                  { if (print_tokens) printf("IF\n"); }
package                             { if (print_tokens) printf("PACKAGE\n"); }
return                              { if (print_tokens) printf("RETURN\n"); }
var                                 { if (print_tokens) printf("VAR\n"); }
func                                { if (print_tokens) printf("FUNC\n"); }


"+"                                 { if (print_tokens) printf("PLUS\n"); } /* Operators and punctuation */
"-"                                 { if (print_tokens) printf("MINUS\n"); }
"*"                                 { if (print_tokens) printf("STAR\n"); }
"/"                                 { if (print_tokens) printf("DIV\n"); }
"("                                 { if (print_tokens) printf("LPAR\n"); }
")"                                 { if (print_tokens) printf("RPAR\n"); }
"="                                 { if (print_tokens) printf("ASSIGN\n"); }
","                                 { if (print_tokens) printf("COMMA\n"); }
";"                                 { if (print_tokens) printf("SEMICOLON\n"); }
"_"                                 { if (print_tokens) printf("BLANKID\n"); }
">"                                 { if (print_tokens) printf("GT\n"); }
"<"                                 { if (print_tokens) printf("LT\n"); }
"%"                                 { if (print_tokens) printf("MOD\n"); }
"!"                                 { if (print_tokens) printf("NOT\n"); }
"{"                                 { if (print_tokens) printf("LBRACE\n"); }
"}"                                 {if (print_tokens)  printf("RBRACE\n"); }
"["                                 { if (print_tokens) printf("LSQ\n"); }
"]"                                 { if (print_tokens) printf("RSQ\n"); }
"=="                                  { if (print_tokens) printf("EQ\n"); }
">="                                  { if (print_tokens) printf("GE\n"); }
"<="                                  { if (print_tokens) printf("LE\n"); }
"!="                                  { if (print_tokens) printf("NE\n"); }
"&&"                                  { if (print_tokens) printf("AND\n"); }
"||"                                  { if (print_tokens) printf("OR\n"); }


int                                 { if (print_tokens) printf("INT\n"); } /* Types */
float32                             { if (print_tokens) printf("FLOAT32\n"); }
bool                                { if (print_tokens) printf("BOOL\n"); }
string                              { if (print_tokens) printf("STRING\n"); }


fmt.Println                         { if (print_tokens) printf("PRINT\n"); } /* Functions */
strconv.Atoi                        { if (print_tokens) printf("PARSEINT\n"); }
os.Args                             { if (print_tokens) printf("CMDARGS\n"); }


break                               { reserved(); } /* Reserved keywords */
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
"++"                                  { reserved(); }
"--"                                  { reserved(); }

{digit}*"."{digit}+                 { if (print_tokens) printf("DECIMAL(%s)\n", yytext); }
{digit}+"."{digit}*                 { if (print_tokens) printf("DECIMAL(%s)\n", yytext); }
{digit}*"."{digit}*[e|E][-|+]?{digit}+                 { if (print_tokens) printf("DECIMAL(%s)\n", yytext); }
{digit}+[e|E][-|+]?{digit}+                 { if (print_tokens) printf("DECIMAL(%s)\n", yytext); }



[0][1-8]*[0-8]*                          { if (print_tokens) printf("NATURAL(%s)\n", yytext); }
[1-9]{digit}*                            { if (print_tokens) printf("NATURAL(%s)\n", yytext); } /*naturals nao começam com 0 devido aos octais*/
[0x|0X]([a-fA-F])+                  { if (print_tokens) printf("NATURAL(%s)\n", yytext); }



{letter}({letter}|{digit})*         { if (print_tokens) printf("IDENTIFIER(%s)\n", yytext); }

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
<STRING>[^\\\n"]*                    { if (print_tokens) printf("STRLIT(%s)\n", yytext); }
<STRING>\n                          { newline(); BEGIN(INITIAL); }

.                                   { printf("Line %d, column %d: illegal character (%s)\n", line, tok_column, yytext); }

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

void newline() {
    line++;
    column = 1;
}

void reserved(){
  if (print_tokens) {
        printf("RESERVED\n");
    }
}
