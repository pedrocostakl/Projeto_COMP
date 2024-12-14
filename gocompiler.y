%{

    /*
    **  Pedro Sousa da Costa - 2022220304
    **  Marco Manuel Almeida e Silva - 2021211653
    */

    #include <stdio.h>
    #include <string.h>

    #include "ast.h"

    extern int yylex(void);
    void yyerror(char *);

    extern char *yytext;
    extern int line;                 /* Line externa do ficheiro lex */
    extern int column;               /* Column externa do ficheiro lex */
    extern int tok_line;
    extern int tok_column;
    extern int last_column;
    extern int last_line;
    extern int last_action_newline;
    int syntax_error_flag = 0;
    struct node_t *program;
    struct node_t *type;

%}

%token<pass> SEMICOLON
%token<pass> COMMA
%token<pass> BLANKID
%token<pass> ASSIGN
%token<pass> STAR
%token<pass> DIV
%token<pass> MINUS
%token<pass> PLUS
%token<pass> EQ
%token<pass> GE
%token<pass> GT
%token<pass> LBRACE
%token<pass> LE
%token<pass> LPAR
%token<pass> LSQ
%token<pass> LT
%token<pass> MOD
%token<pass> NE
%token<pass> NOT
%token<pass> AND
%token<pass> OR
%token<pass> RBRACE
%token<pass> RPAR
%token<pass> RSQ
%token<pass> PACKAGE
%token<pass> RETURN
%token<pass> ELSE
%token<pass> FOR
%token<pass> IF
%token<pass> VAR
%token<pass> INT
%token<pass> FLOAT32
%token<pass> BOOL
%token<pass> STR
%token<pass> PRINT
%token<pass> PARSEINT
%token<pass> FUNC
%token<pass> CMDARGS
%token<pass> RESERVED


%token<pass> IDENTIFIER STRLIT NATURAL DECIMAL

%type<node> program type declarations var_declaration var_spec func_declaration func_header parameters parameter
%type<node> func_body vars_statements statement block block_statements parse_args func_invocation func_invocation_exprs expr

%left LOW
%right ASSIGN
%left OR
%left AND
%left EQ NE LT GT LE GE
%left PLUS MINUS  // Binary plus and minus with left associativity
%left DIV STAR MOD
%right UMINUS UPLUS   // Declare precedence for unary minus and plus
%right NOT
%nonassoc LPAR RPAR
%left HIGH

%union {
    struct pass_t pass;
    struct node_t *node;
}

%%

program
: PACKAGE IDENTIFIER SEMICOLON declarations
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = program = newcategory(Program,pass);
    addchild(program, $4);
}
| PACKAGE IDENTIFIER SEMICOLON
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = program = newcategory(Program,pass);
}
;

declarations
: var_declaration SEMICOLON declarations
{
    $$ = newintermediate();
    addchild($$, $1);
    addchild($$, $3);
}
| var_declaration SEMICOLON
{
    $$ = $1;
}
| func_declaration SEMICOLON declarations
{
    $$ = newintermediate();
    addchild($$, $1);
    addchild($$, $3);
}
| func_declaration SEMICOLON
{
    $$ = $1;
}
;

var_declaration
: VAR var_spec
{
    $$ = $2;
}
| VAR LPAR var_spec SEMICOLON RPAR
{
    $$ = $3;
}
;

var_spec
: IDENTIFIER type
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(VarDecl,pass);
    addchild($$, $2);
    addchild($$, newnode(Identifier, $1));
}
| IDENTIFIER COMMA var_spec
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newintermediate();
    struct node_t *vardecl = newcategory(VarDecl,pass);
    addchild(vardecl, newcategory(type->category,pass));
    addchild(vardecl, newnode(Identifier, $1));
    addchild($$, vardecl);
    addchild($$, $3);
}
;

func_declaration
: FUNC func_header func_body
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(FuncDecl,pass);
    addchild($$, $2);
    addchild($$, $3);
}
;

func_header
: IDENTIFIER LPAR parameters RPAR type
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(FuncHeader,pass);
    addchild($$, newnode(Identifier, $1));
    addchild($$, $5);
    struct node_t *params = newcategory(FuncParams,pass);
    addchild(params, $3);
    addchild($$, params);
}
| IDENTIFIER LPAR parameters RPAR
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(FuncHeader,pass);
    addchild($$, newnode(Identifier, $1));
    struct node_t *params = newcategory(FuncParams,pass);
    addchild(params, $3);
    addchild($$, params);

}
| IDENTIFIER LPAR RPAR type
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(FuncHeader,pass);
    addchild($$, newnode(Identifier, $1));
    addchild($$, $4);
    addchild($$, newcategory(FuncParams,pass));
}
| IDENTIFIER LPAR RPAR
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(FuncHeader,pass);
    addchild($$, newnode(Identifier, $1));
    addchild($$, newcategory(FuncParams,pass));
}
;

parameters
: parameter
{
    $$ = $1;
}
| parameters COMMA parameter
{
    $$ = newintermediate();
    addchild($$, $1);
    addchild($$, $3);
}
;

parameter
: IDENTIFIER type
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(ParamDecl,pass);
    addchild($$, $2);
    addchild($$, newnode(Identifier, $1));
}
;

func_body
: LBRACE vars_statements RBRACE
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(FuncBody,pass);
    addchild($$, $2);
}
| LBRACE RBRACE
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(FuncBody,pass);
}
;

vars_statements
: vars_statements var_declaration SEMICOLON
{
    $$ = newintermediate();
    addchild($$, $1);
    addchild($$, $2);
}
| vars_statements statement SEMICOLON
{
    $$ = newintermediate();
    addchild($$, $1);
    addchild($$, $2);
}
| var_declaration SEMICOLON
{
    $$ = $1;
}
| statement SEMICOLON
{
    $$ = $1;
}
| vars_statements SEMICOLON
{
    $$ = $1;
}
| SEMICOLON 
{
    $$ = NULL;
}
;

statement
: IDENTIFIER ASSIGN expr
{
    $$ = newnode(Assign, $2);
    addchild($$, newnode(Identifier, $1));
    addchild($$, $3);
}
| block
{
    $$ = $1;
}
| IF expr block
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newnode(If, $1);
    addchild($$, $2);
    addchild($$, $3);
    addchild($$, newcategory(Block,pass));
}
| IF expr block ELSE block
{
    $$ = newnode(If, $1);
    addchild($$, $2);
    addchild($$, $3);
    addchild($$, $5);
}
| FOR expr block
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(For,pass);
    addchild($$, $2);
    addchild($$, $3);
}
| FOR block
{
    
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(For,pass);
    addchild($$, $2);
}
| RETURN expr
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(Return, pass);
    addchild($$, $2);
}
| RETURN
{
    $$ = newcategory(Return,$1);
}
| func_invocation
{
    $$ = $1;
}
| parse_args
{
    $$ = $1;
}
| PRINT LPAR expr RPAR
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    struct node_t *aux_node = newnode(Intermediate, pass);
    aux_node = $3;
    pass.line = aux_node->line;
    pass.column = aux_node->column;

    $$ = newcategory(Print,pass);
    addchild($$, $3);
}
| PRINT LPAR STRLIT RPAR
{
    $$ = newcategory(Print,$1);
    addchild($$, newnode(StrLit, $3));
}
| error
{
    $$ = NULL;
}
;

block
: LBRACE RBRACE
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(Block,pass);
}
| LBRACE block_statements RBRACE
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    $$ = newcategory(Block,pass);
    addchild($$, $2);
}
;

block_statements
: statement SEMICOLON
{
    $$ = $1;
}
|  block_statements statement SEMICOLON 
{
    $$ = newintermediate();
    addchild($$, $1);
    addchild($$, $2);
}
;

parse_args
: IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ expr RSQ RPAR
{
    
    $$ = newcategory(ParseArgs,$5);
    addchild($$, newnode(Identifier, $1));
    addchild($$, $9);
}
| IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR
{
    $$ = NULL;
}
;

func_invocation
: IDENTIFIER LPAR RPAR
{
    /*struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;*/
    $$ = newcategory(Call,$1);
    addchild($$, newnode(Identifier, $1));
}
| IDENTIFIER LPAR func_invocation_exprs RPAR
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    //struct node_t *aux_node = newnode(Intermediate, pass);
    pass = $1;
    pass.line = pass.line ;
    
    $$ = newcategory(Call,pass);
    addchild($$, newnode(Identifier, $1));
    addchild($$, $3);
}
| IDENTIFIER LPAR error RPAR
{
    $$ = NULL;
}
;

func_invocation_exprs
: expr
{
    $$ = $1;
}
| func_invocation_exprs COMMA expr
{
    $$ = newintermediate();
    addchild($$, $1);
    addchild($$, $3);
}
;

expr
: expr OR expr
{
    $$ = newnode(Or, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr AND expr
{
    $$ = newnode(And, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr LT expr
{
    $$ = newnode(Lt, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr GT expr
{
    $$ = newnode(Gt, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr EQ expr
{
    $$ = newnode(Eq, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr NE expr
{
    $$ = newnode(Ne, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr LE expr
{
    $$ = newnode(Le, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr GE expr
{
    $$ = newnode(Ge, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr PLUS expr
{
    $$ = newnode(Add, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr MINUS expr    // Binary Minus
{
    $$ = newnode(Sub, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr STAR expr
{
    $$ = newnode(Mul, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr DIV expr
{
    $$ = newnode(Div, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| expr MOD expr
{
    $$ = newnode(Mod, $2);
    addchild($$, $1);
    addchild($$, $3);
}
| NOT expr
{
    $$ = newnode(Not, $1);
    addchild($$, $2);
}
| MINUS expr %prec UMINUS  // Unary Minus with explicit precedence
{
    $$ = newnode(Minus, $1);
    addchild($$, $2);
}
| PLUS expr %prec UPLUS  // Unary Plus with explicit precedence
{
    $$ = newnode(Plus, $1);
    addchild($$, $2);
}
| NATURAL
{
    $$ = newnode(Natural, $1);
}
| DECIMAL
{
    $$ = newnode(Decimal, $1);
}
| IDENTIFIER
{
    $$ = newnode(Identifier, $1);
}
| func_invocation
{
    $$ = $1;
}
| LPAR expr RPAR
{
    $$ = $2;
}
| LPAR error RPAR
{
    $$ = NULL;
}
;

type
: INT
{ 
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    type = newcategory(Int,pass);
    $$ = type;
}
| FLOAT32
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    type = newcategory(Float32,pass);
    $$ = type;
}
| BOOL
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    type = newcategory(Bool,pass);
    $$ = type;
}
| STR
{
    struct pass_t pass;
    pass.token = strdup("return");  // Optionally set the token to "return"
    pass.line = @1.first_line;      // Use the position of the RETURN keyword
    pass.column = @1.first_column;
    type = newcategory(String,pass);
    $$ = type;
}
;

%%

void yyerror(char *error) {
    syntax_error_flag = 1;  // Set the error flag to 1
    if (last_action_newline == 0) {
        printf("Line %d, column %d: %s: %s\n", line, tok_column, error, yytext);
    } else {
        printf("Line %d, column %d: %s: %s\n", line-1, last_column-1, error, yytext);
    }
}
