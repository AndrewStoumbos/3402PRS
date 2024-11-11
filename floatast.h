/* $Id: ast.h,v 1.24 2024/10/25 00:55:18 leavens Exp leavens $ */
#ifndef _AST_H
#define _AST_H
#include <stdbool.h>
#include "machine_types.h"
#include "type_exp.h"
#include "file_location.h"

// forward declaration of id_use
typedef struct id_use_s id_use;

// The following types for structs named N_t
// are returned by the parser.
// The struct N_t is the type of information kept in the AST
// that is related to the nonterminal N in the abstract syntax.


// The generic struct type (generic_t) has the fields that
// should be in all alternatives for ASTs.
typedef struct {
    file_location *file_loc;
    void *next; // for lists
} generic_t;

// empty ::=
typedef struct {
    file_location *file_loc;
} empty_t;

// ident
typedef struct ident_s {
    file_location *file_loc;
    struct ident_s *next; // for lists
    const char *name;
    id_use *idu;
} ident_t;

// (possibly signed) floating point numbers
typedef struct {
    file_location *file_loc;
    const char *text;
    type_exp_e type;
    word_type value;
} number_t;

// tokens as ASTs
typedef struct {
    file_location *file_loc;
    const char *text;
    int code;
} token_t;

// kinds of expressions
typedef enum {expr_bin_op, expr_ident,
              expr_number,
	      expr_logical_not
} expr_kind_e;

// forward declaration for expressions
struct expr_s;

// expr ::= expr op expr
// op ::= == | != | < | <= | + | - | * | /
typedef struct {
    file_location *file_loc;
    type_exp_e type;
    struct expr_s *expr1;
    token_t op;
    struct expr_s *expr2;
} binary_op_expr_t;
    
// expr ::= expr op expr | ident | number
typedef struct expr_s {
    file_location *file_loc;
    expr_kind_e expr_kind;
    union expr_u {
	binary_op_expr_t binary;
	ident_t ident;
	number_t number;
	struct expr_s *logical_not;
    } data;
} expr_t;

// idents ::= { ident }
typedef struct {
    file_location *file_loc;
    ident_t *idents;
} idents_t;

// varDecl ::= varType ident
typedef struct var_decl_s {
    file_location *file_loc;
    struct var_decl_s *next; // for lists
    type_exp_e type;
    idents_t idents;
} var_decl_t;

// var-decls ::= { varDecl }
typedef struct {
    file_location *file_loc;
    var_decl_t *var_decls;
} var_decls_t;

// kinds of statements
typedef enum {assign_stmt, begin_stmt,
              if_stmt, read_stmt,
              write_stmt
} stmt_kind_e;

// forward declaration
// of statement AST struct, for recursions
struct stmt_s;

// assignStmt ::= ident = expr
typedef struct {
    file_location *file_loc;
    const char *name;
    struct expr_s *expr;
    id_use *idu;
} assign_stmt_t;

// stmts ::= { stmt }
typedef struct {
    file_location *file_loc;
    struct stmt_s *stmts;
} stmts_t;

// beginStmt ::= begin varDecls stmts 
typedef struct {
    file_location *file_loc;
    var_decls_t var_decls;
    stmts_t stmts;
} begin_stmt_t;

// ifStmt ::= if expr stmt
typedef struct {
    file_location *file_loc;
    expr_t expr;
    struct stmt_s *body;
} if_stmt_t;

// readStmt ::= read ident
typedef struct {
    file_location *file_loc;
    const char *name;
    id_use *idu;
} read_stmt_t;

// writeStmt ::= write expr
typedef struct {
    file_location *file_loc;
    expr_t expr;
} write_stmt_t;

// stmt ::= assignStmt | beginStmt
//       | ifStmt | readStmt | writeStmt
typedef struct stmt_s {
    file_location *file_loc;
    struct stmt_s *next; // for lists
    stmt_kind_e stmt_kind;
    union {
	assign_stmt_t assign_stmt;
	begin_stmt_t begin_stmt;
	if_stmt_t if_stmt;
	read_stmt_t read_stmt;
	write_stmt_t write_stmt;
    } data;
} stmt_t;

// program ::= varDecls stmt
typedef struct block_s {
    file_location *file_loc;
    var_decls_t var_decls;
    stmt_t stmt;
} program_t;

// The AST type used by bison
typedef union AST_u {
    generic_t generic;
    var_decls_t var_decls;
    var_decl_t var_decl;
    idents_t idents;
    stmt_t stmt;
    assign_stmt_t assign_stmt;
    begin_stmt_t begin_stmt;
    if_stmt_t if_stmt;
    read_stmt_t read_stmt;
    write_stmt_t write_stmt;
    stmts_t stmts;
    expr_t expr;
    binary_op_expr_t binary_op_expr;
    token_t token;
    number_t number;
    ident_t ident;
    empty_t empty;
} AST;

// Return the file location from an AST
extern file_location *ast_file_loc(AST t);

// Return the filename from the AST t
extern const char *ast_filename(AST t);

// Return the line number from the AST t
extern unsigned int ast_line(AST t);

// Return an AST for the program
extern program_t ast_program(var_decls_t vds, stmt_t stmt);

// Return an AST for varDecls that are empty
extern var_decls_t ast_var_decls_empty(empty_t empty);

// Return an AST varDecls that have a var_decl
extern var_decls_t ast_var_decls(var_decls_t var_decls, var_decl_t var_decl);

// Return an AST for a float var decl
extern var_decl_t ast_var_decl_float(idents_t idents);

// Return an AST for a bool var decl
extern var_decl_t ast_var_decl_bool(idents_t idents);

// Return an AST made for one ident
extern idents_t ast_idents_singleton(ident_t ident);

// Return an AST made for idents
extern idents_t ast_idents(idents_t idents, ident_t ident);

// Return an AST for a write statement
extern write_stmt_t ast_write_stmt(expr_t expr); 

// Return an AST for a read statement
extern read_stmt_t ast_read_stmt(ident_t ident); 

// Return an AST for an if statement
extern if_stmt_t ast_if_stmt(expr_t expr, stmt_t body);

// Return an AST for a begin statement
// containing the given list of statements
extern begin_stmt_t ast_begin_stmt(var_decls_t vds, stmts_t stmts);

// Return an AST for an assignment statement
extern assign_stmt_t ast_assign_stmt(ident_t ident, expr_t expr);

// Return an AST for the list of statements 
extern stmts_t ast_stmts_singleton(stmt_t stmt);

// Return an AST for the list of statements 
extern stmts_t ast_stmts(stmts_t stmts, stmt_t stmt);

// Return an AST for the given statment
extern stmt_t ast_stmt_assign(assign_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_begin(begin_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_if(if_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_read(read_stmt_t s);

// Return an AST for the given statment
extern stmt_t ast_stmt_write(write_stmt_t s);

// Return the type of the given expression.
extern type_exp_e ast_expr_type(expr_t exp);

// Return an AST for a binary op expression
extern binary_op_expr_t ast_binary_op_expr(expr_t expr1, token_t arith_op,
					   expr_t expr2);

// Return an expression AST for a binary operation expresion
extern expr_t ast_expr_binary_op(binary_op_expr_t e);

// Return an expression AST for an identifier
extern expr_t ast_expr_ident(ident_t e);

// Return an expression AST for a number
extern expr_t ast_expr_number(number_t number);

// Return an expression AST for the logical negation of exp
extern expr_t ast_expr_logical_not(expr_t exp);

// Return an AST for the given token
extern token_t ast_token(file_location *file_loc, const char *text, int code);

// Return an AST for an identifier
// found in the file named fn, on line ln, with the given name.
extern ident_t ast_ident(file_location *file_loc, const char *name);

// Return an AST for a (signed) number with the given value
extern number_t ast_number(token_t sgn, word_type value);

// Return an AST for empty found in the given file location
extern empty_t ast_empty(file_location *file_loc);

// Requires: lst is a pointer to a non-circular 
//           linked list with next pointers
//           as in generic_t
// Return a pointer to the last element in lst.
// This only returns NULL if lst == NULL.
extern void *ast_last_list_elem(void *lst);

// Requires: lst is a pointer to a non-circular 
//           linked list with next pointers
//           as in generic_t
// Return the number of elements in the linked list lst
extern int ast_list_length(void *lst);

// Requires: lst is a pointer to a non-circular 
//           linked list with next pointers
//           as in generic_t
// Is lst empty?
extern bool ast_list_is_empty(void *lst);

#endif