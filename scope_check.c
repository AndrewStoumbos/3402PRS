#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scope_check.h"
#include "id_attrs.h"
#include "file_location.h"
#include "ast.h"
#include "utilities.h"
#include "symtab.h"

// Build the symbol table for a program block and check declarations
// Return: Modified AST with id_use pointers updated
block_t scope_check_program(block_t block) 
{
    symtab_enter_scope(); // Enter a new scope for the program block
    
    // Check constant, variable, and procedure declarations
    scope_check_constDecls(block.const_decls);
    scope_check_varDecls(block.var_decls);
    scope_check_procDecls(block.proc_decls);
    
    // Check statements within the block
    block.stmts = scope_check_stmts(block.stmts);
    
    symtab_leave_scope(); // Exit the scope after processing
    return block;
}

// Process constant declarations
// Checks for duplicate declarations
void scope_check_constDecls(const_decls_t cds) 
{
    const_decl_t *cdp1 = cds.start;
    
    // Iterate over each constant declaration
    while (cdp1 != NULL) 
    {
        scope_check_constDecl(*cdp1);
        cdp1 = cdp1->next;
    }
}

// Process a single constant declaration for duplicates
void scope_check_constDecl(const_decl_t cd) 
{
    scope_check_const_def_list(cd.const_def_list);
}

// Process a list of constant definitions
void scope_check_const_def_list(const_def_list_t cdl) 
{
    const_def_t *cdp2 = cdl.start;
    
    // Iterate over each constant definition in the list
    while (cdp2 != NULL) 
    {
        scope_check_const_def(*cdp2);
        cdp2 = cdp2->next;
    }
}

// Process a single constant definition by declaring its identifier
void scope_check_const_def(const_def_t cd) 
{
    if (cd.ident.name != NULL) 
    {
        scope_check_declare_ident(cd.ident, constant_idk);
    } 
    else if (cd.ident.file_loc != NULL) 
    {
        bail_with_prog_error(*(cd.ident.file_loc), "Identifier name is NULL");
    }
}

// Process variable declarations in the block
void scope_check_varDecls(var_decls_t vds) {
    var_decl_t *vdp = vds.var_decls;
    
    // Iterate over each variable declaration
    while (vdp != NULL) 
    {
        scope_check_varDecl(*vdp);
        vdp = vdp->next;
    }
}

// Process a single variable declaration by declaring its identifiers
void scope_check_varDecl(var_decl_t vd) 
{
    scope_check_idents(vd.ident_list, variable_idk);
}

// Declare each identifier in the list as a specific type
void scope_check_idents(ident_list_t ids, id_kind t) 
{
    ident_t *idp = ids.start;
    
    // Iterate over identifiers in the list
    while (idp != NULL) 
    {
        scope_check_declare_ident(*idp, t);
        idp = idp->next;
    }
}

// Declare an identifier in the current scope and report duplicates
void scope_check_declare_ident(ident_t id, id_kind t) 
{
    if (id.name == NULL) 
    {
        if (id.file_loc != NULL) 
        {
            bail_with_prog_error(*(id.file_loc), "Identifier name is NULL");
        }
        return;
    }

    // Check if the identifier is already declared in the current scope
    if (symtab_declared_in_current_scope(id.name)) 
    {
        if (id.file_loc != NULL)
        {
            id_use *ident_id = symtab_lookup(id.name);
            if (ident_id != NULL && ident_id->attrs != NULL) 
            {
                bail_with_prog_error(*(id.file_loc), "%s \"%s\" is already declared as a %s",
                                     kind2str(t), id.name, kind2str(ident_id->attrs->kind));
            }
        }
        return;
    }

    // Declare the identifier with its attributes
    if (id.file_loc != NULL) 
    {
        int ofst_cnt = symtab_scope_loc_count();
        id_attrs *attrs = create_id_attrs(*(id.file_loc), t, ofst_cnt);
        if (attrs != NULL) symtab_insert(id.name, attrs);
    }
}

// Process procedure declarations in the block
void scope_check_procDecls(proc_decls_t pds) 
{
    proc_decl_t *pdp = pds.proc_decls;
    
    // Iterate over each procedure declaration
    while (pdp != NULL) 
    {
        scope_check_procDecl(*pdp);
        pdp = pdp->next;
    }
}

// Process a single procedure declaration and check for duplicates
void scope_check_procDecl(proc_decl_t pd) 
{
    if (pd.name == NULL) 
    {
        if (pd.file_loc != NULL) 
        {
            bail_with_prog_error(*(pd.file_loc), "Procedure name is NULL");
        }
        return;
    }

    if (symtab_declared_in_current_scope(pd.name)) 
    {
        if (pd.block != NULL) 
        {
            *pd.block = scope_check_program(*pd.block);
        } 
        else if (pd.file_loc != NULL) 
        {
            bail_with_prog_error(*(pd.file_loc), "Procedure block is NULL for procedure %s", pd.name);
        }
    } 
    else 
    {
        int ofst_cnt = symtab_scope_loc_count();
        id_attrs *attrs = create_id_attrs(*(pd.file_loc), procedure_idk, ofst_cnt);
        symtab_insert(pd.name, attrs);
    }
}

// Check all statements within a block
// Returns the modified AST with id_use pointers
stmts_t scope_check_stmts(stmts_t stmts) 
{
    switch (stmts.stmts_kind) 
    {
        case empty_stmts_e:
            // No statements to check
            break;

        case stmt_list_e:
            stmts.stmt_list = scope_check_stmt_list(stmts.stmt_list);
            break;

        default:
            bail_with_error("Invalid AST in scope_check_stmts for stmts.stmts_kind!");
            break;
    }

    return stmts;
}

// Process a list of statements
stmt_list_t scope_check_stmt_list(stmt_list_t sl) 
{
    stmt_t *sp = sl.start;

    while (sp != NULL) 
    {
        *sp = scope_check_stmt(*sp);
        sp = sp->next;
    }

    return sl;
}

// Check individual statement for undeclared identifiers
stmt_t scope_check_stmt(stmt_t stmt) 
{
    switch (stmt.stmt_kind) 
    {
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
            bail_with_error("Invalid AST in scope_check_stmt for stmt_kind!");
            break;
    }

    return stmt;
}

// check the condition to make sure that
// all idenfifiers used have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
condition_t scope_check_condition(condition_t cond)
{
    switch (cond.cond_kind) 
    {
        case ck_db:
            cond.data.db_cond = scope_check_db_condition(cond.data.db_cond);
            break;

        case ck_rel:
            cond.data.rel_op_cond = scope_check_rel_op_condition(cond.data.rel_op_cond);
            break;
        
        default:
            bail_with_error("Call to scope_check_condition with an AST that is not a statement for cond.cond_kind");
            break;
    }

    return cond;
}

// check the statement for
// undeclared identifiers
// Return the modified AST with id_use pointers
assign_stmt_t scope_check_assignStmt(assign_stmt_t stmt)
{
    (void) scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    if (stmt.expr == NULL) 
    {
        bail_with_prog_error(*(stmt.file_loc), "Expression is NULL in statement");
        return stmt;
    }
    // Perform the expression scope check
    if (stmt.expr != NULL) 
    {
        *stmt.expr = scope_check_expr(*(stmt.expr));
    }

    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
call_stmt_t scope_check_callStmt(call_stmt_t stmt)
{
    (void) scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
if_stmt_t scope_check_ifStmt(if_stmt_t stmt)
{
    stmt.condition = scope_check_condition(stmt.condition);

    if(stmt.then_stmts != NULL)
    {
        *(stmt.then_stmts) = scope_check_stmts(*(stmt.then_stmts));
    }
    if(stmt.else_stmts != NULL)
    {
        *(stmt.else_stmts) = scope_check_stmts(*(stmt.else_stmts));
    }       

    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
while_stmt_t scope_check_whileStmt(while_stmt_t stmt)
{
    stmt.condition = scope_check_condition(stmt.condition);
    if (stmt.body != NULL) 
    {
        *(stmt.body) = scope_check_stmts(*(stmt.body));
    }

    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
read_stmt_t scope_check_readStmt(read_stmt_t stmt)
{
    (void) scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
print_stmt_t scope_check_printStmt(print_stmt_t stmt)
{
    stmt.expr = scope_check_expr(stmt.expr);
    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
block_stmt_t scope_check_blockStmt(block_stmt_t stmt)
{
    assert(stmt.block != NULL);  // since would bail if not declared
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
            exp.data.binary = scope_check_binary_op_expr(exp.data.binary);
            break;

        case expr_ident:
            exp.data.ident = scope_check_ident_expr(exp.data.ident);
            break;

        case expr_number:
            // no identifiers are possible in this case, so just return
            break;

        case expr_negated:
            exp.data.negated = scope_check_negated_expr(exp.data.negated);
            break;

        default:
            bail_with_error("Unexpected expr_kind_e (%d) in scope_check_expr", exp.expr_kind);
            break;
    }

    return exp;
}

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
    *(exp.expr) = scope_check_expr(*(exp.expr));
    return exp;
}

// check the identifier (id) to make sure that
// all it has been declared (if not, then produce an error)
// Return the modified AST with id_use pointers
ident_t scope_check_ident_expr(ident_t exp)
{
    (void) scope_check_ident_declared(*(exp.file_loc), exp.name);
    return exp;
}

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
    
    assert(id_use_create(ret->attrs, ret->levelsOutward) != NULL);
    return ret;
}

// Check relational operator conditions for declaration of identifiers.
// Returns the modified AST with id_use pointers.
db_condition_t scope_check_db_condition(db_condition_t cond)
{
    cond.dividend = scope_check_expr(cond.dividend);
    cond.divisor = scope_check_expr(cond.divisor);

    return cond;
}

// Check relational operator conditions for declaration of identifiers.
// Returns the modified AST with id_use pointers.
rel_op_condition_t scope_check_rel_op_condition(rel_op_condition_t cond)
{
    cond.expr1 = scope_check_expr(cond.expr1);
    cond.expr2 = scope_check_expr(cond.expr2);

    return cond;
}