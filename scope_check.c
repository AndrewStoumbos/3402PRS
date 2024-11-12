/* $Id: scope_check.c,v 1.20 2023/11/13 14:10:00 leavens Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scope_check.h"
#include "id_attrs.h"
#include "file_location.h"
#include "ast.h"
#include "utilities.h"
#include "symtab.h"
#include "scope_check.h"

// Build the symbol table for the program block and check for duplicate declarations
// or uses of undeclared identifiers. Return the modified AST with id_use pointers.
block_t scope_check_program(block_t block) {
    symtab_enter_scope();
    scope_check_constDecls(block.const_decls);
    scope_check_varDecls(block.var_decls);
    scope_check_procDecls(block.proc_decls);
    block.stmts = scope_check_stmts(block.stmts);
    symtab_leave_scope();
    return block;
}

// Build the symbol table and check the constant declarations in cds.
void scope_check_constDecls(const_decls_t cds) {
    const_decl_t* cdp = cds.start;
    while (cdp != NULL) {
        scope_check_constDecl(*cdp);
        cdp = cdp->next;
    }
}

// Add declarations for the names in cd, reporting duplicate declarations.
void scope_check_constDecl(const_decl_t cd) {
    scope_check_const_def_list(cd.const_def_list);
}

// Check each constant definition in cdl.
void scope_check_const_def_list(const_def_list_t cdl) {
    const_def_t* cdp = cdl.start;
    while (cdp != NULL) {
        scope_check_const_def(*cdp);
        cdp = cdp->next;
    }
}

// Add a declaration for each identifier in a constant definition.
void scope_check_const_def(const_def_t cd) {
    ident_t idp = cd.ident;
    scope_check_declare_ident(idp, cd.type_tag);
}

// Build the symbol table and check the variable declarations in vds.
void scope_check_varDecls(var_decls_t vds) {
    var_decl_t* vdp = vds.var_decls;
    while (vdp != NULL) {
        scope_check_varDecl(*vdp);
        vdp = vdp->next;
    }
}

// Add declarations for the identifiers in vd, reporting duplicate declarations.
void scope_check_varDecl(var_decl_t vd) {
    scope_check_idents(vd.ident_list, vd.type_tag);
}

// Add each identifier in ids to the current scope as type t, reporting any duplicate declarations.
void scope_check_idents(ident_list_t ids, id_kind t) {
    ident_t* idp = ids.start;
    while (idp != NULL) {
        scope_check_declare_ident(*idp, t);
        idp = idp->next;
    }
}

// Declare identifier id in the current scope with type t, checking for duplicates.
void scope_check_declare_ident(ident_t id, id_kind t) {
    if (symtab_declared_in_current_scope(id.name)) {
        bail_with_prog_error(*(id.file_loc), "Identifier \"%s\" is already declared in the current scope.", id.name);
    } else {
        int offset = symtab_scope_loc_count();
        id_attrs* attrs = create_id_attrs(*(id.file_loc), t, offset);
        symtab_insert(id.name, attrs);
    }
}

// Build the symbol table and check the procedure declarations in pds.
void scope_check_procDecls(proc_decls_t pds) {
    proc_decl_t* pdp = pds.proc_decls;
    while (pdp != NULL) {
        scope_check_procDecl(*pdp);
        pdp = pdp->next;
    }
}

// Add declarations for the procedure names in pd, checking their block scope.
void scope_check_procDecl(proc_decl_t pd) {
    symtab_enter_scope();
    *(pd.block) = scope_check_program(*(pd.block));
    symtab_leave_scope();
}

// Check statements in stmts for undeclared identifiers and return the modified AST.
stmts_t scope_check_stmts(stmts_t stmts) {
    switch (stmts.stmts_kind) {
    case empty_stmts_e:
        // No statements, simply return
        break;
    case stmt_list_e:
        stmts.stmt_list = scope_check_stmt_list(stmts.stmt_list);
        break;
    default:
        bail_with_error("Invalid stmts_kind in scope_check_stmts!");
        break;
    }
    return stmts;
}

// Process each statement in stmt_list for undeclared identifiers.
stmt_list_t scope_check_stmt_list(stmt_list_t sl) {
    stmt_t* sp = sl.start;
    while (sp != NULL) {
        *sp = scope_check_stmt(*sp);
        sp = sp->next;
    }
    return sl;
}

// Check a single statement for undeclared identifiers.
stmt_t scope_check_stmt(stmt_t stmt) {
    switch (stmt.stmt_kind) {
    case assign_stmt:
        stmt.data.assign_stmt = scope_check_assignStmt(stmt.data.assign_stmt);
        break;
    case call_stmt:
        stmt.data.call_stmt = scope_check_callStmt(stmt.data.call_stmt);
        break;
    case if_stmt:
        stmt.data.if_stmt = scope_check_ifStmt(stmt.data.if_stmt);
        break;
    case while_stmt:
        stmt.data.while_stmt = scope_check_whileStmt(stmt.data.while_stmt);
        break;
    case read_stmt:
        stmt.data.read_stmt = scope_check_readStmt(stmt.data.read_stmt);
        break;
    case print_stmt:
        stmt.data.print_stmt = scope_check_printStmt(stmt.data.print_stmt);
        break;
    case block_stmt:
        stmt.data.block_stmt = scope_check_blockStmt(stmt.data.block_stmt);
        break;
    default:
        bail_with_error("Invalid stmt_kind in scope_check_stmt!");
        break;
    }
    return stmt;
}

// Check if identifiers in assignStmt are declared and update AST.
assign_stmt_t scope_check_assignStmt(assign_stmt_t stmt) {
    stmt.name = scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    if (stmt.expr != NULL) {
        *stmt.expr = scope_check_expr(*(stmt.expr));
    }
    return stmt;
}

// Check if identifiers in callStmt are declared.
call_stmt_t scope_check_callStmt(call_stmt_t stmt) {
    stmt.name = scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    return stmt;
}

// Check if identifiers in ifStmt are declared and update AST.
if_stmt_t scope_check_ifStmt(if_stmt_t stmt) {
    stmt.condition = scope_check_condition(stmt.condition);
    if (stmt.then_stmts != NULL) {
        *(stmt.then_stmts) = scope_check_stmts(*(stmt.then_stmts));
    }
    if (stmt.else_stmts != NULL) {
        *(stmt.else_stmts) = scope_check_stmts(*(stmt.else_stmts));
    }
    return stmt;
}

// Check if identifiers in whileStmt are declared and update AST.
while_stmt_t scope_check_whileStmt(while_stmt_t stmt) {
    stmt.condition = scope_check_condition(stmt.condition);
    if (stmt.stmts != NULL) {
        *(stmt.stmts) = scope_check_stmts(*(stmt.stmts));
    }
    return stmt;
}

// Check identifiers in readStmt.
read_stmt_t scope_check_readStmt(read_stmt_t stmt) {
    stmt.name = scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    return stmt;
}

// Check identifiers in printStmt.
print_stmt_t scope_check_printStmt(print_stmt_t stmt) {
    stmt.expr = scope_check_expr(stmt.expr);
    return stmt;
}

// Check if identifiers in condition are declared and update AST.
condition_t scope_check_condition(condition_t cond) {
    switch (cond.cond_kind) {
    case ck_db:
        cond.data.db_cond = scope_check_db_condition(cond.data.db_cond);
        break;
    case ck_rel:
        cond.data.rel_op_cond = scope_check_rel_op_condition(cond.data.rel_op_cond);
        break;
    default:
        bail_with_error("Invalid cond_kind in scope_check_condition!");
        break;
    }
    return cond;
}
