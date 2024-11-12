/* $Id: scope_check.h,v 1.8 2023/11/13 14:10:00 leavens Exp $ */
#ifndef _SCOPE_CHECK_H
#define _SCOPE_CHECK_H
#include "ast.h"
#include "id_use.h"

// idents_t is undefined
// type_exp_e is undefined
// begin_stmt_t is undefined
// write_stmt_t is undefined

// Build the symbol table for the given program AST
// and check the given program AST for duplicate declarations
// or uses of identifiers that were not declared
extern block_t scope_check_program(block_t block);

// build the symbol table and check the declarations in vds
extern void scope_check_constDecls(const_decls_t cds);
extern void scope_check_constDecl(const_decl_t cd);
extern void scope_check_const_def_list(const_def_list_t cdl);
extern void scope_check_const_def(const_def_t cd);

// build the symbol table and check the declarations in vds
extern void scope_check_varDecls(var_decls_t vds);
extern void scope_check_varDecl(var_decl_t vd);

// build the symbol table and check the declarations in vds
extern void scope_check_procDecls(proc_decls_t pds);
extern void scope_check_procDecl(proc_decl_t pd);

// Add declarations to the current scope's symbol table for vd,
// producing errors for duplicate declarations
extern void scope_check_varDecl(var_decl_t vd);

// Add declarations for the names in ids to the current scope's symbol table,
// for variables of the type t,
// producing errors for any duplicate declarations
extern void scope_check_idents(ident_list_t ids, id_kind t);

// Add a declaration of the name id.name with the type t
// to the current scope's symbol table,
// producing an error if this would be a duplicate declaration
extern void scope_check_declare_ident(ident_t id, id_kind t);

// check the statements to make sure that
// all idenfifiers referenced in them have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
extern stmts_t scope_check_stmts(stmts_t stmts);
extern stmt_list_t scope_check_stmt_list(stmt_list_t sl);

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
extern stmt_t scope_check_stmt(stmt_t stmt);

extern condition_t scope_check_condition(condition_t cond);


// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
extern assign_stmt_t scope_check_assignStmt(assign_stmt_t stmt);

extern call_stmt_t scope_check_callStmt(call_stmt_t stmt);

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
extern if_stmt_t scope_check_ifStmt(if_stmt_t stmt);

extern while_stmt_t scope_check_whileStmt(while_stmt_t stmt);

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
extern read_stmt_t scope_check_readStmt(read_stmt_t stmt);

extern print_stmt_t scope_check_printStmt(print_stmt_t stmt);

extern block_stmt_t scope_check_blockStmt(block_stmt_t stmt);

// check the expresion to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
extern expr_t scope_check_expr(expr_t exp);

// check the expression (exp) to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
extern binary_op_expr_t scope_check_binary_op_expr(binary_op_expr_t exp);

extern negated_expr_t scope_check_negated_expr(negated_expr_t exp);

// check the identifier (id) to make sure that
// all it has been declared (if not, then produce an error)
// Return the modified AST with id_use pointers
extern ident_t scope_check_ident_expr(ident_t id);

// check that the given name has been declared,
// if so, then return an id_use (containing the attributes) for that name,
// otherwise, produce an error using the file_location (floc) given.
// Return the modified AST with id_use pointers
extern id_use *scope_check_ident_declared(file_location floc, const char *name);

extern db_condition_t scope_check_db_condition(db_condition_t cond);

extern rel_op_condition_t scope_check_rel_op_condition(rel_op_condition_t cond);

#endif