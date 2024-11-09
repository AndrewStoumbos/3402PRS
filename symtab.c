#include "symtab.h"
#include <stdlib.h>
#include <string.h>

// Global symbol table
symtab_t *global_symtab = NULL;

// Hash function (for demonstration)
unsigned int symtab_hash(const char *name) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash * 31) + *name++;
    }
    return hash % SYMTAB_SIZE;
}

// Initialize the global symbol table
void symtab_initialize(void) {
    global_symtab = (symtab_t *)malloc(sizeof(symtab_t));
    if (!global_symtab) {
        // Handle memory allocation failure
        exit(EXIT_FAILURE);
    }
    memset(global_symtab, 0, sizeof(symtab_t));
}

// Destroy the symbol table
void symtab_destroy(void) {
    if (global_symtab) {
        // Free all symbols in the table
        for (int i = 0; i < SYMTAB_SIZE; i++) {
            symbol_t *symbol = global_symtab->buckets[i];
            while (symbol) {
                symbol_t *next = symbol->next;
                free(symbol);
                symbol = next;
            }
        }
        free(global_symtab);
        global_symtab = NULL;
    }
}

// Insert a symbol into the global symbol table
void symtab_insert(const char *name, symbol_type_e type) {
    if (!global_symtab) {
        symtab_initialize();
    }

    unsigned int index = symtab_hash(name);
    symbol_t *new_symbol = (symbol_t *)malloc(sizeof(symbol_t));
    if (!new_symbol) {
        // Handle memory allocation failure
        exit(EXIT_FAILURE);
    }

    new_symbol->name = name;
    new_symbol->type = type;
    new_symbol->is_declared = false;
    new_symbol->is_defined = false;
    new_symbol->next = global_symtab->buckets[index];
    global_symtab->buckets[index] = new_symbol;
}

// Look up a symbol by its name in the global symbol table
symbol_t *symtab_lookup(const char *name) {
    if (!global_symtab) {
        return NULL;
    }

    unsigned int index = symtab_hash(name);
    symbol_t *symbol = global_symtab->buckets[index];

    while (symbol) {
        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
        symbol = symbol->next;
    }

    return NULL;
}

// Remove a symbol from the global symbol table
void symtab_remove(const char *name) {
    if (!global_symtab) {
        return;
    }

    unsigned int index = symtab_hash(name);
    symbol_t *symbol = global_symtab->buckets[index];
    symbol_t *prev = NULL;

    while (symbol) {
        if (strcmp(symbol->name, name) == 0) {
            if (prev) {
                prev->next = symbol->next;
            } else {
                global_symtab->buckets[index] = symbol->next;
            }
            free(symbol);
            return;
        }
        prev = symbol;
        symbol = symbol->next;
    }
}
