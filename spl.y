 /* $Id: bison_spl_y_top.y,v 1.3 2024/10/31 19:24:47 leavens Exp leavens $ */

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

%token <token> periodsym  "."
%token <token> semisym    ";"
%token <token> eqsym      "="
%token <token> commasym   ","
%token <token> becomessym ":="
%token <token> lparensym  "("
%token <token> rparensym  ")"

%token <token> constsym   "const"
%token <token> varsym     "var"
%token <token> procsym    "proc"
%token <token> callsym    "call"
%token <token> beginsym   "begin"
%token <token> endsym     "end"
%token <token> ifsym      "if"
%token <token> thensym    "then"
%token <token> elsesym    "else"
%token <token> whilesym   "while"
%token <token> dosym      "do"
%token <token> readsym    "read"
%token <token> printsym   "print"
%token <token> divisiblesym "divisible"
%token <token> bysym      "by"

%token <token> eqeqsym    "=="
%token <token> neqsym     "!="
%token <token> ltsym      "<"
%token <token> leqsym     "<="
%token <token> gtsym      ">"
%token <token> geqsym     ">="

%type <block> program

%type <block> block

%type <const_decls> constDecls
%type <const_decl> constDecl
%type <const_def_list> constDefList
%type <const_def> constDef

%type <var_decls> varDecls
%type <var_decl> varDecl
%type <ident_list> identList

%type <proc_decls> procDecls
%type <proc_decl> procDecl


%type <stmts> stmts
%type <empty> empty
%type <stmt_list> stmtList
%type <stmt> stmt
%type <assign_stmt> assignStmt
%type <call_stmt> callStmt
%type <if_stmt> ifStmt
%type <while_stmt> whileStmt
%type <read_stmt> readStmt
%type <print_stmt> printStmt
%type <block_stmt> blockStmt

%type <condition> condition
%type <db_condition> dbCondition
%type <rel_op_condition> relOpCondition
%type <token> relOp

%type <expr> expr
%type <expr> term
%type <expr> factor
%type <token> sign

%start program

%code {
 /* extern declarations provided by the lexer */
extern int yylex(void);

 /* The AST for the program, set by the semantic action 
    for the nonterminal program. */
block_t progast; 

 /* Set the program's ast to be t */
extern void setProgAST(block_t t);
}

%%

program : varDecls stmts { program_t prog = ast_program($1, $2); setProgAST(prog); }
;

stmts : stmt { $$ = ast_stmts_singleton($1); }
      | stmts stmt { $$ = ast_stmts($1, $2); }
;

stmt : assignStmt { $$ = ast_stmt_assign($1); }
     | beginStmt { $$ = ast_stmt_begin($1); }
     | ifStmt { $$ = ast_stmt_if($1); }
     | whileStmt { $$ = ast_stmt_while($1); }
     | readStmt { $$ = ast_stmt_read($1); }
     | printStmt { $$ = ast_stmt_print($1); }
;

ifStmt : ifsym "(" expr ")" stmt { $$ = ast_if_stmt($3, $5); }
       | ifsym "(" expr ")" stmt elsesym stmt { $$ = ast_if_else_stmt($3, $5, $7); }
;

whileStmt : whilesym "(" expr ")" stmt { $$ = ast_while_stmt($3, $5); }
;

printStmt : printsym expr ";" { $$ = ast_print_stmt($2); }
;

expr : lterm
     | lterm relOp lterm { $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3)); }
;

lterm : lfactor
      | "!" lterm { $$ = ast_expr_logical_not($2); }
;

lfactor : term
        | lfactor "+" term { $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3)); }
        | lfactor "-" term { $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3)); }
;

term : factor
     | term "*" factor { $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3)); }
     | term "/" factor { $$ = ast_expr_binary_op(ast_binary_op_expr($1, $2, $3)); }
;

factor : identsym { $$ = ast_expr_ident($1); }
       | numbersym { $$ = ast_expr_number($1); }
       | "(" expr ")" { $$ = $2; }
;

%%

// Set the program's ast to be ast
void setProgAST(block_t ast) { progast = ast; }
