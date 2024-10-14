%{
    /* place here any C variables and definitions */

    int line = 1;
    int column = 1;
    int tok_column = 1;
    int print_tokens = 0;  /* Global flag to print tokens if -l is passed */
    int last_token_type = 0;  /* Variable to store the type of the last token */

    /* Token types */
    #define TOK_NATURAL 1
    #define TOK_DECIMAL 2
    #define TOK_STRLIT 3
    #define TOK_IDENTIFIER 4
    #define TOK_RETURN 5
    #define TOK_RPAR 6
    #define TOK_RSQ 7
    #define TOK_RBRACE 8

    void user_action();
    void newline();
    void reserved();

    #define YY_USER_ACTION user_action();

%}

digit   [0-9]
letter  [a-zA-Z_]
escape  \\z

%X COMMENT STRING

%%

"//".*                             { /* Ignore the comment until the end of the line */ }
else                                { if (print_tokens) printf("ELSE\n"); last_token_type = 0; } /* Reserved keywords */
for                                 { if (print_tokens) printf("FOR\n"); last_token_type = 0; }
if                                  { if (print_tokens) printf("IF\n"); last_token_type = 0; }
package                             { if (print_tokens) printf("PACKAGE\n"); last_token_type = 0; }
return                              { if (print_tokens) printf("RETURN\n"); last_token_type = TOK_RETURN; }
var                                 { if (print_tokens) printf("VAR\n"); last_token_type = 0; }
func                                { if (print_tokens) printf("FUNC\n"); last_token_type = 0; }


"+"                                 { if (print_tokens) printf("PLUS\n"); last_token_type = 0; } /* Operators and punctuation */
"-"                                 { if (print_tokens) printf("MINUS\n"); last_token_type = 0; }
"*"                                 { if (print_tokens) printf("STAR\n"); last_token_type = 0; }
"/"                                 { if (print_tokens) printf("DIV\n"); last_token_type = 0; }
"("                                 { if (print_tokens) printf("LPAR\n"); last_token_type = 0; }
")"                                 { if (print_tokens) printf("RPAR\n"); last_token_type = TOK_RPAR; }
"="                                 { if (print_tokens) printf("ASSIGN\n"); last_token_type = 0; }
","                                 { if (print_tokens) printf("COMMA\n"); last_token_type = 0; }
";"                                 { if (print_tokens) printf("SEMICOLON\n"); last_token_type = 0; }
"_"                                 { if (print_tokens) printf("BLANKID\n"); last_token_type = 0; }
">"                                 { if (print_tokens) printf("GT\n"); last_token_type = 0; }
"<"                                 { if (print_tokens) printf("LT\n"); last_token_type = 0; }
"%"                                 { if (print_tokens) printf("MOD\n"); last_token_type = 0; }
"!"                                 { if (print_tokens) printf("NOT\n"); last_token_type = 0; }
"{"                                 { if (print_tokens) printf("LBRACE\n"); last_token_type = 0; }
"}"                                 { if (print_tokens) printf("RBRACE\n"); last_token_type = TOK_RBRACE; }
"["                                 { if (print_tokens) printf("LSQ\n"); last_token_type = 0; }
"]"                                 { if (print_tokens) printf("RSQ\n"); last_token_type = TOK_RSQ; }
"=="                                  { if (print_tokens) printf("EQ\n"); last_token_type = 0; }
">="                                  { if (print_tokens) printf("GE\n"); last_token_type = 0; }
"<="                                  { if (print_tokens) printf("LE\n"); last_token_type = 0; }
"!="                                  { if (print_tokens) printf("NE\n"); last_token_type = 0; }
"&&"                                  { if (print_tokens) printf("AND\n"); last_token_type = 0; }
"||"                                  { if (print_tokens) printf("OR\n"); last_token_type = 0; }


int                                 { if (print_tokens) printf("INT\n"); last_token_type = 0; } /* Types */
float32                             { if (print_tokens) printf("FLOAT32\n"); last_token_type = 0; }
bool                                { if (print_tokens) printf("BOOL\n"); last_token_type = 0; }
string                              { if (print_tokens) printf("STRING\n"); last_token_type = 0; }


fmt.Println                         { if (print_tokens) printf("PRINT\n"); last_token_type = 0; } /* Functions */
strconv.Atoi                        { if (print_tokens) printf("PARSEINT\n"); last_token_type = 0; }
os.Args                             { if (print_tokens) printf("CMDARGS\n"); last_token_type = 0; }


break                               { reserved(); last_token_type = 0; } /* Reserved keywords */
case                                { reserved(); last_token_type = 0; }
chan                                { reserved(); last_token_type = 0; }
const                               { reserved(); last_token_type = 0; }
continue                            { reserved(); last_token_type = 0; }
default                             { reserved(); last_token_type = 0; }
defer                               { reserved(); last_token_type = 0; }
fallthrough                         { reserved(); last_token_type = 0; }
go                                  { reserved(); last_token_type = 0; }
goto                                { reserved(); last_token_type = 0; }
import                              { reserved(); last_token_type = 0; }
interface                           { reserved(); last_token_type = 0; }
map                                 { reserved(); last_token_type = 0; }
range                               { reserved(); last_token_type = 0; }
select                              { reserved(); last_token_type = 0; }
struct                              { reserved(); last_token_type = 0; }
switch                              { reserved(); last_token_type = 0; }
type                                { reserved(); last_token_type = 0; }
"++"                                  { reserved(); last_token_type = 0; }
"--"                                  { reserved(); last_token_type = 0; }

{digit}*"."{digit}+                 { if (print_tokens) printf("DECIMAL(%s)\n", yytext); last_token_type = TOK_DECIMAL; }
{digit}+"."{digit}*                 { if (print_tokens) printf("DECIMAL(%s)\n", yytext); last_token_type = TOK_DECIMAL; }
{digit}*"."{digit}*[eE][+-]?{digit}+ { if (print_tokens) printf("DECIMAL(%s)\n", yytext); last_token_type = TOK_DECIMAL; }
{digit}+[eE][+-]?{digit}+           { if (print_tokens) printf("DECIMAL(%s)\n", yytext); last_token_type = TOK_DECIMAL; }

[0][1-8]*[0-8]*                     { if (print_tokens) printf("NATURAL(%s)\n", yytext); last_token_type = TOK_NATURAL; }
[1-9]{digit}*                       { if (print_tokens) printf("NATURAL(%s)\n", yytext); last_token_type = TOK_NATURAL; }
[0xX]([a-fA-F])+                    { if (print_tokens) printf("NATURAL(%s)\n", yytext); last_token_type = TOK_NATURAL; }

{letter}({letter}|{digit})*         { if (print_tokens) printf("IDENTIFIER(%s)\n", yytext); last_token_type = TOK_IDENTIFIER; }

" "+                                { ; }
"\t"+                               { ; }
"\r"                                { ; }
"\n"                                { newline(); }

"/*"                                { BEGIN(COMMENT); }
<COMMENT>.                          { ; }
<COMMENT>"\n"                       { newline(); }
<COMMENT>"*/"                       { BEGIN(INITIAL); }
<COMMENT><<EOF>>                    { printf("Line %d, column %d: unterminated comment\n", line, column); return 1; } /* Handle unterminated comment */

\"                                  { BEGIN(STRING); }
<STRING>\"                          { BEGIN(INITIAL); last_token_type = TOK_STRLIT; }
<STRING>\\[frt\\\"]                { ; }
<STRING>\\[^fnrt\\\"]               { printf("Line %d, column %d: invalid escape sequence (%s)\n", line, tok_column, yytext); last_token_type = 0; }
<STRING>[^\\"]*                   { if (print_tokens) printf("STRLIT(\"%s\")\n", yytext); last_token_type = TOK_STRLIT; }
<STRING>\n                          { newline(); BEGIN(INITIAL); last_token_type = 0; }

.                                   { printf("Line %d, column %d: illegal character (%s)\n", line, tok_column, yytext); last_token_type = 0; }

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
    /* Insert SEMICOLON if the last token is one of the specified types */
    if (last_token_type == TOK_NATURAL || last_token_type == TOK_DECIMAL ||
        last_token_type == TOK_STRLIT || last_token_type == TOK_IDENTIFIER ||
        last_token_type == TOK_RETURN || last_token_type == TOK_RPAR ||
        last_token_type == TOK_RSQ || last_token_type == TOK_RBRACE) {
        printf("SEMICOLON\n");
    }
    line++;
    column = 1;
}

void reserved(){
  if (print_tokens) {
        printf("RESERVED\n");
    }
}
