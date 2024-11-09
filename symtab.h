#ifndef _SYMTAB_H
#define _SYMTAB_H

#include <stdbool.h>
#include "machine_types.h"
#include "ast.h"

// Define a struct for the symbols
typedef enum { VAR_SYMBOL, FUNC_SYMBOL } symbol_type_e;

typedef struct symbol_s {
    const char *name;         // Symbol name
    symbol_type_e type;       // Type: variable or function
    bool is_declared;         // Whether the symbol is declared
    bool is_defined;          // Whether the symbol is defined (only for functions)
    // Additional info (e.g., data types, etc.)
    // We can add more fields here as needed.
    struct symbol_s *next;    // Next symbol in the hash table bucket (for collision resolution)
} symbol_t;

// Define the symbol table itself (usually a hash table)
#define SYMTAB_SIZE 100
typedef struct {
    symbol_t *buckets[SYMTAB_SIZE]; // Array of symbol lists (hash table)
    // Additional fields if needed (e.g., current scope stack)
} symtab_t;

// Global symbol table
extern symtab_t *global_symtab;

// Function declarations

// Initialize a new symbol table
void symtab_initialize(void);

// Destroy the symbol table (free all allocated memory)
void symtab_destroy(void);

// Insert a symbol into the symbol table
void symtab_insert(const char *name, symbol_type_e type);

// Look up a symbol by its name in the symbol table
symbol_t *symtab_lookup(const char *name);

// Remove a symbol from the symbol table (e.g., for handling scope exit)
void symtab_remove(const char *name);

// Utility functions (e.g., hash function)
unsigned int symtab_hash(const char *name);

#endif /* _SYMTAB_H */
