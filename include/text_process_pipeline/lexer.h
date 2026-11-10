#pragma once

#include <stddef.h>

#define TOKEN_LIST_CAPACITY 32

// --- Data structs ---
// Comprises of all token types
typedef enum{
    TOK_WORD,            // anything that is not an operator
    TOK_PIPE,            // |
    TOK_OR,              // ||
    TOK_BG,              // &
    TOK_AND,             // &&
    TOK_SEMI,            // ;
    TOK_REDIR_IN,        // <
    TOK_HEREDOC,         // <<
    TOK_REDIR_OUT,       // >
    TOK_APPEND           // >>
} TokenType;

// Token struct
typedef struct{
    TokenType type;
    char* value; // malloc mem
    size_t start;
} Token;

typedef struct{
    Token* tokens;
    size_t count;
    size_t capacity;
} TokenList;

// Free tokens
void free_tokens(TokenList *list);
// Print all tokens
void print_tokens(const TokenList *list);
// --- The lexer for splitting the text into tokens ---
int lex(const char* text, TokenList* out, char* err, size_t err_size);