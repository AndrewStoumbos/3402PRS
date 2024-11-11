 /* $Id: float.y,v 1.7 2023/11/01 02:25:45 leavens Exp $ */

%code top {
#include <stdio.h>
}

%code requires {

 /* Including "ast.h" must be at the top, to define the AST type */
#include "ast.h"
#include "machine_types.h"
#include "parser_types.h"
#include "lexer.h"

    /* Report an error to the user on stderr */
extern void yyerror(const char *filename, const char *msg);

}    /* end of %code requires */

%verbose
%define parse.lac full
%define parse.error detailed

 /* the following passes file_name to yyerror,
    and declares it as an formal parameter of yyparse. */
%parse-param { char const *file_name }

%token <ident> identsym
%token <number> numbersym
%token <token> plussym    "+"
%token <token> minussym   "-"
%token <token> multsym    "*"
%token <token> divsym     "/"

%token <token> semisym    ";"
%token <token> commasym   ","
%token <token> becomessym "="

%token <token> floatsym   "float"
%token <token> boolsym    "bool"
%token <token> beginsym   "{"
%token <token> endsym     "}"
%token <token> ifsym      "if"
%token <token> readsym    "read"
%token <token> writesym   "write"

%token <token> eqsym      "=="
%token <token> neqsym     "!="
%token <token> notsym     "!"
%token <token> ltsym      "<"
%token <token> leqsym     "<="
%token <token> gtsym      ">"
%token <token> geqsym     ">="
%token <token> lparensym  "("
%token <token> rparensym  ")"

%type <program> program

%type <var_decls> varDecls
%type <var_decl> varDecl
%type <idents> idents
%type <empty> empty

%type <stmt> stmt
%type <assign_stmt> assignStmt
%type <begin_stmt> beginStmt
%type <if_stmt> ifStmt
%type <read_stmt> readStmt
%type <write_stmt> writeStmt
%type <stmts> stmts
%type <expr> expr

%type <token> relOp
%type <expr> lterm
%type <expr> lfactor
%type <expr> term
%type <expr> factor

%start program

%code {
 /* extern declarations provided by the lexer */
extern int yylex(void);

 /* The AST for the program, set by the semantic action 
    for the nonterminal program. */
program_t progast; 

 /* Set the program's ast to be t */
extern void setProgAST(program_t t);
}

%%

program : varDecls stmt { program_t prog = ast_program($1, $2);
                          setProgAST(prog); } ;

varDecls : empty { $$ = ast_var_decls_empty($1); }
         | varDecls varDecl { $$ = ast_var_decls($1, $2); }
	 ;

empty : %empty
        { file_location *floc
		= file_location_make(lexer_filename(), lexer_line());
	  $$ = ast_empty(floc); }
        ;

varDecl : "float" idents ";" { $$ = ast_var_decl_float($2); }
        | "bool" idents ";" { $$ = ast_var_decl_bool($2); }
	;

idents : identsym { $$ = ast_idents_singleton($1); } 
       | idents "," identsym { $$ = ast_idents($1, $3); }
       ;

stmt : assignStmt { $$ = ast_stmt_assign($1); }
     | beginStmt { $$ = ast_stmt_begin($1); }
     | ifStmt { $$ = ast_stmt_if($1); }
     | readStmt { $$ = ast_stmt_read($1); }
     | writeStmt { $$ = ast_stmt_write($1); }
     ;

assignStmt : identsym "=" expr ";" { $$ = ast_assign_stmt($1,$3); } ;

beginStmt : "{" varDecls stmts "}" { $$ = ast_begin_stmt($2, $3); } ;

ifStmt : "if" "(" expr ")" stmt { $$ = ast_if_stmt($3, $5); } ;

readStmt : "read" identsym ";" { $$ = ast_read_stmt($2); } ;

writeStmt : "write" expr ";" { $$ = ast_write_stmt($2); } ;

stmts : stmt { $$ = ast_stmts_singleton($1); } 
      | stmts stmt { $$ = ast_stmts($1,$2); }
      ;

expr : lterm 
     | lterm relOp lterm
       { $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3)); }
     ;

lterm : lfactor
      | "!" lterm { $$ = ast_expr_logical_not($2); }
      ;

relOp : "=="
      | "!="
      | "<"
      | "<="
      | ">"
      | ">="
      ;

lfactor : term
        | lfactor "+" term
          { $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3)); }
        | lfactor "-" term
          { $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3)); }
        ;

term : factor
     | term "*" factor
       { $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3)); }
     | term "/" factor
       { $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3)); }
     ;

factor : identsym { $$ = ast_expr_ident($1); }
       | numbersym { $$ = ast_expr_number($1); }
       | "(" expr ")" { $$ = $2; }
       ;

%%

// Set the program's ast to be ast
void setProgAST(program_t ast) { progast = ast; }