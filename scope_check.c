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

// Build the symbol table for block
// and check for duplicate declarations
// or uses of undeclared identifiers
// Return the modified AST with id_use pointers
block_t scope_check_program(block_t block)
{
    symtab_enter_scope();
    scope_check_varDecls(block.var_decls);
    // need to update stmt's AST with id_use structs
    block.stmts = scope_check_stmts(block.stmts);
    symtab_leave_scope();
    return block;
}

// build the symbol table and check the declarations in vds
void scope_check_varDecls(var_decls_t vds)
{
    var_decl_t *vdp = vds.var_decls;
    while (vdp != NULL) {
	scope_check_varDecl(*vdp);
	vdp = vdp->next;
    }
}

// Add declarations for the names in vd,
// reporting duplicate declarations
void scope_check_varDecl(var_decl_t vd)
{
    scope_check_idents(vd.ident_list, vd.type_tag);
} 

// Add declarations for the names in ids
// to current scope as type t
// reporting any duplicate declarations
void scope_check_idents(ident_list_t ids, id_kind t)
{
    ident_t *idp = ids.start;
    while (idp != NULL) {
	scope_check_declare_ident(*idp, t);
	idp = idp->next;
    }
}

// Add declaration for id
// to current scope as type t
// reporting if it's a duplicate declaration
void scope_check_declare_ident(ident_t id, id_kind t)
{
    if (symtab_declared_in_current_scope(id.name)) {
        // only variables in BLOCK
	bail_with_prog_error(*(id.file_loc),
			     "Variable \"%s\" has already been declared!",
			     id.name);
    } else {
	int ofst_cnt = symtab_scope_loc_count();
	id_attrs *attrs = create_id_attrs(*(id.file_loc), t, ofst_cnt);
	symtab_insert(id.name, attrs);
    } 
}

// check the statements to make sure that
// all idenfifiers referenced in them have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
stmts_t scope_check_stmts(stmts_t stmts)
{
    switch (stmts.stmts_kind)
    {
        case empty_stmts_e:
        break;

        case stmt_list_e:
            stmt_t *sp = stmts.stmt_list.start;
            while (sp != NULL) 
            {
                *sp = scope_check_stmt(*sp);
                sp = sp->next;
            }
        break;

        default:
            bail_with_error("Call to scope_check_program with an AST that is not a statement for block.stmts.stmts_kind!");
        break;
    }
    return stmts;
}

// check the statement to make sure that
// all idenfifiers used have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
stmt_t scope_check_stmt(stmt_t stmt)
{
    switch (stmt.stmt_kind) 
    {
        case assign_stmt:
            stmt.data.assign_stmt
            = scope_check_assignStmt(stmt.data.assign_stmt);
        break;

        case call_stmt:
            stmt.data.call_stmt
            = scope_check_callStmt(stmt.data.call_stmt);
        break;

        case if_stmt:
            stmt.data.if_stmt
            = scope_check_ifStmt(stmt.data.if_stmt);
        break;

        case while_stmt:
            stmt.data.while_stmt
            = scope_check_whileStmt(stmt.data.while_stmt);
        break;

        case read_stmt:
            stmt.data.read_stmt
            = scope_check_readStmt(stmt.data.read_stmt);
        break;

        case print_stmt:
            stmt.data.print_stmt
            = scope_check_printStmt(stmt.data.print_stmt);
        break;

        case block_stmt:
            stmt.data.block_stmt
            = scope_check_blockStmt(stmt.data.block_stmt);
        break;
        
        default:
            bail_with_error("Call to scope_check_stmt with an AST that is not a statement for stmts.stmt_list.start->stmt_kind");
        break;
    }

    return stmt;
}

// check the statement for
// undeclared identifiers
// Return the modified AST with id_use pointers
assign_stmt_t scope_check_assignStmt(assign_stmt_t stmt)
{
    // stmt.idu = scope_check_ident_declared(*(stmt.file_loc), name); // DELETE

    id_use *temp = scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    stmt.type_tag = temp;
    
    assert(temp != NULL);  // since would bail if not declared
    *stmt.expr = scope_check_expr(*(stmt.expr));

    return stmt;
} // idu and assert arent proper field names

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
call_stmt_t scope_check_callStmt(call_stmt_t stmt)
{
    //stmt.idu = scope_check_ident_declared(*(stmt.file_loc), stmt.name); // DELETE

    id_use *temp = scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    stmt.type_tag = temp;

    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
if_stmt_t scope_check_ifStmt(if_stmt_t stmt)
{
    *(stmt.then_stmts) = scope_check_stmts(*(stmt.then_stmts));
    *(stmt.else_stmts) = scope_check_stmts(*(stmt.else_stmts));

    //stmt.expr = scope_check_expr(stmt.expr);       // DELETE
    //*(stmt.body) = scope_check_stmt(*(stmt.body)); // DELETE

    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
while_stmt_t scope_check_whileStmt(while_stmt_t stmt)
{
    *(stmt.body) = scope_check_stmts(*(stmt.body));

    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
read_stmt_t scope_check_readStmt(read_stmt_t stmt)
{
    //stmt.idu = scope_check_ident_declared(*(stmt.file_loc), stmt.name); // DELETE

    id_use *temp = scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    stmt.type_tag = temp;

    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
print_stmt_t scope_check_printStmt(print_stmt_t stmt)
{
    // id_use *temp = scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    // stmt.type_tag = temp;
    // NOT SURE WHAT TO DO HERE ^^^^

    stmt.expr = scope_check_expr(stmt.expr);

    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
block_stmt_t scope_check_blockStmt(block_stmt_t stmt)
{
    *(stmt.block) = scope_check_program(*(stmt.block));
    
    return stmt;
}

// check the expresion to make sure that
// all idenfifiers used have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
expr_t scope_check_expr(expr_t exp)
{
    switch (exp.expr_kind) 
    {
        case expr_bin:
            exp.data.binary
            = scope_check_binary_op_expr(exp.data.binary);
        break;

        case expr_ident:
            exp.data.ident
            = scope_check_ident_expr(exp.data.ident);
        break;

        case expr_number:
            // no identifiers are possible in this case, so just return
        break;

        case expr_negated:
            exp.data.negated
            = scope_check_negated_expr(exp.data.negated);
        break;

      //  case expr_logical_not:
      //      *(exp.data.logical_not)
      //      = scope_check_expr(*(exp.data.logical_not));
      //  break;

        default:
            bail_with_error("Unexpected expr_kind_e (%d) in scope_check_expr",
                exp.expr_kind);
        break;
    }
    return exp;
} // expr_bin_op and expr_logical_not is not proper cases

// check that all identifiers used in exp
// have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
binary_op_expr_t scope_check_binary_op_expr(binary_op_expr_t exp)
{
    *(exp.expr1) = scope_check_expr(*(exp.expr1));
    // (note: no identifiers can occur in the operator)
    *(exp.expr2) = scope_check_expr(*(exp.expr2));
    return exp;
}

negated_expr_t scope_check_negated_expr(negated_expr_t exp)
{

}

// check the identifier (id) to make sure that
// all it has been declared (if not, then produce an error)
// Return the modified AST with id_use pointers
ident_t scope_check_ident_expr(ident_t id)
{
    //id.idu = scope_check_ident_declared(*(id.file_loc), id.name);

    id_use *temp = scope_check_ident_declared(*(id.file_loc), id.name);
    id.type_tag = temp;

    return id;
} // .idu is not a proper field name

// check that name has been declared,
// if so, then return an id_use for it
// otherwise, produce an error 
id_use *scope_check_ident_declared(file_location floc, const char *name)
{
    id_use *ret = symtab_lookup(name);
    if (ret == NULL) 
    {
	    bail_with_prog_error(floc, "identifier \"%s\" is not declared!", name);
    }
    
    assert(id_use_get_attrs(ret) != NULL);
    return ret;
}