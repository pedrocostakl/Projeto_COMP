
%{
    /*
    **  Pedro Sousa da Costa - 2022220304
    **  Marco Manuel Almeida e Silva - 2021211653
    */

    #include <stdarg.h>
    #include <string.h>

    #include "y.tab.h"
    #include "ast.h"
    
    #define BUF_SIZE 8192

    void user_action();
    void print_token(const char *tok, ...);
    int emit_semicolon();
    void newline();
    void reserved();

    int line = 1;                     /* Stores the current line */
    int tok_line = 1;
    int column = 1;                   /* Stores the current column */
    int tok_column = 1;               /* Stores the column where the current token starts */
    int comment_line = 0;             /* Stores the line where the comment starts */
    int comment_column = 0;           /* Stores the column where the comment starts */
    int string_column = 1;            /* Auxiliary variable to store the column where the string starts */
    int valid_string = 0;             /* Flag to determine if the string should be printed */
    int semicolon = 0;                /* Used to know when to emit semicolons */
    int print_tokens = 0;             /* Global flag to print tokens if -l is passed */
    int parse = 0;                    /* Enables the returning of token values to the parser */
    char temp_string[BUF_SIZE];

    extern struct node_t *program;
    extern int syntax_error_flag;

    #define YY_USER_ACTION user_action();
    #define PASS_TOKEN() yylval.lexeme = strdup(yytext);

%}

unicode_letter    [a-zA-Z]
letter            {unicode_letter}|"_"
digit             [0-9]
octal_digit       [0-7]
hex_digit         [0-9a-fA-F]
reserved          break|case|chan|const|continue|default|defer|fallthrough|go|goto|import|interface|map|range|select|struct|switch|type|"++"|"--"

%X COMMENT STRING

%%

"//".*                                      { /* Ignore the comment until the end of the line */ }

else                                        { print_token("ELSE"); semicolon = 0; if (parse) return ELSE; } /* Reserved keywords */
for                                         { print_token("FOR"); semicolon = 0; if (parse) return FOR; }
if                                          { print_token("IF"); semicolon = 0; if (parse) return IF; }
package                                     { print_token("PACKAGE"); semicolon = 0; if (parse) return PACKAGE; }
return                                      { print_token("RETURN"); semicolon = 1; if (parse) return RETURN; }
var                                         { print_token("VAR"); semicolon = 0; if (parse) return VAR; }
func                                        { print_token("FUNC"); semicolon = 0; if (parse) return FUNC; }

"+"                                         { print_token("PLUS"); semicolon = 0; if (parse) return PLUS; } /* Operators and punctuation */
"-"                                         { print_token("MINUS"); semicolon = 0; if (parse) return MINUS; }
"*"                                         { print_token("STAR"); semicolon = 0; if (parse) return STAR; }
"/"                                         { print_token("DIV"); semicolon = 0; if (parse) return DIV; }
"("                                         { print_token("LPAR"); semicolon = 0; if (parse) return LPAR; }
")"                                         { print_token("RPAR"); semicolon = 1; if (parse) return RPAR; }
"="                                         { print_token("ASSIGN"); semicolon = 0; if (parse) return ASSIGN; }
","                                         { print_token("COMMA"); semicolon = 0; if (parse) return COMMA; }
";"                                         { print_token("SEMICOLON"); semicolon = 0; if (parse) return SEMICOLON; }
"_"                                         { print_token("BLANKID"); semicolon = 0; if (parse) return BLANKID; }
">"                                         { print_token("GT"); semicolon = 0; if (parse) return GT; }
"<"                                         { print_token("LT"); semicolon = 0; if (parse) return LT; }
"%"                                         { print_token("MOD"); semicolon = 0; if (parse) return MOD; }
"!"                                         { print_token("NOT"); semicolon = 0; if (parse) return NOT; }
"{"                                         { print_token("LBRACE"); semicolon = 0; if (parse) return LBRACE; }
"}"                                         { print_token("RBRACE"); semicolon = 1; if (parse) return RBRACE; }
"["                                         { print_token("LSQ"); semicolon = 0; if (parse) return LSQ; }
"]"                                         { print_token("RSQ"); semicolon = 1; if (parse) return RSQ; }
"=="                                        { print_token("EQ"); semicolon = 0; if (parse) return EQ; }
">="                                        { print_token("GE"); semicolon = 0; if (parse) return GE; }
"<="                                        { print_token("LE"); semicolon = 0; if (parse) return LE; }
"!="                                        { print_token("NE"); semicolon = 0; if (parse) return NE; }
"&&"                                        { print_token("AND"); semicolon = 0; if (parse) return AND; }
"||"                                        { print_token("OR"); semicolon = 0; if (parse) return OR; }

int                                         { print_token("INT"); semicolon = 0; if (parse) return INT; } /* Types */
float32                                     { print_token("FLOAT32"); semicolon = 0; if (parse) return FLOAT32; }
bool                                        { print_token("BOOL"); semicolon = 0; if (parse) return BOOL; }
string                                      { print_token("STRING"); semicolon = 0; if (parse) return STR; }

fmt.Println                                 { print_token("PRINT"); semicolon = 0; if (parse) return PRINT; } /* Functions */
strconv.Atoi                                { print_token("PARSEINT"); semicolon = 0; if (parse) return PARSEINT; }
os.Args                                     { print_token("CMDARGS"); semicolon = 0; if (parse) return CMDARGS; }

{reserved}                                  { reserved(); if (parse) return RESERVED; } /* Reserved keywords */

{letter}({letter}|{digit})*                 { print_token("IDENTIFIER(%s)", yytext); semicolon = 1; PASS_TOKEN(); if (parse) return IDENTIFIER; }

[1-9]{digit}*                               { print_token("NATURAL(%s)", yytext); semicolon = 1; PASS_TOKEN(); if (parse) return NATURAL; } /*naturals don't start with 0, only octals do*/
[0][xX]{hex_digit}+                         { print_token("NATURAL(%s)", yytext); semicolon = 1; PASS_TOKEN(); if (parse) return NATURAL; }
0{octal_digit}*                             { print_token("NATURAL(%s)", yytext); semicolon = 1; PASS_TOKEN(); if (parse) return NATURAL; }
0{digit}+                                   { printf("Line %d, column %d: invalid octal constant (%s)\n", line, tok_column, yytext); }

{digit}*"."{digit}+                         { print_token("DECIMAL(%s)", yytext); semicolon = 1; PASS_TOKEN(); if (parse) return DECIMAL; }
{digit}+"."{digit}*                         { print_token("DECIMAL(%s)", yytext); semicolon = 1; PASS_TOKEN(); if (parse) return DECIMAL; }
{digit}*"."{digit}*[eE][-+]?{digit}+        { print_token("DECIMAL(%s)", yytext); semicolon = 1; PASS_TOKEN(); if (parse) return DECIMAL; }
{digit}+[eE][-+]?{digit}+                   { print_token("DECIMAL(%s)", yytext); semicolon = 1; PASS_TOKEN(); if (parse) return DECIMAL; }

" "+                                        { ; }
"\t"+                                       { ; }
"\r"                                        { ; }
"\n"                                        { newline(); if (emit_semicolon()) { if (parse) return SEMICOLON; } }

"/*"                                        { BEGIN(COMMENT); comment_line = line; comment_column = tok_column; }
<COMMENT>.                                  { ; }
<COMMENT>"\n"                               { newline(); }
<COMMENT>"*/"                               { BEGIN(INITIAL); }
<COMMENT><<EOF>>                            {
                                                user_action(); 
                                                BEGIN(INITIAL); 
                                                printf("Line %d, column %d: unterminated comment\n", comment_line, comment_column); 
                                                semicolon = 0; 
                                            }

\"                                          {
                                                BEGIN(STRING); 
                                                string_column = tok_column; 
                                                temp_string[0] = '\"';
                                                temp_string[1] = '\0';
                                                //temp_string[0] = '\0';
                                                valid_string = 1;
                                            }
<STRING>\"                                  { 
                                                BEGIN(INITIAL); 
                                                if (valid_string) {
                                                    strcat(temp_string, "\"");
                                                    print_token("STRLIT(%s)", temp_string); 
                                                    semicolon = 1; 
                                                    yylval.lexeme = strdup(temp_string);
                                                    yytext = temp_string;
                                                    tok_column = string_column;
                                                    if (parse) return STRLIT; 
                                                } 
                                            }
<STRING>\\\n                                { 
                                                BEGIN(INITIAL); 
                                                printf("Line %d, column %d: invalid escape sequence (%c)\n", line, tok_column, yytext[0]); 
                                                printf("Line %d, column %d: unterminated string literal\n", line, string_column);
                                                valid_string = 0;
                                                semicolon = 0;
                                                newline();
                                            }
<STRING>\n                                  { 
                                                BEGIN(INITIAL); 
                                                printf("Line %d, column %d: unterminated string literal\n", line, string_column); 
                                                semicolon = 0; 
                                                newline();
                                            }
<STRING>\\[fnrt\\\"]                        {
                                                /* Valid escape sequences: \f, \n, \r, \t, \\, \" */
                                                strcat(temp_string, yytext); 
                                            }
<STRING>\\[^fnrt\\]                         {
                                                /* Print the error, but stay in STRING state */
                                                printf("Line %d, column %d: invalid escape sequence (%s)\n", line, tok_column, yytext); 
                                                valid_string = 0; 
                                            }
<STRING>[^\\\n"]+                           {
                                                /* Regular characters inside the string */
                                                strcat(temp_string, yytext); 
                                            }
<STRING>.                                   {
                                                /* Handle any illegal characters */
                                                printf("Line %d, column %d: illegal character in string literal (%s)\n", line, tok_column, yytext);
                                            }
<STRING><<EOF>>                             {
                                                user_action();
                                                BEGIN(INITIAL);
                                                printf("Line %d, column %d: unterminated string literal\n", line, string_column);
                                            }

<<EOF>>                                     { if (emit_semicolon()) { if (parse) return SEMICOLON; } return 0; }  /* Handle end of file */
.                                           { printf("Line %d, column %d: illegal character (%s)\n", line, tok_column, yytext); }

%%

extern int yylex();
extern int yyparse();

int main(int argc, char **argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "-l") == 0) {
            print_tokens = 1;  /* Enable token printing if -l option is passed */
            yylex();
            return 0;
        }
        if (strcmp(argv[1], "-t") == 0) {
            //yydebug = 1;//apagar antes de submeter no
            parse = 1;
            yyparse();
            if (syntax_error_flag == 0) {
                show(program, 0, 0);
            }
            return 0;
        }
    }
    /* Running the yyparse in here will lead to mooshak test failure */
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

int emit_semicolon() {
    if (semicolon) {
        print_token("SEMICOLON");  /* Emit a semicolon if the last token requires it */
        semicolon = 0;     /* Reset the flag */
        return 1;
    }
    return 0;
}

void newline() {
    tok_line = line;
    line++;
    column = 1;
    tok_column = 1;
}

void reserved(){
    print_token("RESERVED(%s)", yytext);
    semicolon = 0;
}
