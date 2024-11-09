#include "scope_check.h"
#include "symtab.h"
#include <stdio.h>
#include <string.h>

// Function to perform scope checking on the block's declarations
void scope_check_declarations(const_decls_t const_decls, var_decls_t var_decls, proc_decls_t proc_decls) {
    // Check constants
    const_decl_t *current_const_decl = const_decls.start;
    while (current_const_decl != NULL) {
        const_def_list_t const_def_list = current_const_decl->const_def_list;
        const_def_t *current_const_def = const_def_list.start;

        while (current_const_def != NULL) {
            // Check if the constant name is already in the symbol table
            const char *const_name = current_const_def->ident.name; // Assuming ident has name field
            symbol_t *symbol = symtab_lookup(const_name);
            if (symbol != NULL) {
                // Handle redeclaration of the constant
                printf("Error: Constant '%s' is redeclared.\n", const_name);
            } else {
                // Insert the constant into the symbol table
                symtab_insert(const_name, VAR_SYMBOL); // Assuming constants are treated as variables in the symbol table
            }
            current_const_def = current_const_def->next;
        }
        current_const_decl = current_const_decl->next;
    }

    // Check variables
    var_decl_t *current_var_decl = var_decls.var_decls;
    while (current_var_decl != NULL) {
        ident_list_t ident_list = current_var_decl->ident_list;
        ident_t *current_ident = ident_list.start;

        while (current_ident != NULL) {
            // Check if the variable name is already in the symbol table
            const char *var_name = current_ident->name; // Assuming ident has name field
            symbol_t *symbol = symtab_lookup(var_name);
            if (symbol != NULL) {
                // Handle redeclaration of the variable
                printf("Error: Variable '%s' is redeclared.\n", var_name);
            } else {
                // Insert the variable into the symbol table
                symtab_insert(var_name, VAR_SYMBOL);
            }
            current_ident = current_ident->next;
        }
        current_var_decl = current_var_decl->next;
    }

    // Check procedures
    proc_decl_t *current_proc_decl = proc_decls.proc_decls;
    while (current_proc_decl != NULL) {
        const char *proc_name = current_proc_decl->name;
        // Check if the procedure name is already in the symbol table
        symbol_t *symbol = symtab_lookup(proc_name);
        if (symbol != NULL) {
            // Handle redeclaration of the procedure
            printf("Error: Procedure '%s' is redeclared.\n", proc_name);
        } else {
            // Insert the procedure into the symbol table
            symtab_insert(proc_name, FUNC_SYMBOL);
        }
        current_proc_decl = current_proc_decl->next;
    }
}

// Function to perform scope checking for statements
void scope_check_statements(stmt_list_t stmt_list) {
    stmt_t *current_stmt = stmt_list.start;
    while (current_stmt != NULL) {
        if (current_stmt->stmt_kind == assign_stmt) {
            assign_stmt_t assign_stmt = current_stmt->data.assign_stmt;
            if (symtab_lookup(assign_stmt.name) == NULL) {
                printf("Error: Variable '%s' not declared\n", assign_stmt.name);
            }
        } else if (current_stmt->stmt_kind == call_stmt) {
            call_stmt_t call_stmt = current_stmt->data.call_stmt;
            if (symtab_lookup(call_stmt.name) == NULL) {
                printf("Error: Procedure '%s' not declared\n", call_stmt.name);
            }
        }
        current_stmt = current_stmt->next;
    }
}

// Function to perform scope checking on the block
void scope_check_program(block_t block) {
    // Start by checking the declarations in the block
    scope_check_declarations(block.const_decls, block.var_decls, block.proc_decls);

    // Now check the statements in the block
    scope_check_statements(block.stmts.stmt_list);
}
