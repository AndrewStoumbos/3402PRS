#ifndef _SCOPE_CHECK_H
#define _SCOPE_CHECK_H

#include "ast.h"
#include "id_use.h"

//--------------------------------------------------------------
// Program-level Scope Checking
//--------------------------------------------------------------

/**
 * Build the symbol table for the program AST (block)
 * and check for duplicate declarations or undeclared identifier usage.
 */
extern block_t scope_check_program(block_t block);

//--------------------------------------------------------------
// Constant Declarations Scope Checking
//--------------------------------------------------------------

/**
 * Build the symbol table and check constant declarations.
 */
extern void scope_check_constDecls(const_decls_t cds);
extern void scope_check_constDecl(const_decl_t cd);
extern void scope_check_const_def_list(const_def_list_t cdl);
extern void scope_check_const_def(const_def_t cd);

//--------------------------------------------------------------
// Variable Declarations Scope Checking
//--------------------------------------------------------------

/**
 * Build the symbol table and check variable declarations.
 */
extern void scope_check_varDecls(var_decls_t vds);
extern void scope_check_varDecl(var_decl_t vd);

//--------------------------------------------------------------
// Procedure Declarations Scope Checking
//--------------------------------------------------------------

/**
 * Build the symbol table and check procedure declarations.
 */
extern void scope_check_procDecls(proc_decls_t pds);
extern void scope_check_procDecl(proc_decl_t pd);

//--------------------------------------------------------------
// Identifier Declarations Scope Checking
//--------------------------------------------------------------

/**
 * Add declarations for identifiers to the current scope's symbol table.
 * - Check identifiers for duplicate declarations.
 * - Declare a single identifier with a specified kind.
 */
extern void scope_check_idents(ident_list_t ids, id_kind t);
extern void scope_check_declare_ident(ident_t id, id_kind t);

//--------------------------------------------------------------
// Statements Scope Checking
//--------------------------------------------------------------

/**
 * Check all statements to ensure identifiers referenced have been declared.
 * Returns the modified AST with id_use pointers where applicable.
 */
extern stmts_t scope_check_stmts(stmts_t stmts);
extern stmt_list_t scope_check_stmt_list(stmt_list_t sl);
extern stmt_t scope_check_stmt(stmt_t stmt);
extern assign_stmt_t scope_check_assignStmt(assign_stmt_t stmt);
extern call_stmt_t scope_check_callStmt(call_stmt_t stmt);
extern if_stmt_t scope_check_ifStmt(if_stmt_t stmt);
extern while_stmt_t scope_check_whileStmt(while_stmt_t stmt);
extern read_stmt_t scope_check_readStmt(read_stmt_t stmt);
extern print_stmt_t scope_check_printStmt(print_stmt_t stmt);
extern block_stmt_t scope_check_blockStmt(block_stmt_t stmt);

//--------------------------------------------------------------
// Expressions Scope Checking
//--------------------------------------------------------------

/**
 * Check all expressions to ensure identifiers referenced have been declared.
 * Returns the modified AST with id_use pointers where applicable.
 */
extern expr_t scope_check_expr(expr_t exp);
extern binary_op_expr_t scope_check_binary_op_expr(binary_op_expr_t exp);
extern negated_expr_t scope_check_negated_expr(negated_expr_t exp);

//--------------------------------------------------------------
// Identifier Usage Verification
//--------------------------------------------------------------

/**
 * Verify that identifiers used in expressions or statements have been declared.
 * - Returns the modified AST with id_use pointers for identifiers.
 * - Produces an error for undeclared identifiers.
 */
extern ident_t scope_check_ident_expr(ident_t exp);
extern id_use *scope_check_ident_declared(file_location floc, const char *name);

//--------------------------------------------------------------
// Conditions Scope Checking
//--------------------------------------------------------------

/**
 * Check condition expressions to ensure identifiers referenced have been declared.
 * Returns the modified AST with id_use pointers.
 */
extern condition_t scope_check_condition(condition_t cond);
extern db_condition_t scope_check_db_condition(db_condition_t cond);
extern rel_op_condition_t scope_check_rel_op_condition(rel_op_condition_t cond);

#endif /* _SCOPE_CHECK_H */
